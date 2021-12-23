#include "conditional_xpr_node.h"

ConditionalXprNode::ConditionalXprNode()
	: XprNode(XprNode::Id::CONDITIONAL)
{
}

bool ConditionalXprNode::type_check_impl()
{
	XprNode const *cond_xpr = get_subxpr(0);
	XprNode const *tr_xpr = get_subxpr(1);
	XprNode const *fl_xpr = get_subxpr(2);

	Type cond_type = cond_xpr->get_xpr_type();
	Type tr_type = tr_xpr->get_xpr_type();
	Type fl_type = fl_xpr->get_xpr_type();

	Type ret_type;

	if (!cond_type.is_scalar())
	{
		std::cerr << "The first argument of the conditional operator should be scalar" << std::endl;
		return false;
	}

	if (tr_type.is_arithmetic() && fl_type.is_arithmetic())
		ret_type = tr_type.common_real_type(fl_type);
	else if (tr_type == Type::void_type() && fl_type == Type::void_type())
		ret_type = Type::void_type();
	else if (tr_type.is_pointer() && fl_type.is_pointer() && tr_type.unqualified().is_compatible_with(fl_type.unqualified()))
		ret_type = tr_type;	// TODO all the qualifiers of f should be taken to t
	else if (tr_type.is_pointer() && fl_xpr->is_nullpointer_constant())
		ret_type = tr_type;
	else if (tr_xpr->is_nullpointer_constant() && fl_type.is_pointer())
		ret_type = fl_type;

	set_xpr_type(ret_type);
	return true;
}
