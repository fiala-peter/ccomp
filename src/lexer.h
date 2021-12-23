/**
 * @file lexer.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief Declaration of class ::Lexer and ::TokenList
 * @version 0.1
 * @date 2021-01-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef LEXER_H_DEFINED
#define LEXER_H_DEFINED

#include "token.h"

#include <list>

/** @brief The lexer returns the tokens as a list of ::Token objects */
using TokenList = std::list<Token>;

class Lexer
{

public:
	/**
	 * @brief Read a token list from an input stream and store it internally
	 * 
	 * @param is the input stream
	 */
	void read_token_list(std::istream &is);

	/**
	 * @brief Display the token list in an output stream
	 * 
	 * @param os the output stream
	 */
	void print_token_list(std::ostream &os) const;

	/**
	 * @brief Return the list of tokens
	 * 
	 * @return The list of tokens
	 */
	TokenList const &get_token_list() const;

private:
	TokenList m_token_list;
};

#endif // LEXER_H_DEFINED
