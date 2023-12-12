/**
 * @file type.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class ::Type
 * @version 0.1
 * @date 2021-01-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#include "serializable.h"
#include "type_node.h"

#include <iostream>

/**
 * @brief Representation of C types and their operations
 * 
 * @details All types generated during compilation are stored in a static
 * forest of types. Each type is represented as a linked list.
 * The type  int *[3] , for example, is represented as a three-element list as
 * pointer-to -> array[3]-of -> int.
 * Type lists sharing trailing sublists are combined into trees.
 * Each type derived from the same base type (e.g. int) are in a single tree.
 * Trees with different base types form the type forest.
 * 
 * The Type class contains a pointer to the actual node in the type forest.
 * This choice makes type comparisons easy (comparing addresses), and allows
 * to modify types referenced by other types during compilation.
 */
class Type
	: public Serializable
{
public:
	/**
	 * @brief Construct a new Type object
	 * @param ptr The pointer to the type's head in the type tree
	 */
	Type(TypeTreeNode *ptr = nullptr) : m_ptr(ptr) { }

	/** @brief Determine if the type is an integer type or not */
	bool is_integer() const { return TypeNode::is_integer(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is an enumerated type or not */
	bool is_enumerated() const { return TypeNode::is_enumerated(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is complete or not */
	bool is_incomplete() const { return m_ptr->get_node().is_incomplete(); }

	/** @brief Determine if the type is complete object type or not */
	bool is_complete_object() const { return !is_incomplete() && is_object(); }

	/** @brief Lookup the value of a symbol for an enumerated type */
	int const *lookup_enum_constant(std::string const &id) const { return m_ptr->get_node().lookup_enum_constant(id); }

	/** @brief Lookup the type of a field of a structure */
	Type const *lookup_structure_field_type(std::string const &id) const { return m_ptr->get_node().lookup_structure_field_type(id); }

	/** @brief Lookup the offset of a structure's field */
	size_t lookup_structure_field_offset(std::string const &id) const { return m_ptr->get_node().lookup_structure_field_offset(id); }

	/** @brief Determine if the type is floating or not */
	bool is_floating() const { return TypeNode::is_floating(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is a signed integer */
	bool is_signed_integer() const { return TypeNode::is_signed_integer(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is an unsigned integer */ 
	bool is_unsigned_integer() const { return TypeNode::is_unsigned_integer(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is void */ 
	bool is_void() const { return TypeNode::is_void(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is object type */
	bool is_object() const { return TypeNode::is_object(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is arithmetic type */
	bool is_arithmetic() const { return TypeNode::is_arithmetic(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is real type */
	bool is_real() const { return TypeNode::is_real(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is pointer type */
	bool is_pointer() const { return TypeNode::is_pointer(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is a structure type */
	bool is_structure() const { return TypeNode::is_structure(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is array type */
	bool is_array() const { return TypeNode::is_array(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is function type */
	bool is_function() const { return TypeNode::is_function(m_ptr->get_node().get_id()); }

	/** @brief Determine if the type is a variadic argument function type */
	bool is_vararg() const { return m_ptr->get_node().is_vararg(); }

	/** @brief Determine if the type is scalar type */
	bool is_scalar() const { return TypeNode::is_scalar(m_ptr->get_node().get_id()); }

	/** @brief Returns the referenced type of a pointer type */
	Type referenced_type() const;

	/** @brief Returns the element type of an array type */
	Type element_type() const;

	/** @brief Returns the return type of a function type */
	Type return_type() const;

	/** @brief Returns a pointer type to the actual type */
	Type pointer_to() const { return attach(TypeNode(TypeNode::POINTER)); }

	/** @brief Returns an array type consising of the actual type */
	Type array_of(size_t nelem) const { return attach(TypeNode(TypeNode::ARRAY, nelem)); }

	/** @brief Returns a function type returning the actual type */
	Type function_returning(TypeNode::Declarations const &ptl, bool is_vararg) const;

	/** @brief Returns the unqualified version of a type */
	Type unqualified() const 
	{
		// TODO implement this feature
		return *this;
	}

	/** @brief Returns a void type */
	static Type void_type() { return install(TypeNode(TypeNode::VOID)); }

	/** @brief Returns a char type */
	static Type char_type() { return install(TypeNode(TypeNode::CHAR)); }

	/** @brief Returns a signed char type */
	static Type schar_type() { return install(TypeNode(TypeNode::SCHAR)); }

	/** @brief Returns an unsigned char type */
	static Type uchar_type() { return install(TypeNode(TypeNode::UCHAR)); }

	/** @brief Returns a short type */
	static Type short_type() { return install(TypeNode(TypeNode::SHORT)); }

	/** @brief Returns an unsigned short type */
	static Type ushort_type() { return install(TypeNode(TypeNode::USHORT)); }

	/** @brief Returns an int type */
	static Type int_type() { return install(TypeNode(TypeNode::INT)); }

	/** @brief Returns an unsigned int type */
	static Type uint_type() { return install(TypeNode(TypeNode::UINT)); }

	/** @brief Returns a long int type */
	static Type long_type() { return install(TypeNode(TypeNode::LONG)); }

	/** @brief Returns an unsigned long int type */
	static Type ulong_type() { return install(TypeNode(TypeNode::ULONG)); }

	/** @brief Returns a long long int type */
	static Type llong_type() { return install(TypeNode(TypeNode::LLONG)); }

	/** @brief Returns an unsigned long long int type */
	static Type ullong_type() { return install(TypeNode(TypeNode::ULLONG)); }

	/** @brief Returns a float type */
	static Type float_type() { return install(TypeNode(TypeNode::FLOAT)); }

	/** @brief Returns a double type */
	static Type double_type() { return install(TypeNode(TypeNode::DOUBLE)); }

	/** @brief Determines if two types are compatible */
	bool is_compatible_with(Type const &other) const;

	/** @brief Returns the size of the type in bytes */
	size_t get_size_in_bytes() const;

	/** @brief Returns the type's alignment in bytes */
	size_t get_alignment_in_bytes() const;

	/** @brief Compares two types for equality */
	bool operator==(Type const &other) const { return m_ptr == other.m_ptr; }

	/** @brief Determines if two types are different or not */
	bool operator!=(Type const &other) const { return !(*this == other); }

	/** @brief Determines the common real type of two arithmetic types */
	Type common_real_type(Type const &other) const;

	/** @brief Computes integer promotions on a type */
	Type integer_promote() const;

	/**
	 * @brief Apply the current type's type modifiers to an other type
	 * @param other The other type
	 * 
	 * @details For example if the current type is pointer to char, and the other type is array of int
	 * then the resulting type is pointer to array of int. This operation is useful
	 * when parsing complicated declarations like char (*p)[3], where the pointer-to
	 * type modifier needs to be applied to array[3] of char.
	 * @return The resulting type's head
	 */
	Type replace_back_to(Type const &other) const;

	/**
	 * @brief Replace the first array[0]-of modifier to pointer-to modifier
	 * @return Type The resulting type after replacement
	 * 
	 * @details This operation is useful when parsing function parameter
	 * types in the form   int p[][3]   that need to be simplified to
	 * int (*p)[3]
	 */
	Type first_empty_array_to_pointer() const;

	/** @brief Print a type to an output stream */
	void print(std::ostream &os, size_t level = 0) const override;

	/** @brief Returns the number of declarations of a function or structure */
	size_t get_num_declarations() const;

	/** @brief Returns the i-th declaration of a function  or structure list */
	Declaration const &get_declaration(size_t i) const;

	/** 
	 * @brief Install a new type node to the type tree as a new root
	 * @param as_new forces that a new type should be installed
	 * 
	 * @details This function is used for basic types like char that
	 * are root nodes of the type forest.
	 */
	static Type install(TypeNode const &tn, bool as_new = false);

	/**
	 * @brief Attach a new type node to the head of an existing type
	 * @param node The new type node (new head type node)
	 * @return The new type
	 */
	Type attach(TypeNode const &node) const;

	/** @brief Print the total type pool to an output stream */
	static void print_pool(std::ostream &os);

	/** @brief add a new enumeration constant to an enumerated type */
	void add_enum_constant(std::string const &id, int val) { m_ptr->get_node().add_enum_constant(id, val); }

	/** @brief Add a new field to a structure type */
	void add_field(std::string const &id, Type const &type) { m_ptr->get_node().add_field(id, type); }

	/** @brief free the whole type forest (at the end of everything) */
	static void free_pool()
	{
		for (auto s : m_pool)
			delete s;
	}

	/** @brief return a pointer to function type for function types */
	Type function_to_pointer_cast() const
	{
		if (is_function())
			return pointer_to();
		return *this;
	}

	/** @brief return a pointer to element type for array types */
	Type array_to_pointer_cast() const
	{
		if (is_array())
			return element_type().pointer_to();
		return *this;
	}

private:
	static int print_rec(TypeTreeNode *c, std::ostream &os);

private:
	TypeTreeNode *m_ptr;
	static std::vector<TypeTreeNode *> m_pool;
	static size_t m_tag_cntr;
	static int m_label;
};

#endif
