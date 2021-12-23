#ifndef UNARY_XPR_NODE_H_INCLUDED
#define UNARY_XPR_NODE_H_INCLUDED

#include "xpr_node.h"

class UnaryXprNode : public XprNode
{
public:
	UnaryXprNode(XprNode::Id op)
		: XprNode(op)
	{
	}

	bool type_check_impl() override;

	XprNode *clone() override
	{
		XprNode *c = new UnaryXprNode(*this);
		for (size_t i = 0; i < get_num_subxprs(); ++i)
			c->set_subxpr(i, get_subxpr(i)->clone());
		return c;
	}
};

#endif
