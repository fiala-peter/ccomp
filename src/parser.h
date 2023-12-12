/**
 * @file parser.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief 
 * @version 0.1
 * @date 2022-11-25
 * 
 * @copyright Copyright (c) 2022
 */
#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "ast_node.h"
#include "declaration.h"
#include "lexer.h"
#include "symbol_tree.h"
#include "statement_node.h"
#include "translation_unit.h"
#include "type.h"
#include "xpr_node.h"

#include <vector>

/**
 * @brief The ::Parser class transforms the list of tokens into a syntax tree
 */
class Parser
{
private:
	bool is_type_specifier(Token const *t) const;

	bool is_type_name(Token const *t) const;

	static XprNode::Id syntax_token(Token::Id tt);

	XprNode *parse_integer_constant();
	XprNode *parse_character_constant();
	XprNode *parse_floating_constant();
	XprNode *parse_string_literal();
	XprNode *parse_identifier(bool type_check);
	XprNode *parse_fieldname();
	XprNode *parse_primary_expression();
	XprNode *parse_postfix_expression();
	XprNode *parse_unary_expression();
	XprNode *parse_cast_expression();
	XprNode *parse_binary_xpr(size_t prec);
	XprNode *parse_conditional_expression();
	XprNode *parse_assignment_expression();
	XprNode *parse_expression();
	XprNode *parse_constant_expression();
	XprNode *parse_initializer(XprNode *lhs);

	bool parse_type_name(Type *type);
	bool parse_parameter_type_list(TypeNode::Declarations *ptl, bool *is_vararg);
	bool parse_post_declarator(Declaration *decl);
	bool parse_direct_declarator(Declaration *decl);
	bool parse_declarator(Declaration *decl);
	bool parse_struct_specifier(std::string *typedef_name);
	bool parse_enum_specifier(std::string *typedef_name);
	bool parse_declaration_specifiers(Declaration *decl);
	bool parse_struct_declaration(std::vector<Declaration> *declarations);
	bool parse_declaration();
	bool parse_declarations();

	std::shared_ptr<StmNode> parse_return_statement();
	std::shared_ptr<StmNode> parse_break_statement();
	std::shared_ptr<StmNode> parse_continue_statement();
	std::shared_ptr<StmNode> parse_if_statement();
	std::shared_ptr<StmNode> parse_while_statement();
	std::shared_ptr<StmNode> parse_do_statement();
	std::shared_ptr<StmNode> parse_for_statement();
	std::shared_ptr<StmNode> parse_empty_statement();
	std::shared_ptr<StmNode> parse_expression_statement();
	std::shared_ptr<StmNode> parse_statement();
	std::shared_ptr<CompoundNode> parse_compound_statement();

	/**
	 * @brief Parse a translation unit and return the pointer to its AST node
	 * 
	 * @return pointer to the AST node or nullptr if unsuccessful
	 */
	TransUnitNode *parse_translation_unit();

	void next_token(void);
	void previous_token(void);
	bool expect(Token::Id t);

	/**
	 * @brief Allocate a new symbol table node and set the current symbol table pointer
	 */
	void enter_scope(void);
	/**
	 * @brief visit the parent of the current symbol table node
	 */
	void exit_scope(void);

public:
	void error_message(std::string const &str);
	
	/**
	 * @brief parses the token list
	 * 
	 * @return true if the parsing if successful
	 * @return false if the parsing fails
	 */
	bool parse();

	/**
	 * @brief Return the translation unit node (root) of the AST
	 * 
	 * @return The root of the AST
	 */
	TransUnitNode *get_translation_unit()
	{
		return m_translation_unit;
	}

	/**
	 * @brief Construct a new Parser object
	 * 
	 * @param tl the token list
	 */
	Parser(TokenList const &tl)
		: m_token_list(tl)
		, m_token_eof(Token::Id::END_OF_FILE)
		, m_token_list_ptr(tl.begin())
		, m_token_list_end(tl.cend())
		, m_translation_unit(nullptr)
		, m_st_ptr(nullptr)
		, m_enum_counter(0)
	{
	}

	/**
	 * @brief Destroy the Parser object
	 * 
	 */
	~Parser()
	{
		delete m_translation_unit;
	}

	Parser(Parser const &other) = delete;

	Parser const &operator=(Parser const &other) = delete;

	std::string generate_anonymous_enum_tag() { return "anonymous#" + std::to_string(++m_enum_counter); }

	bool check_declaration(Declaration *decl);

private:
	TokenList const &m_token_list;
	Token m_token_eof;
	TokenList::const_iterator m_token_list_ptr, m_token_list_end;
	Token const *m_current_token;

	std::shared_ptr<CompoundNode> m_current_block;

	TransUnitNode *m_translation_unit;	// root of the AST
	SymbolNode *m_st_ptr;	// pointer to the current symbol table node

	size_t m_enum_counter;
};

#endif // PARSER_H_INCLUDED
