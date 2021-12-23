#include "symbol_tree.h"

SymbolTableEntry *SymbolNode::lookup_local(std::string const &id)
{
	return m_symbol_table.lookup(id);
}

SymbolTableEntry *SymbolNode::lookup_global(std::string const &id)
{
	SymbolNode *p = this;
	while (p != nullptr)
	{
		SymbolTableEntry *ste = p->m_symbol_table.lookup(id);
		if (ste != nullptr)
			return ste;
		p = p->m_parent;
	}
	return nullptr;
}

int const *SymbolNode::lookup_constant_global(std::string const &id) const
{
	SymbolNode const *p = this;
	while (p != nullptr)
	{
		int const *t = p->m_symbol_table.lookup_constant(id);
		if (t != nullptr)
			return t;
		p = p->m_parent;
	}
	return nullptr;
}

void SymbolNode::print(std::ostream &os, size_t level) const
{
	if (!m_symbol_table.empty())
	{
		os << "----------------------" << std::endl
		   << m_symbol_table
		   << "----------------------" << std::endl;
	}
}
