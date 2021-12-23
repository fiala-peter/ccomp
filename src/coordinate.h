/**
 * @file coordinate.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief Declaration of class ::Coordinate
 * @version 0.1
 * @date 2021-01-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef COORDINATE_H_INCLUDED
#define COORDINATE_H_INCLUDED

#include <iostream>

/**
 * @brief This class stores the line and column number of an identified token
 */
class Coordinate
{
public:
	Coordinate(size_t line = 0, size_t col = 0);

	void print(std::ostream &os) const;

	size_t get_line() const { return m_line; }
	size_t get_col() const { return m_col; }

private:
	size_t m_line;
	size_t m_col;
};

std::ostream &operator<<(std::ostream &os, Coordinate const &c);

#endif
