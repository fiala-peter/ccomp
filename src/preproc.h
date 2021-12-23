/**
 * @file preproc.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class Preprocessor
 * @version 0.1
 * @date 2021-02-06
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef PREPROC_H_INCLUDED
#define PREPROC_H_INCLUDED

#include "preproc_token.h"

#include <cstring>
#include <fstream>
#include <list>
#include <map>
#include <streambuf>
#include <string>

/** @brief declaration of the preprocessor class
 * 
 * @details the preprocessor preprocesses a source file and writes
 * the result into a destination file
*/
class Preprocessor
{
public:
	/**
	 * @brief Preprocesses the C file fname. The preprocessed file is written into prepname.
	 * 
	 * @param fname name of the source file
	 * @param prepname name of the result file
	 */
	void process(char const *fname, char const *prepname);

private:
	/** @brief determines if a character is a white space (excluding new lines) */
	static bool is_white_space(char c);

	/** @brief parses white space sequences into a token */
	static char const *lex_white_space_sequence(char const *s, PreprocToken *pt);

	/** @brief parses header names into a token */
	static char const *lex_header_name_impl(char const *s, PreprocToken *pt, char first, char last);

	/** @brief parses <header> names into a token */
	static char const *lex_h_header_name(char const *s, PreprocToken *pt);

	/** @brief parses "header" names into a token */
	static char const *lex_q_header_name(char const *s, PreprocToken *pt);

	/** @brief determines if character is a first identifier char or not */
	static bool is_identifier_nondigit(char c) { return isalpha(c) || c == '_'; }

	/** @brief determines if character is an identifier char or not */
	static bool is_identifier(char c) { return isalnum(c) || c == '_'; }

	/** @brief parses identifiers into a token */
	static char const *lex_identifier(char const *str, PreprocToken *pt);

	/** @brief parses preprocessor numbers into a token */
	static char const *lex_pp_number(char const *str, PreprocToken *pt);

	/** @brief parses string literals into a token */
	static char const *lex_string_literal(char const *str, PreprocToken *pt);

	/** @brief parses a single C-character */
	static char const *lex_c_char(char const *str);

	/** @brief parses a character constant into a token */
	static char const *lex_character_constant(char const *str, PreprocToken *pt);

	/** @brief parses a new-line character into a token */
	static char const *lex_new_line(char const *str, PreprocToken *pt);

	/** @brief parses a line-comment into a token */
	static char const *lex_line_comment(char const *str, PreprocToken *pt);

	/** @brief parses a block-comment into a token */
	static char const *lex_block_comment(char const *str, PreprocToken *pt);

	/** @brief parses a punctuator into a token */
	static char const *lex_punctuator(char const *str, PreprocToken *pt);

	/** @brief transform a string into a list of tokens */
	std::list<PreprocToken> tokenize(char const *fname);

	/** @brief the entry point of parsing */
	bool parse_preprocessing_file()
	{
		while (m_current_token != m_end)
			if (!parse_group_part())
				return false;
		return true;
	}

	/** @brief parse a group-part nonterminal */
	bool parse_group_part();

	/** @brief determine if an if-section follows */
	bool is_if_section() const;

	/** @parse an if-section nonterminal */
	bool parse_if_section();

	/** @brief parse an if-group nonterminal */
	bool parse_if_group(int *cond);

	/** @brief parse a control-line nonterminal */
	bool is_elif_group() const;

	/** @brief parse an elif-group nonterminal */
	bool parse_elif_group(int *cond);

	/** @brief determine if an else-group follows */
	bool is_else_group() const;

	/** @brief parse an else-group nonterminal */
	bool parse_else_group();

	bool is_endif_line() const;

	/** @brief parse an endif-line nonterminal */
	bool parse_endif_line();

	/** @brief determine if a control line follows */
	bool is_control_line() const;

	/** @brief parse a control-line nonterminal */
	bool parse_control_line();

	/** @brief parse a text-line nonterminal */
	bool parse_text_line();

	bool is_non_directive() const;

	/** @brief parse a non-directive nonterminal */
	bool parse_non_directive();

	/** @brief parse a new-line nonterminal */
	bool parse_new_line();



	bool parse_constant_expression(int *result)
	{
		return parse_conditional_expression(result);
	}

	bool is_defined(std::string const &identifier) const
	{
		return m_macros.find(identifier) != m_macros.end();
	}

	using parsing_function = bool (Preprocessor::*)(int *);

	template <class Op>
	bool parse_xpr(int *result, parsing_function parse, std::string const &str, Op op)
	{
		int val;
		(this->*parse)(&val);
		while (m_current_token->get_string() == str)
		{
			next_token();
			int rhs;
			(this->*parse)(&rhs);
			val = op(val, rhs);
		}
		*result = val;
		return true;
	}

	bool parse_unary_expression(int *result);

	bool parse_multiplicative_expression(int *result);

	bool parse_additive_expression(int *result);

	bool parse_shift_expression(int *result);

	bool parse_relational_expression(int *result);

	bool parse_equality_expression(int *result);

	bool parse_and_expression(int *result);

	bool parse_exclusive_or_expression(int *result);

	bool parse_inclusive_or_expression(int *result);

	bool parse_logical_and_expression(int *result);

	bool parse_logical_or_expression(int *result);

	bool parse_conditional_expression(int *result);

	void next_token(bool skip_spaces = true)
	{
		m_current_token++;
		if (skip_spaces)
			skip_white_spaces();
	}

	void skip_white_spaces()
	{
		while (m_current_token->is_white_space())
			next_token();
	}

	void add_macro(std::string const &identifier)
	{
		m_macros[identifier] = std::list<PreprocToken>();
	}

	void remove_macro(std::string const &identifier)
	{
		auto it = m_macros.find(identifier);
		if (it != m_macros.end())
			m_macros.erase(it);
	}

	void add_macro_replacement(std::string const &identifier, PreprocToken const &tok)
	{
		m_macros[identifier].push_back(tok);
	}

	/** @brief substitute a macro and return a list of preproc tokents */
	std::list<PreprocToken> macro_substitute(std::string const &id) const;

private:
	std::list<PreprocToken> m_pp_token_list;
	std::list<PreprocToken>::iterator m_current_token, m_end;
	std::map<std::string, std::list<PreprocToken>> m_macros;
	bool m_write;
	std::ofstream m_ofs;
};

#endif
