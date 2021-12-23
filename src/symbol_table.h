/**
 * @file symbol_table.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief Class representing the Symbol table and its operations
 * @version 0.1
 * @date 2021-01-30
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SYMBOL_TABLE_H_INCLUDED
#define SYMBOL_TABLE_H_INCLUDED

#include "serializable.h"
#include "storage.h"
#include "type.h"

#include <iomanip>
#include <list>
#include <string>

/** @brief one entry of the symbol table */
class SymbolTableEntry
	: public Serializable
{
public:
	/** @brief Symbol table entry categories */
	enum Category
	{
		TYPE,
		OBJECT,
		FUNCTION
	};

	SymbolTableEntry(Category cat, std::string const &id, Type const &type, Storage storage, Linkage linkage = Linkage::NO_LINKAGE)
		: m_category(cat)
		, m_identifier(id)
		, m_type(type)
		, m_storage(storage)
		, m_linkage(linkage)
	{
	}

	/** @brief return the category of a symbol table entry */
	Category get_category() const { return m_category; }

	/** @brief return the identifier of a symbol table entry */
	std::string const &get_id() const { return m_identifier; }

	/** @brief return the type of a symbol table entry */
	Type const &get_type() const { return m_type; }

	/** @brief return the type of a symbol table entry */
	Type &get_type() { return m_type; }

	/** @brief indicates if the entry is a typedef name */
	bool is_type() const { return m_category == TYPE; }

	/** @brief indicates if the entry is an object */
	bool is_object() const { return m_category == OBJECT; }

	/** @brief indicates if the entry is a function */
	bool is_function() const { return m_category == FUNCTION; }

	/** @brief indicates if the entry (object) is external */
	bool is_extern() const { return m_storage == Storage::EXTERN; }

	/** @brief indicates if the entry (object) is external */
	bool is_static() const { return m_storage == Storage::STATIC; }

	Linkage get_linkage() const { return m_linkage; }

	/** @brief print an entry to an output stream */
	void print(std::ostream &os, size_t level = 0) const override
	{
		std::string catnames[] = {"TYPE", "OBJECT", "FUNCTION"};
		os << std::setw(8) << catnames[(int)m_category];
		os << std::setw(20) << m_identifier << ":\t" << m_type;
	}

private:
	Category m_category;
	std::string m_identifier;
	Type m_type;
	Storage m_storage;
	Linkage m_linkage;
};

/** @brief Class representing a Symbol table and its operations */
class SymbolTable
	: std::list<SymbolTableEntry>,
	  public Serializable
{
public:
	using Container = std::list<SymbolTableEntry>;
	using const_iterator = Container::const_iterator;

	using std::list<SymbolTableEntry>::begin;
	using std::list<SymbolTableEntry>::end;
	using std::list<SymbolTableEntry>::cbegin;
	using std::list<SymbolTableEntry>::cend;
	using std::list<SymbolTableEntry>::crbegin;
	using std::list<SymbolTableEntry>::crend;

	/** @brief lookup a symbol in the table and return a pointer to its entry
	 *  @return pointer to the table entry of nullptr
	 */
	SymbolTableEntry *lookup(std::string const &id)
	{
		for (auto &entry : *this)
			if (entry.get_id() == id)
				return &entry;
		return nullptr;
	}

	/**
	 *  @brief install a new entry into the table
	 *  @return true if a new entry needed to be allocated. False if the entry was already contained.
	 */
	bool install(SymbolTableEntry::Category cat, std::string const &id, Type const &type, Storage storage)
	{
		if (lookup(id) != nullptr)
			return false;
		this->push_front(SymbolTableEntry(cat, id, type, storage));
		return true;
	}

	/** @brief lookup a symbol's type
	 * @return pointer to the type
	 */
	Type *lookup_type(std::string const &id)
	{
		SymbolTableEntry *pe = lookup(id);
		if (pe == nullptr)
			return nullptr;
		return &(pe->get_type());
	}

	/** @brief lookup an enumeration constant */
	int const *lookup_constant(std::string const &id) const
	{
		for (auto const &se : *this)
		{
			Type const &t = se.get_type();
			if (t.is_enumerated())
			{
				int const *p = t.lookup_enum_constant(id);
				if (p != nullptr)
					return p;
			}
		}
		return nullptr;
	}

	void print(std::ostream &os, size_t level = 0) const override
	{
		for (auto s : *this)
			os << s << std::endl;
	}

	/** @brief indicates if the table is empty */
	bool empty() const { return Container::empty(); }
};

#endif // SYMBOL_TABLE_H_INCLUDED
