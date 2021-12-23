#ifndef POSTFIX_XPR_NODE_H_INCLUDED
#define POSTFIX_XPR_NODE_H_INCLUDED

#include "xpr_node.h"

class PostfixXprNode : public XprNode
{
public:
	PostfixXprNode(XprNode::Id op)
		: XprNode(op)
	{
	}

	bool type_check_impl() override;

	XprNode *clone() override
	{
		PostfixXprNode *c = new PostfixXprNode(*this);
		for (size_t i = 0; i < get_num_subxprs(); ++i)
			c->set_subxpr(i, get_subxpr(i)->clone());
		return c;
	}
};

#endif
