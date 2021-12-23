#include "ast_node.h"
#include "statement_node.h"
#include "integer_constant.h"
#include "xpr_node.h"

AstNode::~AstNode()
{
	for (auto c : m_subxprs)
		delete c;
	// substatements do not need to be deleted as they are contained via shared pointers
}

void AstNode::set_subxpr(size_t i, XprNode *c)
{
	m_subxprs[i] = c;
	if (c != nullptr)
		c->m_parent = this;
}

void AstNode::add_subxpr(XprNode *c)
{
	m_subxprs.push_back(c);
	if (c != nullptr)
		c->m_parent = this;
}

void AstNode::add_substm(std::shared_ptr<StmNode> const &c)
{
	m_substms.push_back(c);
	if (c != nullptr)
		c->m_parent = this;
}

void AstNode::constant_fold()
{
	for (auto s : get_substms())
		s->constant_fold();

	for (auto s : get_subxprs())
		s->constant_fold();

	for (size_t i = 0; i < get_num_subxprs(); ++i)
	{
		if (!get_subxpr(i)->is_constant_expression())
			continue;
		if (get_subxpr(i)->get_id() == XprNode::Id::INTEGER_XPR)
		{
			IntegerConstant *ixpr = new IntegerConstant(get_subxpr(i)->evaluate_constant());
			delete get_subxpr(i);
			set_subxpr(i, ixpr);
		}
	}
}
