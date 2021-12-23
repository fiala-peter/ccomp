#include "token.h"

std::ostream &operator<<(std::ostream &os, Token const &tok)
{
	tok.print(os);
	return os;
}

bool Token::is_binop() const
{
	return m_id == Id::ASTERISK || m_id == Id::PER || m_id == Id::MOD
		|| m_id == Id::PLUS || m_id == Id::MINUS
		|| m_id == Id::SHR || m_id == Id::SHL
		|| m_id == Id::LESS || m_id == Id::LESS_EQUAL || m_id == Id::GREATER || m_id == Id::GREATER_EQUAL
		|| m_id == Id::EQ || m_id == Id::NEQ
		|| m_id == Id::AMPERSAND || m_id == Id::BITWISE_XOR || m_id == Id::BITWISE_OR
		|| m_id == Id::LOGICAL_AND || m_id == Id::LOGICAL_OR || m_id == Id::COMMA;
}

bool Token::is_storage_class_specifier() const
{
	return m_id == Id::EXTERN || m_id == Id::STATIC || m_id == Id::AUTO || m_id == Id::REGISTER;
}

bool Token::is_type_qualifier() const
{
	return m_id == Id::CONST || m_id == Id::VOLATILE || m_id == Id::RESTRICT;
}
