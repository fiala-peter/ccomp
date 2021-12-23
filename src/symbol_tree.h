/**
 * @file symbol_tree.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of the tree of symbol tables
 * @version 0.1
 * @date 2021-01-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef SYMBOL_TREE_H_INCLUDED
#define SYMBOL_TREE_H_INCLUDED

#include "serializable.h"
#include "symbol_table.h"
#include "type.h"

#include <vector>

/** @brief one node of the tree of symbol tables */
class SymbolNode
	: public Serializable
{
public:
	/** @brief return pointer to the parent node */
	SymbolNode *get_parent() { return m_parent; }

	/** @brief indicate if the node is root */
	bool is_root() const { return m_parent == nullptr; }

	/** default constructor */
	SymbolNode() : m_parent(nullptr) { }

	/** @brief set the parent pointer */
	void set_parent(SymbolNode *p) { m_parent = p; }

	/** @brief local lookup of an identifier */
	SymbolTableEntry *lookup_local(std::string const &id);

	/** @brief global lookup of an identifier */
	SymbolTableEntry *lookup_global(std::string const &id);

	/** @brief global lookup of a constant */
	int const *lookup_constant_global(std::string const &id) const;

	/** @brief install a new typedef into the symbol tree
	 * @return true if the type could be installed or has already been there, false if the typedef name is reserved for an other type
	*/
	bool install_type(std::string const &id, Type const &type)
	{
		if (!m_symbol_table.install(SymbolTableEntry::TYPE, id, type, Storage::NO_STORAGE))
			return *m_symbol_table.lookup_type(id) == type;
		return true;
	}
	
	/** @brief install a new object into the symbol tree
	 * @return true if the object could be installed, false if it has already been installed
	*/
	bool install_object(std::string const &id, Type const &type, Storage storage)
	{
		return m_symbol_table.install(SymbolTableEntry::OBJECT, id, type, storage);
	}

	/** @brief install a new function into the symbol tree */
	bool install_function(std::string const &id, Type const &type, Storage storage = Storage::EXTERN)
	{
		if (!m_symbol_table.install(SymbolTableEntry::FUNCTION, id, type, storage))
			return *m_symbol_table.lookup_type(id) == type;
		return true;
	}

	void print(std::ostream &os, size_t level = 0) const override;

	/** @brief return the symbol table of the current node */
	SymbolTable const &get_symbols() const { return m_symbol_table; }

private:
	SymbolTable m_symbol_table;
	SymbolNode *m_parent;
};

#endif
