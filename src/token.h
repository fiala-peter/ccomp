/**
 * @file token.h
 * @author P. Fiala (fiala@hit.bme.hu)
 * @brief Declaration of class ::Token
 * @version 0.1
 * @date 2021-12-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include "coordinate.h"

#include <iostream>
#include <string>

/**
 * @brief Representation of C language tokens
 */
class Token
{
public:
	enum class Id
	{
		NO_TOKEN,
		AMPERSAND,
		BRACKET_CLOSE,
		BRACKET_OPEN,
		ARROW,
		ASSIGN,
		ASTERISK,
		AUTO,
		BITWISE_AND_ASSIGN,
		BITWISE_OR,
		BITWISE_OR_ASSIGN,
		BITWISE_NOT,
		BITWISE_XOR,
		BITWISE_XOR_ASSIGN,
		BRACE_CLOSE,
		BRACE_OPEN,
		BREAK,
		CASE,
		CHAR,
		CHARACTER_CONSTANT,
		COLON,
		COMMA,
		CONDITIONAL,
		CONTINUE,
		CONST,
		DECREMENT,
		DEFAULT,
		DIV_ASSIGN,
		DO,
		DOUBLE,
		ENUM,
		ELLIPSIS,
		ELSE,
		EQ,
		EXTERN,
		FLOAT,
		FLOATING_CONSTANT,
		FOR,
		GOTO,
		GREATER,
		GREATER_EQUAL,
		IDENTIFIER,
		IF,
		INCREMENT,
		INT,
		INTEGER_CONSTANT,
		LESS,
		LESS_EQUAL,
		LOGICAL_AND,
		LOGICAL_OR,
		LOGICAL_NOT,
		LONG,
		MOD,
		MOD_ASSIGN,
		MINUS,
		MINUS_ASSIGN,
		NEQ,
		PARENTHESES_CLOSE,
		PARENTHESES_OPEN,
		PER,
		PLUS,
		PLUS_ASSIGN,
		POINT,
		REGISTER,
		RESTRICT,
		RETURN,
		SIGNED,
		SHL,
		SHL_ASSIGN,
		SHORT,
		SHR,
		SHR_ASSIGN,
		SIZEOF,
		SEMICOLON,
		STATIC,
		STRING_LITERAL,
		STRUCT,
		SWITCH,
		TIMES_ASSIGN,
		UNION,
		UNSIGNED,
		VOID,
		VOLATILE,
		WHILE,
		TYPEDEF,
		END_OF_FILE
	};

	/**
	 * @brief Construct a new Token object
	 * 
	 * @param id the token's identifier
	 * @param coord the token's coordinate in the input file
	 */
	Token(Id id = Id::NO_TOKEN, Coordinate coord = Coordinate())
		: m_id(id), m_coord(coord), m_int_constant(0)
	{
	}

	/**
	 * @brief Return the token id
	 */
	Id get_id() const { return m_id; }

	/**
	 * @brief Set the token's coordinate
	 * 
	 * @param coord the token's coordinate in the input file
	 */
	void set_coordinate(Coordinate const &coord) { m_coord = coord; }

	/**
	 * @brief Return the token's coordinate
	 */
	Coordinate const &get_coordinate() const { return m_coord; }

	/**
	 * @brief Set the token's double constant member
	 * 
	 * @param d 
	 */
	void set_double_constant(double d) { m_double_constant = d; }

	/**
	 * @brief Return the token's double constant member
	 */
	double get_double_constant() const { return m_double_constant; };

	/**
	 * @brief Set the token's int constant
	 */
	void set_int_constant(int c) { m_int_constant = c; }

	/**
	 * @brief Return the token's int constant
	 */
	int get_int_constant() const { return m_int_constant; }

	/**
	 * @brief Set the token's string content
	 */
	void set_string(std::string const &str) { m_string = str; }

	/**
	 * @brief Return the token's string content
	 */
	std::string const &get_string() const { return m_string; }

	/**
	 * @brief Print the token to an output stream
	 */
	void print(std::ostream &os) const { os << m_coord << "   " << m_string; }

	/**
	 * @brief Indicate if the token represents a binary operator
	 */
	bool is_binop() const;

	/**
	 * @brief Indicate if the token is a storage class specifier
	 */
	bool is_storage_class_specifier() const;

	/**
	 * @brief Indicate if the token is a type specifier
	 */
	bool is_type_qualifier() const;

private:
	std::string m_string;
	union {
		int m_int_constant;
		double m_double_constant;
	};
	Id m_id;
	Coordinate m_coord;
};

std::ostream &operator<<(std::ostream &os, Token const &tok);

#endif
