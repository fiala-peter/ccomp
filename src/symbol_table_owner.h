#ifndef SYMBOL_TABLE_OWNER_H_INCLUDED
#define SYMBOL_TABLE_OWNER_H_INCLUDED

#include "symbol_tree.h"

class SymbolTableOwner
{
public:
	SymbolTableOwner()
		: m_st(nullptr) {}

	~SymbolTableOwner() { delete m_st; }

	SymbolTableOwner(SymbolTableOwner const &other) = delete;

	SymbolTableOwner const &operator=(SymbolTableOwner const &other) = delete;

	void set_symbol_pointer(SymbolNode *ptr) { m_st = ptr; }

	SymbolNode *get_symbol_pointer() const { return m_st; }

private:
	SymbolNode *m_st;
};

#endif
