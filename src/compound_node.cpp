#include "compound_node.h"

void CompoundNode::print(std::ostream &os, size_t level) const
{
	for (size_t i = 0; i < level; i++)
		os << ((i == level - 1) ? "+---" : "    ");
	os << "COMPOUND_STATEMENT:" << std::endl
	   << *get_symbol_pointer() << std::endl;
	for (auto c : get_substms())
		c->print(os, level + 1);
}
