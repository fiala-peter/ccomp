#ifndef STATEMENT_NODE_H_INCLUDED
#define STATEMENT_NODE_H_INCLUDED

#include "ast_node.h"

class StmNode
	: public AstNode
{
public:
	enum class Id
	{
		BLOCK,
		BREAK,
		CONTINUE,
		DO,
		EMPTY,
		FOR,
		IF,
		RETURN,
		XPR,
		WHILE
	};

	StmNode(Id id) : m_id(id) {}

	Id get_id() const { return m_id; }

	void print(std::ostream &os, size_t level = 0) const override;

private:
	Id m_id;
};

#endif
