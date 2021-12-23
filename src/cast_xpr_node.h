#ifndef CAST_XPR_NODE_H_INCLUDED
#define CAST_XPR_NODE_H_INCLUDED

#include "xpr_node.h"
#include "integer_constant.h"

class CastXprNode : public XprNode
{
public:
	CastXprNode()
		: XprNode(XprNode::Id::CAST)
	{
	}

	bool type_check_impl() override;

	virtual XprNode *clone() override
	{
		XprNode *c = new CastXprNode(*this);
		for (size_t i = 0; i < get_num_subxprs(); ++i)
			set_subxpr(i, get_subxpr(i)->clone());
		return c;
	}

	IntegerConstant evaluate_constant() const override
	{
		IntegerConstant ret = get_subxpr(0)->evaluate_constant();
		// TODO this cast should be implemented
		return ret.cast(get_xpr_type());
	}

};

#endif
