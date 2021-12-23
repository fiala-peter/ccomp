#include "cast_xpr_node.h"

bool CastXprNode::type_check_impl()
{
	/*
		Unless the type name specifies void type, the type name shall specify qualified
		or unqualified scalar type and the operand shall have scalar type.
	 */
	XprNode *child_xpr = get_subxpr(0);
	Type const &child_type = child_xpr->get_xpr_type();
	Type const &casted_type = get_xpr_type();

	if (casted_type == Type::void_type())
		return true;
	if (!casted_type.is_scalar() || !child_type.is_scalar())
	{
		std::cerr << "Both the operand and the result of the cast operator should be scalar type" << std::endl;
		return false;
	}
	if (child_type.is_pointer() && casted_type.is_floating())
	{
		std::cerr << "Casting from pointer to floating type is not allowed." << std::endl;
		return false;
	}
	if (child_type.is_floating() && casted_type.is_pointer())
	{
		std::cerr << "Casting from floating type to pointer is not allowed." << std::endl;
		return false;
	}
	return true;
}
