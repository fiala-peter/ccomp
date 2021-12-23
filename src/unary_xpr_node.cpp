#include "unary_xpr_node.h"
#include "binary_xpr_node.h"

bool UnaryXprNode::type_check_impl()
{
	XprNode *child_xpr = get_subxpr(0);
	Type const &child_type = child_xpr->get_xpr_type();

	switch (get_id())
	{
	case XprNode::Id::ADDRESS_OF:
	{
		/*
			The operand of the unary & operator shall be either a function designator or an lvalue
			that designates an object that is not a bit-field and is not declared with the register
			storage-class specifier.
		*/

		// todo the above constraints are not implemented here precisely
		if (!child_type.is_function() && !child_xpr->is_modifiable_lvalue())
		{
			std::cerr << "operand of the unary & operator shall be either a function designator or an lvalue that designates an object" << std::endl;
			return false;
		}

		set_xpr_type(child_type.pointer_to());
		return true;
	}
	case XprNode::Id::SIZEOF:
	{
		/*
			The sizeof operator shall not be applied to an expression that has function type or an
			incomplete type, to the parenthesized name of such a type, or to an lvalue that
			designates a bit-field object.
		*/
		if (child_type.is_function())
		{
			std::cerr << "sizeof function is illegal" << std::endl;
			return false;
		}

		if (child_type.is_incomplete())
		{
			std::cerr << "sizeof incomplete type is illegal" << std::endl;
			return false;
		}

		set_xpr_type(Type::ulong_type());
		return true;
	}
	case XprNode::Id::DEREFERENCE:
	{
		/*
		The operand of the unary * operator shall have pointer type.
		*/
		if (!child_type.is_pointer())
		{
			std::cerr << "The argument of the dereference operator should be a pointer." << std::endl;
			return false;
		}
		set_xpr_type(child_type.referenced_type());
		return true;
	}
	case XprNode::Id::PREINCREMENT:
	case XprNode::Id::PREDECREMENT:
	{
		/*
			The operand of the prefix increment or decrement operator shall have qualified or
			 unqualified scalar type and shall be a modifiable lvalue.
		*/
		if (!child_xpr->is_modifiable_lvalue())
		{
			std::cerr << " The argument of prein/decrement should be a modifiable lvalue." << std::endl;
			return false;
		}
		if (!child_type.is_scalar())
		{
			std::cerr << " The argument of prein/decrement should be scalar." << std::endl;
			return false;
		}
		set_xpr_type(child_type);
		return true;
	}
	case XprNode::Id::UNARY_MINUS:
	case XprNode::Id::UNARY_PLUS:
	{
		/*
		The operand of the unary + or - operator shall have arithmetic type
		*/
		if (!child_type.is_arithmetic())
		{
			std::cerr << "Argument of unary + and - operators should be arithmetic type" << std::endl;
			return false;
		}
		if (child_type.is_integer())
			set_subxpr(0, conditional_cast(child_xpr, child_type.integer_promote()));
		set_xpr_type(get_subxpr(0)->get_xpr_type());
		return true;
	}
	case Id::BITWISE_NOT:
	{
		/*
		The operand of the unary ~ operator should have integral type.
		*/
		if (!child_type.is_integer())
		{
			std::cerr << "Argument of bitwise NOT operators should be integer type" << std::endl;
			return false;
		}
		set_subxpr(0, conditional_cast(child_xpr, child_type.integer_promote()));
		set_xpr_type(get_subxpr(0)->get_xpr_type());
		return true;
	}
	case Id::LOGICAL_NOT:
	{
		/*
		The operand of the unary ! operator should have scalar type.
		*/
		if (!child_type.is_scalar())
		{
			std::cerr << "Argument of logical NOT operator should be scalar type" << std::endl;
			return false;
		}
		set_xpr_type(Type::int_type());
		return true;
	}
	default:
		throw __FILE__ ": Unhandled operator for unary type check";
	}
	return false;
}
