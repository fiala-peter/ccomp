#include "function_node.h"

void FunctionNode::print(std::ostream &os, size_t level) const
{
	for (size_t i = 0; i < level; i++)
		os << ((i == level - 1) ? "+---" : "    ");

	os << "FUNCTION: " << get_identifier() << " returning " << m_return_type << std::endl
	   << *get_symbol_pointer() << std::endl;
	m_block->print(os, level + 1);
}
