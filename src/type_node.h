/**
 * @file type_node.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class ::TypeNode
 * @version 0.1
 * @date 2021-01-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef TYPE_NODE_H_INCLUDED
#define TYPE_NODE_H_INCLUDED

#include <iostream>
#include <vector>

#include "serializable.h"

class Declaration;
class Type;

/** @brief The data stored in the nodes of the type tree */
class TypeNode
	: public Serializable
{
public:
	using Declarations = std::vector<Declaration>;

	enum Id
	{
		VOID = 1 << 0,
		CHAR = 1 << 1,
		SCHAR = 1 << 2,
		UCHAR = 1 << 3,
		SHORT = 1 << 4,
		USHORT = 1 << 5,
		INT = 1 << 6,
		UINT = 1 << 7,
		LONG = 1 << 8,
		ULONG = 1 << 9,
		LLONG = 1 << 10,
		ULLONG = 1 << 11,
		FLOAT = 1 << 12,
		DOUBLE = 1 << 13,
		LDOUBLE = 1 << 14,
		POINTER = 1 << 15,
		ARRAY = 1 << 16,
		FUNCTION = 1 << 17,
		ENUM = 1 << 18,
		STRUCT = 1 << 19,
		UNION = 1 << 20
	};

	/** @brief base type constructor */
	explicit TypeNode(Id id)
		: m_id(id), m_size(size_in_bytes(id)), m_alignment(size_in_bytes(id))
		, m_n_declarations(0), m_declarations(nullptr)
		, m_is_vararg(false)
	{
	}

	/** @brief array constructor */
	TypeNode(Id id, size_t nelem)
		: m_id(id), m_size(nelem), m_alignment(0)
		, m_n_declarations(0), m_declarations(nullptr)
		, m_is_vararg(false)
	{
		if (id != TypeNode::ARRAY)
			throw __FILE__ ": This constructor is only valid for array nodes";
	}

	/** @brief enum/struct/union constructor */
	TypeNode(Id id, std::string const &tag)
		: m_id(id), m_size(0), m_alignment(0)
		, m_n_declarations(0), m_declarations(nullptr)
		, m_is_vararg(false)
		, m_tag(tag)
	{
		if (id != TypeNode::ENUM && id != TypeNode::STRUCT && id != TypeNode::UNION)
			throw __FILE__ ": This constructor is only valid for enum, struct and union type nodes";
	}

	/** @brief assignment operator */
	TypeNode const &operator=(TypeNode const &other);

	/** @brief copy constructor */
	TypeNode(TypeNode const &other) : m_declarations(nullptr) { *this = other; }

	~TypeNode();

	/** @brief return the type id */
	TypeNode::Id get_id() const { return m_id; }

	/** @brief For arrays it returns the number of elements, otherwise the size in bytes.
	 * For incomplete types, it returns 0 */
	size_t get_size() const { return m_size; }

	/** @brief returns the type's alignment */
	size_t get_alignment() const { return m_alignment; }

	/** @brief compares two type nodes for equality */
	bool operator==(TypeNode const &other) const;

	/** @brief compares two type nodes for difference */
	bool operator!=(TypeNode const &other) const { return !(*this == other); }

	/** @brief prints a type node to an output stream */
	void print(std::ostream &os, size_t level = 0) const override;

	/** @brief add a declaration to a function or structure type node */
	void add_declarations(Declarations const &dl);

	/** @brief return the number of declarations of a function or structure type node */
	size_t get_num_declarations() const { return m_n_declarations; }

	/** @brief return the declarations of a function or structure type node */
	Declaration const &get_declaration(size_t i) const;

	/** @brief return a base type's size in bytes */
	static size_t size_in_bytes(Id arg);

	/** @brief determine if a base type is an object type or not */
	static bool is_object(Id arg) { return !is_function(arg); }

	/** @brief determine if a base type is a function type or not */
	static bool is_function(Id arg) { return arg == FUNCTION; }

	/** @brief determine if a base type is a signed integer or not */
	static bool is_signed_integer(Id arg)
	{ 
		int v = SCHAR | SHORT | INT | LONG | LLONG;
		return arg & v;
	}

	/** @brief determine if a base type is an unsigned integer or not */
	static bool is_unsigned_integer(Id arg)
	{
		int v = UCHAR | USHORT | UINT | ULONG | ULLONG;
		return arg & v;
	}

	/** @brief determine if a base type is a floating type or not */
	static bool is_floating(Id arg)
	{
		int v = FLOAT | DOUBLE | LDOUBLE;
		return arg & v;
	}

	/** @brief determine if a base type is a character type or not */
	static bool is_character(Id arg)
	{
		int v = CHAR | SCHAR | UCHAR;
		return arg & v;
	}

	/** @brief determine if a base type is a basic type or not */
	static bool is_basic(Id arg)
	{
		return arg == CHAR || is_signed_integer(arg) || is_unsigned_integer(arg) || is_floating(arg);
	}

	/** @brief determine if a base type is an enumerated type or not */
	static bool is_enumerated(Id arg) { return arg == ENUM; }

	/** @brief determine if a base type is an integer type or not */
	static bool is_integer(Id arg)
	{
		return arg == CHAR || is_signed_integer(arg) || is_unsigned_integer(arg) || is_enumerated(arg);
	}

	/** @brief determine if a base type is an arithmetic type or not */
	static bool is_arithmetic(Id arg) { return is_integer(arg) || is_floating(arg); }

	/** @brief determine if a base type is a real type or not */
	static bool is_real(Id arg) { return is_integer(arg) || is_floating(arg); }

	/** @brief determine if a base type is a void type or not */
	static bool is_void(Id arg) { return arg == VOID; }

	/** @brief determine if a base type is an array type or not */
	static bool is_array(Id arg) { return arg == ARRAY; }

	/** @brief determine if a base type is a structure type or not */
	static bool is_structure(Id arg) { return arg == STRUCT; }

	/** @brief determine if a base type is a union type or not */
	static bool is_union(Id arg) { return arg == UNION; }

	/** @brief determine if a base type is a pointer type or not */
	static bool is_pointer(Id arg) { return arg == POINTER; }

	/** @brief determine if a base type is a scalar type or not */
	static bool is_scalar(Id arg) { return is_arithmetic(arg) || is_pointer(arg); }

	/** @brief determine if a base type is an aggregate type or not */
	static bool is_aggregate(Id arg) { return is_array(arg) || is_structure(arg); }

	/** @brief determine if a base type is a vararg function type or not */
	bool is_vararg() const { return m_is_vararg; }

	/** @brief sets the vararg state of a type node */
	void set_vararg(bool varg) { m_is_vararg = varg; }

	/** @brief determines if a can represent b */
	static bool can_represent(Id a, Id b);

	/** @brief returns the integer promoted type */
	static Id integer_promote(Id arg);

	/** @brief returns the integer conversion rank of an integer type */
	static int integer_conversion_rank(Id arg);

	/** @brief converts an integer to signed */
	static Id to_signed_integer(Id arg);

	/** @brief return the common real type of two types */
	static Id common_real_type(Id lhs, Id rhs);

	/** @brief determines if a type is incomplete */
	bool is_incomplete() const
	{
		if (is_array(m_id) && m_size == 0)
			return true;
		if (is_enumerated(m_id) && m_enum_constants.empty())
			return true;
		if (is_structure(m_id) && m_n_declarations == 0)
			return true;
		return false;
	}

	/** @brief add an enum constant to an enumerated type */
	void add_enum_constant(std::string const &id, int val)
	{
		if (get_id() != ENUM)
			throw __FILE__ ": Enum constants only valid for enum type nodes";
		for (size_t i = 0; i < m_enum_constants.size(); ++i)
			if (m_enum_constants[i].first == id)
				throw __FILE__ ": Enum constant duplicate error";
		// if a constant is added to the enum, it becomes complete, so its size and alignment is not zero anymore
		m_size = size_in_bytes(TypeNode::INT);
		m_alignment = size_in_bytes(TypeNode::INT);
		m_enum_constants.push_back(std::make_pair(id, val));
	}

	/** @brief add a field to a structure type */
	void add_field(std::string const &id, Type const &type);

	/** @brief Lookup an enumeration constant */
	int const *lookup_enum_constant(std::string const &id) const
	{
		if (get_id() != ENUM)
			throw __FILE__ ": Enum constants only valid for enum type nodes";
		for (auto const &s : m_enum_constants)
			if (s.first == id)
				return &s.second;
		return nullptr;
	}

	/** @brief Lookup a structure field's type */
	Type const *lookup_structure_field_type(std::string const &id) const;

	/** @brief Lookup a structure field's offset */
	size_t lookup_structure_field_offset(std::string const &id) const;

	/** @brief return the tag of a structure/enum type */
	std::string const &get_tag() const
	{
		if (m_id != ENUM && m_id != STRUCT)
			throw __FILE__ ": Requesting tag of non-enum / non-structure type node";
		return m_tag;
	}

	/** @brief set the tag of an enum / structure type */
	void set_tag(std::string const &tag)
	{
		if (m_id != ENUM && m_id != STRUCT)
			throw __FILE__ ": Requesting tag of non-enum type node";
		m_tag = tag;
	}

private:
	Id m_id;
	size_t m_size;
	size_t m_alignment;
	Declaration *m_declarations;
	std::vector<std::pair<std::string, int>> m_enum_constants;
	size_t m_n_declarations;
	bool m_is_vararg;
	std::string m_tag;
};

/** @brief Representation of Type tree nodes */
class TypeTreeNode
{
public:
	TypeTreeNode(TypeNode const &tn, TypeTreeNode *arg = nullptr)
		: m_node(tn)
	{
		set_arg(arg);
	}

	void set_arg(TypeTreeNode *arg)
	{
		m_arg = arg;
		if (m_arg != nullptr)
			m_arg->add_child(this);
	}

	TypeTreeNode *get_arg() const { return m_arg; }

	std::vector<TypeTreeNode *> const &get_children() const { return m_children; }

	TypeNode const &get_node() const { return m_node; }

	TypeNode &get_node() { return m_node; }

	void add_child(TypeTreeNode *c) { m_children.push_back(c); }

	TypeTreeNode(TypeTreeNode const &other) = delete;

	TypeTreeNode const &operator=(TypeTreeNode const &other) = delete;

	~TypeTreeNode()
	{
		for (auto c : m_children)
			delete c;
	}

private:
	TypeNode m_node;
	TypeTreeNode *m_arg;
	std::vector<TypeTreeNode *> m_children;
};

#endif
