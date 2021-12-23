/**
 * @file declaration.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class Declaration
 */
#ifndef DECLARATION_H_INCLUDED
#define DECLARATION_H_INCLUDED

#include "storage.h"
#include "type.h"

#include <string>

/** @brief representation of C declarations */
class Declaration
{
public:
	/** @brief constructor */
	Declaration()
		: m_is_typedef(false)
		, m_storage(Storage::NO_STORAGE)
		, m_linkage(Linkage::UNDEFINED_LINKAGE)
	{
	}

	/** @brief constructor */
	Declaration(Type const &t, std::string const &id = "")
		: m_type(t)
		, m_identifier(id)
		, m_is_typedef(false)
		, m_storage(Storage::NO_STORAGE)
		, m_linkage(Linkage::UNDEFINED_LINKAGE)
		, m_scope(Scope::BLOCK_SCOPE)
	{
	}

	/** @brief set the type member */
	void set_type(Type const &type) { m_type = type; }

	/** @brief get the type */
	Type const &get_type() const { return m_type; }

	/** @brief set the identifier */
	void set_identifier(std::string const &id) { m_identifier = id; }

	/** @brief get the identifier */
	std::string const &get_identifier() const { return m_identifier; }

	/** @brief set the typedef indicator */
	void set_typedef(bool td) { m_is_typedef = td; }

	/** @brief indicate if the declaration is a typedef */
	bool is_typedef() const { return m_is_typedef; }

	/** @brief set the storage mode */
	void set_storage(Storage st) { m_storage = st; }

	Storage get_storage() const { return m_storage; }

	/** @brief set the linkage mode */
	void set_linkage(Linkage link) { m_linkage = link; }

	Linkage get_linkage() const { return m_linkage; }

	/** @brief set the scope mode */
	void set_scope(Scope scope) { m_scope = scope; }

	Scope get_scope() const { return m_scope; }

	/** @brief indicate if the declaration is abstract */
	bool is_abstract() const { return m_identifier.empty(); }

	/** @brief indicate if the declaration is extern */
	bool is_extern() const { return m_storage == Storage::EXTERN; }

	/** @brief indicate if the declaration is static */
	bool is_static() const { return m_storage == Storage::STATIC; }

private:
	std::string m_identifier;
	Type m_type;
	bool m_is_typedef;
	Storage m_storage;
	Linkage m_linkage;
	Scope m_scope;
};

#endif
