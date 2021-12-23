#ifndef IDENTIFIER_XPR_NODE_H_INCLUDED
#define IDENTIFIER_XPR_NODE_H_INCLUDED

#include "xpr_node.h"

#include <string>

class IdentifierXprNode : public XprNode
{
public:
	IdentifierXprNode(XprNode::Id id, std::string const &identifier = "") : XprNode(id), m_identifier(identifier) { }

	bool type_check_impl() override { return true;  }

	XprNode *clone() override { return new IdentifierXprNode(*this); }

	void set_identifier(std::string const identifier) { m_identifier = identifier; }

	std::string const &get_identifier() const { return m_identifier; }

	void print(std::ostream &os, size_t level = 0) const override
	{
		for (size_t i = 0; i < level; i++)
			os << "    ";
		os << m_identifier << ": " << get_xpr_type() << std::endl;
	}

private:
	std::string m_identifier;
};

#endif
