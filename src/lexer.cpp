#include "lexer.h"

#include <cstring>
#include <string>

char const *Lexer::lex_floating_constant(char const *str, Token *token)
{
	// floating constant
	char const *end = str;
	bool wasdot = false;
	while (isdigit(*end))
		end++;
	if (*end == '.')
	{
		wasdot = true;
		end++;
	}
	while (isdigit(*end))
		end++;
	if (wasdot)
	{
		std::string flc(str, end);
		size_t ndigits;
		double b = std::stod(flc, &ndigits);
		if (ndigits == flc.size())
		{
			*token = Token(Token::Id::FLOATING_CONSTANT);
			token->set_string(std::string(str, end));
			token->set_double_constant(b);
			return end;
		}
	}
	return str;
}

char const *Lexer::lex_decimal_integer_constant(char const *str, Token *token)
{
	// integer constant
	long long c = 0;
	char const *end = str;
	while (isdigit(*end))
		c = 10 * c + *end++ - '0';
	*token = Token(Token::Id::INTEGER_CONSTANT);
	token->set_int_constant(c);
	token->set_string(std::string(str, end));
	return end;
}

char const *Lexer::lex_octal_integer_constant(char const *str, Token *token)
{
	long long c = 0;
	char const *end = str;
	while (*end >= '0' && *end <= '7')
	{
		c = 8 * c + (*end - '0');
		end++;
	}
	*token = Token(Token::Id::INTEGER_CONSTANT);
	token->set_int_constant(c);
	token->set_string(std::string(str, end));
	return end;
}

char const *Lexer::lex_hexadecimal_integer_constant(char const *str, Token *token)
{
	long long c = 0;
	char const *end = str + 2; // skip 0x prefix
	while (isxdigit(*end))
	{
		c *= 16;
		if (*end >= '0' && *end <= '9')
			c += (*end - '0');
		else if (*end >= 'A' && *end <= 'F')
			c += (*end - 'A' + 10);
		else if (*end >= 'a' && *end <= 'f')
			c += *end - 'a' + 10;
		end++;
	}
	*token = Token(Token::Id::INTEGER_CONSTANT);
	token->set_int_constant(c);
	token->set_string(std::string(str, end));
	return end;
}

char const *Lexer::lex_integer_constant(char const *str, Token *token)
{
	// integer constant
	if (!isdigit(*str))
		return str;
	if (*str != '0')
		return lex_decimal_integer_constant(str, token);
	if (*str == '0' && *(str + 1) != 'x' && *(str + 1) != 'X')
		return lex_octal_integer_constant(str, token);
	if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) != 'X'))
		return lex_hexadecimal_integer_constant(str, token);
	return str;
}

char const *Lexer::lex_string_literal(char const *str, Token *token)
{
	char const *end = str;
	// string literal
	if (*end == '\"')
	{
		end++;
		while (*end != '\0' && *end != '\"')
		{
			end++;
			if (*end == '\\' && *(end + 1) == '\"')
				end += 2;
		}
		if (*end == '\0')
			return str;
		end++;
		std::string data(str + 1, end - 1);
		*token = Token(Token::Id::STRING_LITERAL);
		token->set_string(data);
		return end;
	}
	return str;
}

char const *Lexer::lex_character_constant(char const *str, Token *token)
{
	char const *end = str;
	// string literal
	if (*end == '\'')
	{
		end++;
		while (*end != '\0' && *end != '\'')
		{
			end++;
			if (*end == '\\' && *(end + 1) == '\'')
				end += 2;
		}
		if (*end == '\0')
			return str;
		end++;
		std::string data(str, end);
		*token = Token(Token::Id::CHARACTER_CONSTANT);
		token->set_string(data);
		return end;
	}
	return str;
}

char const *Lexer::read_next_token(char const *str, Token *token)
{
	while (*str != '\0' && isspace(*str))
		str++;
	if (*str == '\0')
		return nullptr;

	// try to read keywords
	struct
	{
		Token::Id t;
		char const *keywrd;
	} keywords[] = {
		{Token::Id::AUTO, "auto"},
		{Token::Id::BREAK, "break"},
		{Token::Id::CASE, "case"},
		{Token::Id::CHAR, "char"},
		{Token::Id::CONST, "const"},
		{Token::Id::CONTINUE, "continue"},
		{Token::Id::DEFAULT, "default"},
		{Token::Id::DO, "do"},
		{Token::Id::DOUBLE, "double"},
		{Token::Id::ELSE, "else"},
		{Token::Id::ENUM, "enum"},
		{Token::Id::EXTERN, "extern"},
		{Token::Id::FLOAT, "float"},
		{Token::Id::FOR, "for"},
		{Token::Id::GOTO, "goto"},
		{Token::Id::IF, "if"},
		{Token::Id::INT, "int"},
		{Token::Id::LONG, "long"},
		{Token::Id::RESTRICT, "restrict"},
		{Token::Id::RETURN, "return"},
		{Token::Id::SHORT, "short"},
		{Token::Id::SIGNED, "signed"},
		{Token::Id::SIZEOF, "sizeof"},
		{Token::Id::STATIC, "static"},
		{Token::Id::STRUCT, "struct"},
		{Token::Id::SWITCH, "switch"},
		{Token::Id::TYPEDEF, "typedef"},
		{Token::Id::UNION, "union"},
		{Token::Id::UNSIGNED, "unsigned"},
		{Token::Id::VOID, "void"},
		{Token::Id::WHILE, "while"},
		{Token::Id::NO_TOKEN, nullptr}};
	for (int i = 0; keywords[i].keywrd != nullptr; i++)
	{
		auto n = strlen(keywords[i].keywrd);
		if (strncmp(str, keywords[i].keywrd, n) == 0 && (str + n) != nullptr && !isalnum(str[n]) && str[n] != '_')
		{
			*token = Token(keywords[i].t);
			token->set_string(std::string(str, str + n));
			return str + n;
		}
	}

	// try to read operators
	struct
	{
		Token::Id t;
		char const *op;
	} ops[] = {
		{Token::Id::ELLIPSIS, "..."},
		{Token::Id::SHR_ASSIGN, ">>="},
		{Token::Id::SHL_ASSIGN, "<<="},
		{Token::Id::BITWISE_XOR_ASSIGN, "^="},
		{Token::Id::BITWISE_OR_ASSIGN, "|="},
		{Token::Id::BITWISE_AND_ASSIGN, "&="},
		{Token::Id::PLUS_ASSIGN, "+="},
		{Token::Id::MINUS_ASSIGN, "-="},
		{Token::Id::TIMES_ASSIGN, "*="},
		{Token::Id::DIV_ASSIGN, "/="},
		{Token::Id::MOD_ASSIGN, "%="},
		{Token::Id::LOGICAL_OR, "||"},
		{Token::Id::LOGICAL_AND, "&&"},
		{Token::Id::EQ, "=="},
		{Token::Id::NEQ, "!="},
		{Token::Id::SHL, "<<"},
		{Token::Id::SHR, ">>"},
		{Token::Id::LESS_EQUAL, "<="},
		{Token::Id::GREATER_EQUAL, ">="},
		{Token::Id::INCREMENT, "++"},
		{Token::Id::DECREMENT, "--"},
		{Token::Id::ARROW, "->"},
		{Token::Id::SEMICOLON, ";"},
		{Token::Id::PARENTHESES_OPEN, "("},
		{Token::Id::PARENTHESES_CLOSE, ")"},
		{Token::Id::BRACE_OPEN, "{"},
		{Token::Id::BRACE_CLOSE, "}"},
		{Token::Id::PLUS, "+"},
		{Token::Id::MINUS, "-"},
		{Token::Id::ASTERISK, "*"},
		{Token::Id::ASSIGN, "="},
		{Token::Id::MOD, "%"},
		{Token::Id::PER, "/"},
		{Token::Id::LESS, "<"},
		{Token::Id::GREATER, ">"},
		{Token::Id::AMPERSAND, "&"},
		{Token::Id::BITWISE_OR, "|"},
		{Token::Id::BITWISE_XOR, "^"},
		{Token::Id::BITWISE_NOT, "~"},
		{Token::Id::COMMA, ","},
		{Token::Id::BRACKET_OPEN, "["},
		{Token::Id::BRACKET_CLOSE, "]"},
		{Token::Id::POINT, "."},
		{Token::Id::CONDITIONAL, "?"},
		{Token::Id::COLON, ":"},
		{Token::Id::LOGICAL_NOT, "!"},
		{Token::Id::NO_TOKEN, nullptr}};
	for (int i = 0; ops[i].op != nullptr; i++)
	{
		auto n = strlen(ops[i].op);
		if (strncmp(str, ops[i].op, n) == 0)
		{
			*token = Token(ops[i].t);
			token->set_string(std::string(str, str + n));
			return str + n;
		}
	}

	// try to read an identifier
	char const *end = str;
	if (isalpha(*end) || *end == '_')
	{
		end++;
		while (*end != '\0' && (isalnum(*end) || *end == '_'))
			end++;
		*token = Token(Token::Id::IDENTIFIER);
		token->set_string(std::string(str, end));
		return end;
	}

	// try to read floating constant
	end = lex_floating_constant(str, token);
	if (end != str)
		return end;

	// try to read integer constant
	end = lex_integer_constant(str, token);
	if (end != str)
		return end;

	// try to read string literal
	end = lex_string_literal(str, token);
	if (end != str)
		return end;

	// try to read character constant
	end = lex_character_constant(str, token);
	if (end != str)
		return end;

	return str;
}

void Lexer::lex_input(std::istream &is)
{
	enum
	{
		MAX_LINE = 1024
	};
	char linebuf[MAX_LINE];
	size_t line_cntr = 0;

	while (is.getline(linebuf, MAX_LINE))
	{
		char const *ptr = linebuf;
		char const *end;
		line_cntr++;
		Token token;
		while ((end = read_next_token(ptr, &token)) != nullptr)
		{
			if (end == ptr)
			{
				std::cerr << "Lexing error at line " << line_cntr << ". Could not interpret " << ptr << std::endl;
				return;
			}
			token.set_coordinate(Coordinate(line_cntr, ptr - linebuf + 1));
			m_token_list.push_back(token);
			ptr = end;
		}
	}
}

void Lexer::print_token_list(std::ostream &os) const
{
	for (auto const &tok : m_token_list)
		os << tok << std::endl;
}

TokenList const &Lexer::get_token_list() const
{
	return this->m_token_list;
}
