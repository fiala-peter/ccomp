#ifndef CONDITIONAL_XPR_NODE_H_INCLUDED
#define CONDITIONAL_XPR_NODE_H_INCLUDED

#include "xpr_node.h"

class ConditionalXprNode : public XprNode
{
public:
	ConditionalXprNode();

	bool type_check_impl() override;

	virtual XprNode *clone() override
	{
		XprNode *c = new ConditionalXprNode(*this);
		for (auto x : get_subxprs())
			c->add_subxpr(x->clone());
		return c;
	}
};

#endif
