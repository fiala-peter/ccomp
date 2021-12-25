#include "code_generator.h"
#include "lexer.h"
#include "parser.h"
#include "preproc.h"
#include "symbol_table.h"
#include "type.h"

#include <fstream>
#include <cstring>

int main(int argc, char *argv[])
{
	char const *asmname = "a.s";   // name of output
	char const *lexname = nullptr; // name of lex file
	char const *astname = nullptr; // name of syntax tree file
	char const *prepname = "a.prep";

	if (argc < 2)
	{
		std::cout << "Usage: " << argv[0] << " [options] <input name>\n";
		return 0;
	}
	char const *inputname = argv[1];
	for (int i = 2; i < argc; ++i)
	{
		if (strcmp(argv[i], "-o") == 0)
			asmname = argv[++i];
		else if (strcmp(argv[i], "-prep") == 0)
			prepname = argv[++i];
		else if (strcmp(argv[i], "-lex") == 0)
			lexname = argv[++i];
		else if (strcmp(argv[i], "-ast") == 0)
			astname = argv[++i];
	}

	try
	{
		// preprocessing
		Preprocessor prep;
		prep.process(inputname, prepname);
		std::cout << "Preprocessing complete." << std::endl;

		// lexing
		std::fstream fin(prepname);
		Lexer lexer;
		lexer.lex_input(fin);
		fin.close();
		std::cout << "Lexing complete." << std::endl;
		if (lexname != nullptr)
		{
			std::ofstream flex(lexname);
			lexer.print_token_list(flex);
			flex.close();
		}

		// parsing
		Parser parser(lexer.get_token_list());
		if (!parser.parse())
			throw "Could not parse program.";

		std::cout << "Parsing complete." << std::endl;
		if (astname != nullptr)
		{
			std::ofstream ast(astname);
			parser.get_translation_unit()->print(ast);
		}

		// constant folding
		parser.get_translation_unit()->constant_fold();
		std::cout << "Constant folding complete." << std::endl;
		if (astname != nullptr)
		{
			std::ofstream ast(astname);
			parser.get_translation_unit()->print(ast);
		}

		// code generation
		std::ofstream ofs(asmname);
		CodeGenerator code_generator(ofs);
		code_generator.generate_translation_unit(parser.get_translation_unit());
		std::cout << "Code generation complete." << std::endl;

		// print type tree
		std::ofstream gfs("graph.dot");
		Type::print_pool(gfs);
		gfs.close();
		
		// delete type tree
		Type::free_pool();

	}
	catch (std::exception const &e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
	}
	catch (char const *e)
	{
		std::cerr << "Exception caught: " << e << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception caught." << std::endl;
	}

	return 0;
}
