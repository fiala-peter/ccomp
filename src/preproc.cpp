#include "preproc.h"

void Preprocessor::process(char const *fname, char const *prepname)
{
	m_write = true;
	m_pp_token_list = tokenize(fname);
	m_current_token = m_pp_token_list.begin();
	m_end = m_pp_token_list.end();
	m_ofs = std::ofstream(prepname);

	parse_preprocessing_file();
}

bool Preprocessor::is_white_space(char s)
{
	char white_spaces[] = {' ', '\t', '\0'}; // the last item terminates the array
	for (size_t i = 0; white_spaces[i] != '\0'; ++i)
		if (s == white_spaces[i])
			return true;
	return false;
}

char const *Preprocessor::lex_white_space_sequence(char const *s, PreprocToken *pt)
{
	if (!is_white_space(*s))
		return s;
	char const *end = s;
	while (is_white_space(*end))
		end++;
	pt->set_category(PreprocToken::WHITE_SPACE_SEQUENCE);
	pt->set_string(std::string(s, end));
	return end;
}

char const *Preprocessor::lex_header_name_impl(char const *s, PreprocToken *pt, char first, char last)
{
	char const *end = s;
	if (*end != first)
		return s;
	end++;
	while (*end != '\n' && *end != last)
		end++;
	if (*end == '\n')
		return s;
	end++;
	std::string str(s, end);
	pt->set_category(PreprocToken::HEADER_NAME);
	pt->set_string(str);
	return end;
}

char const *Preprocessor::lex_h_header_name(char const *s, PreprocToken *pt)
{
	return lex_header_name_impl(s, pt, '<', '>');
}

char const *Preprocessor::lex_q_header_name(char const *s, PreprocToken *pt)
{
	return lex_header_name_impl(s, pt, '"', '"');
}

char const *Preprocessor::lex_identifier(char const *str, PreprocToken *pt)
{
	char const *end = str;
	if (!is_identifier_nondigit(*end))
		return str;
	end++;
	while (is_identifier(*end))
		end++;
	pt->set_category(PreprocToken::IDENTIFIER);
	pt->set_string(std::string(str, end));
	return end;
}

char const *Preprocessor::lex_pp_number(char const *str, PreprocToken *pt)
{
	char const *end = str;
	if (!isdigit(*end) && !(*end == '.' && isdigit(*(end + 1))))
		return str;
	if (*end == '.')
		end++;
	while (true)
	{
		if (isdigit(*end))
			end++;
		else if (*end == '.')
			end++;
		else if ((*end == 'e' || *end == 'E' || *end == 'p' || *end == 'P') && (*(end + 1) == '+' || *(end + 1) == '-'))
			end += 2;
		else if (isalpha(*end) || *end == '_')
			end++;
		else
			break;
	}
	pt->set_category(PreprocToken::PP_NUMBER);
	pt->set_string(std::string(str, end));
	return end;
}

char const *Preprocessor::lex_string_literal(char const *str, PreprocToken *pt)
{
	char const *end = str;
	// string literal
	if (*end == '\"')
	{
		end++;
		while (*end != '\"')
			end++;
		end++;
		std::string data(str + 1, end - 1);
		pt->set_category(PreprocToken::STRING_LITERAL);
		pt->set_string(data);
		return end;
	}
	return str;
}

char const *Preprocessor::lex_c_char(char const *str)
{
	if (str[0] == '\\')
	{
		// try lexing simple escape sequence
		char simple[] = {'\\', '\"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v', '0'};
		for (size_t i = 0; i < sizeof simple; ++i)
			if (str[1] == simple[i])
				return str + 2;
		// try lexing octal escape_sequence (at most 3 octal digits)
		if (str[1] >= '0' && str[1] <= '7')
			for (size_t i = 1; i < 3; ++i)
				if (str[i + 1] >= '0' && str[i + 1] <= '7')
					return str + 1 + i;
		// try lexing hexadecimal escape sequence
		if (str[1] == 'x')
		{
			size_t i = 0;
			while (isxdigit(str[2 + i]))
				i++;
			if (i > 0)
				return str + 2 + i;
		}
		// invalid escape sequence
		return str;
	}
	// plain character
	if (str[0] != '\n' && str[0] != '\\' && str[0] != '\'')
		return str + 1;
	// could not lex
	return str;
}

char const *Preprocessor::lex_character_constant(char const *str, PreprocToken *pt)
{
	char const *end = str;
	if (*end == 'u' || *end == 'U' || *end == 'L')
		end++;
	if (*end == '\'')
	{
		end++;
		while (*end != '\'')
			end = lex_c_char(end);
		end++; // skip closing '
		std::string data(str, end);
		pt->set_category(PreprocToken::CHARACTER_CONSTANT);
		pt->set_string(data);
		return end;
	}
	return str;
}

char const *Preprocessor::lex_new_line(char const *str, PreprocToken *pt)
{
	if (*str == '\n')
	{
		pt->set_category(PreprocToken::NEW_LINE);
		pt->set_string("\n");
		++str;
	}
	return str;
}

char const *Preprocessor::lex_line_comment(char const *str, PreprocToken *pt)
{
	if (str[0] == '/' && str[1] == '/')
	{
		char const *end = str;
		while (*end != '\n')
			end++;
		pt->set_category(PreprocToken::WHITE_SPACE_SEQUENCE);
		pt->set_string(" ");
		return end;
	}
	return str;
}

char const *Preprocessor::lex_block_comment(char const *str, PreprocToken *pt)
{
	if (str[0] == '/' && str[1] == '*')
	{
		char const *end = str + 2;
		while (end[0] != '*' || end[1] != '/')
			end++;
		end += 2;
		pt->set_category(PreprocToken::WHITE_SPACE_SEQUENCE);
		pt->set_string(" ");
		return end;
	}
	return str;
}

char const *Preprocessor::lex_punctuator(char const *str, PreprocToken *pt)
{
	char const *puncts[] = {"...", ">>=", "<<=", "^=", "|=", "&=", "+=", "-=",
							"*=", "/=", "%=", "||", "&&", "==", "!=", "<<",
							">>", "<=", ">=", "++", "--", "->", ";", "(", ")",
							"{", "}", "+", "-", "*", "=", "%", "/", "<", ">",
							"&", "|", "^", "~", ",", "[", "]", ".", "?", ":", "!",
							"##", "#",
							nullptr};
	for (int i = 0; puncts[i] != nullptr; i++)
	{
		size_t n = strlen(puncts[i]);
		if (strncmp(str, puncts[i], n) == 0)
		{
			pt->set_category(PreprocToken::PUNCTUATOR);
			pt->set_string(std::string(str, str + n));
			return str + n;
		}
	}
	return str;
}

std::list<PreprocToken> Preprocessor::tokenize(char const *fname)
{
	// read the whole file into a string
	std::ifstream ifs(fname);
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	// remove '\'+'\n' sequences
	size_t where = 0;
	while ((where = str.find("\\\n", where)) != std::string::npos)
		str.erase(where, 2);

	// split into list of pp-tokens
	std::list<PreprocToken> pp_token_list;
	using lexer_fptr = char const *(*)(char const *, PreprocToken *);
	lexer_fptr lexer_funs[] = {
		lex_white_space_sequence,
		lex_h_header_name,
		lex_q_header_name,
		lex_identifier,
		lex_pp_number,
		lex_string_literal,
		lex_character_constant,
		lex_new_line,
		lex_line_comment,
		lex_block_comment,
		lex_punctuator,
		nullptr};

	char const *s = str.c_str();
	while (*s != '\0')
	{
		bool lexed = false;
		for (size_t i = 0; lexer_funs[i] != nullptr; ++i)
		{
			PreprocToken pt;
			char const *end;
			if ((end = lexer_funs[i](s, &pt)) != s)
			{
				s = end;
				pp_token_list.push_back(pt);
				lexed = true;
				break;
			}
		}
		if (!lexed)
			throw __FILE__ ": unprocessed preprocessor token.";
	}

	// insert newline character to end if needed
	if (pp_token_list.empty() || pp_token_list.back().get_category() != PreprocToken::NEW_LINE)
	{
		PreprocToken pt;
		pt.set_category(PreprocToken::NEW_LINE);
		pp_token_list.push_back(pt);
	}

	return pp_token_list;
}

bool Preprocessor::parse_group_part()
{
	if (is_if_section())
		return parse_if_section();
	if (is_control_line())
		return parse_control_line();
	if (is_non_directive())
		return parse_non_directive();
	return parse_text_line();
}

bool Preprocessor::is_if_section() const
{
	auto iter = m_current_token;
	while (iter->is_white_space())
		iter++;
	if (iter->get_string() != "#")
		return false;
	iter++;
	while (iter->is_white_space())
		iter++;
	if (iter->get_string() != "if" && iter->get_string() != "ifdef" && iter->get_string() != "ifndef")
		return false;
	return true;
}

bool Preprocessor::parse_if_section()
{
	bool write_restore = m_write;

	int if_cond;
	parse_if_group(&if_cond);
	bool fulfilled = if_cond;

	m_write = write_restore && !fulfilled;

	while (is_elif_group())
	{
		parse_elif_group(&if_cond);
		fulfilled = fulfilled || if_cond;
		m_write = write_restore && !fulfilled;
	}

	if (is_else_group())
		parse_else_group();
	parse_endif_line();

	m_write = write_restore;

	return true;
}

bool Preprocessor::parse_if_group(int *cond)
{
	skip_white_spaces();
	next_token(); // #
	if (m_current_token->get_string() == "if")
	{
		next_token();
		int result;
		parse_constant_expression(&result);
		m_write = m_write && result;
		parse_new_line();
		m_write = m_write && result;
		while (!is_elif_group() && !is_else_group() && !is_endif_line())
			parse_group_part();
		*cond = result;
	}
	else if (m_current_token->get_string() == "ifdef")
	{
		next_token();
		std::string identifier = m_current_token->get_string();
		next_token();
		parse_new_line();
		int result = is_defined(identifier);
		m_write = m_write && result;
		while (!is_elif_group() && !is_else_group() && !is_endif_line())
			parse_group_part();
		*cond = result;
	}
	else if (m_current_token->get_string() == "ifndef")
	{
		next_token();
		std::string identifier = m_current_token->get_string();
		next_token();
		parse_new_line();
		int result = !is_defined(identifier);
		m_write = m_write && result;
		while (!is_elif_group() && !is_else_group() && !is_endif_line())
			parse_group_part();
		*cond = result;
	}
	else
		throw __FILE__ ": unprocessed if-group element";
	return true;
}

bool Preprocessor::is_elif_group() const
{
	auto iter = m_current_token;
	while (iter->is_white_space())
		iter++;
	if (iter->get_string() != "#")
		return false;
	iter++;
	while (iter->is_white_space())
		iter++;
	return (iter->get_string() == "elif");
}

bool Preprocessor::parse_elif_group(int *cond)
{
	skip_white_spaces();
	next_token(); // #
	next_token(); // elif
	int result;
	parse_constant_expression(&result);
	m_write = m_write && result;
	parse_new_line();
	while (!is_elif_group() && !is_else_group() && !is_endif_line())
		if (!parse_group_part())
			return false;
	*cond = result;
	return true;
}

bool Preprocessor::is_else_group() const
{
	auto iter = m_current_token;
	while (iter->is_white_space())
		iter++;
	if (iter->get_string() != "#")
		return false;
	iter++;
	while (iter->is_white_space())
		iter++;
	return (iter->get_string() == "else");
}

bool Preprocessor::parse_else_group()
{
	skip_white_spaces();
	next_token(); // #
	next_token(); // else
	parse_new_line();
	while (!is_endif_line())
		parse_group_part();
	return true;
}

bool Preprocessor::is_endif_line() const
{
	auto iter = m_current_token;
	while (iter->is_white_space())
		iter++;
	if (iter->get_string() != "#")
		return false;
	iter++;
	while (iter->is_white_space())
		iter++;
	return (iter->get_string() == "endif");
}

bool Preprocessor::parse_endif_line()
{
	skip_white_spaces();
	next_token(); // #
	next_token(); // endif
	return parse_new_line();
}

bool Preprocessor::is_control_line() const
{
	auto iter = m_current_token;
	while (iter->is_white_space())
		iter++;
	if (iter->get_string() != "#")
		return false;
	iter++;
	while (iter->is_white_space())
		iter++;
	if (iter->get_string() != "define" && iter->get_string() != "undef" && iter->get_string() != "include")
		return false;
	return true;
}

bool Preprocessor::parse_control_line()
{
	skip_white_spaces();
	next_token(); // #
	if (m_current_token->get_string() == "define")
	{
		next_token();
		std::string const &identifier = m_current_token->get_string();
		if (m_write)
			add_macro(identifier);
		next_token();
		while (m_current_token->get_category() != PreprocToken::NEW_LINE)
		{
			if (m_write)
				add_macro_replacement(identifier, *m_current_token);
			next_token();
		}
		next_token(); // skip newline
	}
	else if (m_current_token->get_string() == "undef")
	{
		next_token();
		std::string const &identifier = m_current_token->get_string();
		next_token();
		if (m_write)
			remove_macro(identifier);
		parse_new_line();
	}
	else if (m_current_token->get_string() == "include")
	{
		next_token();
		std::string headername = m_current_token->get_string();
		next_token();
		skip_white_spaces();
		// now we are at the new line after the include
		// paste the content after the new line
		if (m_write)
		{
			std::string fname(headername.begin() + 1, headername.end() - 1);
			std::list<PreprocToken> include_content = this->tokenize(fname.c_str());
			auto pos = m_current_token;
			pos++;
			m_pp_token_list.insert(pos, include_content.begin(), include_content.end());
		}

		parse_new_line();
	}
	else
		throw __FILE__ ": Undefined control line entry";
	return true;
}

bool Preprocessor::parse_text_line()
{
	while (m_current_token->get_category() != PreprocToken::NEW_LINE)
	{
		if (m_write)
		{
			// lookup identifiers in marco table
			if (m_current_token->get_category() == PreprocToken::IDENTIFIER)
			{
				std::string const &id = m_current_token->get_string();
				if (is_defined(id))
				{
					std::list<PreprocToken> replace = macro_substitute(id);
					for (auto j : replace)
						m_ofs << j.get_string();
				}
				else
					m_ofs << id;
			}
			else
				m_ofs << m_current_token->get_string();
		}
		next_token(false);
	}
	next_token(false); // newline token
	m_ofs << std::endl;

	return true;
}

bool Preprocessor::is_non_directive() const
{
	auto iter = m_current_token;
	while (iter->is_white_space())
		iter++;
	return iter->get_string() == "#";
}

bool Preprocessor::parse_non_directive()
{
	skip_white_spaces();
	next_token(); // #
	while (m_current_token->get_category() != PreprocToken::NEW_LINE)
		next_token();
	next_token(); // new line
	return true;
}

bool Preprocessor::parse_new_line()
{
	skip_white_spaces();
	if (m_current_token->get_category() == PreprocToken::NEW_LINE)
	{
		next_token();
		return true;
	}
	return false;
}

std::list<PreprocToken> Preprocessor::macro_substitute(std::string const &id) const
{
	auto it = m_macros.find(id);
	if (it == m_macros.end())
		throw __FILE__ ": Macro does not exist";

	// get replacement list
	std::list<PreprocToken> replace = it->second;
	// traverse replacement list for recursive substitutions
	for (auto j = replace.begin(); j != replace.end();)
	{
		if (j->get_category() == PreprocToken::IDENTIFIER)
		{
			std::string const &subid = j->get_string();
			if (subid != id && is_defined(subid))
			{
				std::list<PreprocToken> sublist = macro_substitute(subid);
				j = replace.erase(j);
				replace.insert(j, sublist.begin(), sublist.end());
			}
			else
				++j;
		}
		else
			++j;
	}
	return replace;
}

bool Preprocessor::parse_unary_expression(int *result)
{
	if (m_current_token->get_string() == "(")
	{
		next_token();
		int val;
		parse_constant_expression(&val);
		if (m_current_token->get_string() != ")")
			return false;
		next_token();
		*result = val;
		return true;
	}
	if (m_current_token->get_string() == "+")
	{
		int val;
		next_token();
		parse_unary_expression(&val);
		*result = val;
		return true;
	}
	if (m_current_token->get_string() == "-")
	{
		int val;
		next_token();
		parse_unary_expression(&val);
		*result = -val;
		return true;
	}
	if (m_current_token->get_string() == "!")
	{
		int val;
		next_token();
		parse_unary_expression(&val);
		*result = !val;
		return true;
	}
	if (m_current_token->get_string() == "defined")
	{
		next_token();
		bool is_bracket = false;
		if (m_current_token->get_string() == "(")
		{
			is_bracket = true;
			next_token();
		}
		std::string const &identifier = m_current_token->get_string();
		*result = is_defined(identifier);
		next_token();
		if (is_bracket)
		{
			if (m_current_token->get_string() != ")")
				return false;
			next_token();
		}
		return true;
	}
	if (m_current_token->get_category() == PreprocToken::IDENTIFIER)
	{
		std::string id = m_current_token->get_string();
		if (!is_defined(id))
			return false;
		auto list = this->macro_substitute(id);
		if (list.size() != 1)
			return false;
		long val = std::atol(list.begin()->get_string().c_str());
		*result = val;
		next_token();
		return true;
	}
	if (m_current_token->get_category() == PreprocToken::PP_NUMBER)
	{
		long val = std::atol(m_current_token->get_string().c_str());
		*result = val;
		next_token();
		return true;
	}
	throw __FILE__ ": Unprocessed unary expression";
}

bool Preprocessor::parse_multiplicative_expression(int *result)
{
	int val;
	parse_unary_expression(&val);
	std::string op = m_current_token->get_string();
	while (op == "*" || op == "/" || op == "%")
	{
		next_token();
		int rhs;
		parse_unary_expression(&rhs);
		if (op == "*")
			val = val * rhs;
		else if (op == "/")
			val = val / rhs;
		else
			val = val % rhs;
		op = m_current_token->get_string();
	}
	*result = val;
	return true;
}

bool Preprocessor::parse_additive_expression(int *result)
{
	int val;
	parse_multiplicative_expression(&val);
	std::string op = m_current_token->get_string();
	while (op == "+" || op == "-")
	{
		next_token();
		int rhs;
		parse_multiplicative_expression(&rhs);
		if (op == "+")
			val = val + rhs;
		else
			val = val - rhs;
		op = m_current_token->get_string();
	}
	*result = val;
	return true;
}

bool Preprocessor::parse_shift_expression(int *result)
{
	int val;
	parse_additive_expression(&val);
	std::string op = m_current_token->get_string();
	while (op == "<<" || op == ">>")
	{
		next_token();
		int rhs;
		parse_additive_expression(&rhs);
		if (op == "<<")
			val = val << rhs;
		else
			val = val >> rhs;
		op = m_current_token->get_string();
	}
	*result = val;
	return true;
}

bool Preprocessor::parse_relational_expression(int *result)
{
	int val;
	parse_shift_expression(&val);
	std::string op = m_current_token->get_string();
	while (op == "<" || op == ">" || op == "<=" || op == ">=")
	{
		next_token();
		int rhs;
		parse_shift_expression(&rhs);
		if (op == "<")
			val = val < rhs;
		else if (op == ">")
			val = val > rhs;
		else if (op == "<=")
			val = val <= rhs;
		else
			val = val >= rhs;
		op = m_current_token->get_string();
	}
	*result = val;
	return true;
}

bool Preprocessor::parse_equality_expression(int *result)
{
	int val;
	parse_relational_expression(&val);
	std::string op = m_current_token->get_string();
	while (op == "==" || op == "!=")
	{
		next_token();
		int rhs;
		parse_relational_expression(&rhs);
		if (op == "==")
			val = val == rhs;
		else
			val = val != rhs;
		op = m_current_token->get_string();
	}
	*result = val;
	return true;
}

bool Preprocessor::parse_and_expression(int *result)
{
	return parse_xpr(result, &Preprocessor::parse_equality_expression, "&",
					 [](int a, int b) { return a & b; });
}

bool Preprocessor::parse_exclusive_or_expression(int *result)
{
	return parse_xpr(result, &Preprocessor::parse_and_expression, "^",
					 [](int a, int b) { return a ^ b; });
}

bool Preprocessor::parse_inclusive_or_expression(int *result)
{
	return parse_xpr(result, &Preprocessor::parse_exclusive_or_expression, "|",
					 [](int a, int b) { return a | b; });
}

bool Preprocessor::parse_logical_and_expression(int *result)
{
	return parse_xpr(result, &Preprocessor::parse_inclusive_or_expression, "&&",
					 [](int a, int b) { return a && b; });
}

bool Preprocessor::parse_logical_or_expression(int *result)
{
	return parse_xpr(result, &Preprocessor::parse_logical_and_expression, "||",
					 [](int a, int b) { return a || b; });
}

bool Preprocessor::parse_conditional_expression(int *result)
{
	int cond;
	parse_logical_or_expression(&cond);
	if (m_current_token->get_string() == "?")
	{
		next_token();
		int tr, fls;
		parse_constant_expression(&tr);
		if (m_current_token->get_string() != ":")
			return false;
		next_token();
		parse_constant_expression(&fls);
		*result = cond ? tr : fls;
	}
	*result = cond;
	return true;
}
