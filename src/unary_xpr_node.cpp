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
		set_xpr_type(child_type.pointer_to());
		return true;
	}
	case XprNode::Id::SIZEOF:
	{
		if (child_type.is_function())
		{
			std::cerr << "sizeof function is illegal" << std::endl;
			return false;
		}
#if 0		
		if (child_type.is_incomplete())
		{
			std::cerr << "sizeof incomplete type is illegal" << std::endl;
			return false;
		}
#endif
		set_xpr_type(Type::ulong_type());
		return true;
	}
	case XprNode::Id::DEREFERENCE:
	{
		if (!child_type.is_pointer())
		{
			std::cerr << "The argument of the dereferenc operator should be a pointer."
					  << std::endl;
			return false;
		}
		set_xpr_type(child_type.referenced_type());
		return true;
	}
	case XprNode::Id::PREINCREMENT:
	case XprNode::Id::PREDECREMENT:
	{
		if (!child_xpr->is_modifiable_lvalue())
		{
			std::cerr << " The argument of prein/decrement should be a modifiable lvalue."
					  << std::endl;
			return false;
		}
		if (!child_type.is_pointer() && !child_type.is_real())
		{
			std::cerr << " The argument of prein/decrement should be pointer or real."
					  << std::endl;
			return false;
		}
		set_xpr_type(child_type);
		return true;
	}
	case XprNode::Id::UNARY_MINUS:
	case XprNode::Id::UNARY_PLUS:
	{
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
