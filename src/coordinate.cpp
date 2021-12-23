#include "coordinate.h"

#include <iomanip>

void Coordinate::print(std::ostream &os) const
{
	os << std::setw(3) << m_line << ':' << std::setw(2) << m_col;
}

std::ostream &operator<<(std::ostream &os, Coordinate const &c)
{
	c.print(os);
	return os;
}

Coordinate::Coordinate(size_t line, size_t col) : m_line(line), m_col(col)
{
}
