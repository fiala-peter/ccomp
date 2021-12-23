#ifndef ASSIGNMENT_XPR_NODE_H_INCLUDED
#define ASSIGNMENT_XPR_NODE_H_INCLUDED

#include "xpr_node.h"

class AssignmentXprNode : public XprNode
{
public:
	AssignmentXprNode(XprNode::Id op)
		: XprNode(op)
	{
	}

	bool type_check_impl() override
	{
		XprNode *lhs_xpr = get_subxpr(0);
		XprNode *rhs_xpr = get_subxpr(1);

		Type const &lhs_type = lhs_xpr->get_xpr_type();
		Type const &rhs_type = rhs_xpr->get_xpr_type();

		if (!lhs_xpr->is_modifiable_lvalue())
		{
			std::cerr << "Left hand side of assignment should be a modifiable lvalue." << std::endl;
			return false;
		}

		bool ok = false;
		if (this->get_id() == XprNode::Id::ASSIGN)
		{
			if (lhs_type.is_arithmetic() && rhs_type.is_arithmetic())
				ok = true;
			else if (lhs_type.is_structure() && lhs_type.is_compatible_with(rhs_type))
				ok = true;
			else if (lhs_type.is_pointer() && rhs_type.is_pointer() && lhs_type.referenced_type().is_compatible_with(rhs_type.referenced_type()))
				ok = true;
			else if (lhs_type.is_pointer() && lhs_type.referenced_type().is_object() && rhs_type.is_pointer() && rhs_type.referenced_type().is_void())
				ok = true;
			else if (lhs_type.is_pointer() && lhs_type.referenced_type().is_void() && rhs_type.is_pointer() && rhs_type.referenced_type().is_object())
				ok = true;
			else if (lhs_type.is_pointer() && rhs_xpr->is_nullpointer_constant())
				ok = true;
		}
		else if (this->get_id() == XprNode::Id::PLUS_ASSIGN || this->get_id() == XprNode::Id::MINUS_ASSIGN)
		{
			if (lhs_type.is_pointer() && lhs_type.referenced_type().is_complete_object() && rhs_type.is_integer())
				ok = true;
			else if (lhs_type.is_arithmetic() && rhs_type.is_arithmetic())
				ok = true;
		}
		else
		{
			throw __FILE__ ": Unimplemented compound assignment type check";
		}

		if (!ok)
		{
			std::cerr << "Assignment conditions not fulfilled" << std::endl;
			return false;
		}

		// TODO this is only valid for assignment, not for compound assignments
		set_subxpr(1, conditional_cast(rhs_xpr, lhs_type));
		set_xpr_type(lhs_type);

		return true;
	}

	virtual XprNode *clone() override
	{
		XprNode *c = new AssignmentXprNode(*this);
		for (auto x : get_subxprs())
			c->add_subxpr(x->clone());
		return c;
	}

};

#endif
