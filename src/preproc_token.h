#ifndef PREPROC_TOKEN_H_INCLUDED
#define PREPROC_TOKEN_H_INCLUDED

#include <string>

/** @brief class representing a preprocessor token */
class PreprocToken
{
public:
	/** @brief preprocessor tokens are classified into these categories */
	enum Category
	{
		CHARACTER_CONSTANT,		//< 'a', '\n', ...
		HEADER_NAME,			///< <stdio.h> or "myheader.h"
		IDENTIFIER,				///< apple
		NEW_LINE,				///< \n
		OTHER,
		PP_NUMBER,				///< any number constant (integer or floating point)
		PUNCTUATOR,				///< like . , { }
		STRING_LITERAL,			///< like "hello baby"
		WHITE_SPACE_SEQUENCE	/// any sequence of ' ', '\t'
	};

	/** @brief Set the category member */
	void set_category(Category const &cat) { m_cat = cat; }

	/** @brief Get the category member */
	Category const &get_category() const { return m_cat; }

	/** @brief Set the string member */
	void set_string(std::string const &str) { m_str = str; }

	/** @brief Get the string member */
	std::string const &get_string() const { return m_str; }

	/** @brief indicate if the token is white space or not */
	bool is_white_space() const { return get_category() == WHITE_SPACE_SEQUENCE; }

private:
	Category m_cat;
	std::string m_str;
};

#endif
