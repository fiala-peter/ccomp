#ifndef LABEL_H_INCLUDED
#define LABEL_H_INCLUDED

#include <iostream>
#include <string>

class Label
{
public:
	Label(int v = 0, int s = 0) : m_value(v), m_scope(s) {}

	std::string str() const;

	int get_scope() const { return m_scope; }

private:
	int m_value;
	int m_scope;
};

std::ostream &operator<<(std::ostream &os, Label const &l);

#endif
