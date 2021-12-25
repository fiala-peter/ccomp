#include "translation_unit.h"

void TransUnitNode::print(std::ostream &os, size_t level) const
{
	for (size_t i = 0; i < level; i++)
		os << ((i == level - 1) ? "+---" : "    ");

	os << "TRANSLATION_UNIT" << std::endl
	   << *get_symbol_pointer() << std::endl;
	for (auto c : get_functions())
		c->print(os, level + 1);
}
