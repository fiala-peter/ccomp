#include "floating_constant.h"

FloatingConstant::FloatingConstant() : XprNode(XprNode::Id::FLOATING_XPR)
{
}

void FloatingConstant::set_value(double d)
{
	m_value = d;
}

void FloatingConstant::print(std::ostream &os, size_t level) const
{
	for (size_t i = 0; i < level; i++)
		os << ((i == level - 1) ? "+---" : "    ");

	os << "FLOATING_CONSTANT: " << m_value << std::endl;
}
