#include "label.h"

std::string Label::str() const
{
	return std::string(".L") + std::to_string(m_value);
}

std::ostream &operator<<(std::ostream &os, Label const &l)
{
	return os << l.str();
}
