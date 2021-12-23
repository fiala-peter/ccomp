#ifndef STRING_LITERAL_NODE_H_INCLUDED
#define STRING_LITERAL_NODE_H_INCLUDED

#include "xpr_node.h"

#include <string>

class StringLiteralNode : public XprNode
{
public:
	StringLiteralNode()
		: XprNode(XprNode::Id::STRING_LITERAL)
	{
		set_xpr_type(Type::char_type().pointer_to());
	}

	StringLiteralNode(std::string const &str)
		: XprNode(XprNode::Id::STRING_LITERAL)
		, m_string(str)
	{
		set_xpr_type(Type::char_type().pointer_to());
	}

	bool type_check_impl() override
	{
		return true;
	}

	virtual XprNode *clone() override
	{
		return new StringLiteralNode(*this);
	}

	void set_string(std::string const str) { m_string = str; }

	std::string const &get_string() const { return m_string; }

private:
	std::string m_string;
};

#endif
