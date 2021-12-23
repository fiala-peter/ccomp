#ifndef COMPOUND_NODE_H_INCLUDED
#define COMPOUND_NODE_H_INCLUDED

#include "statement_node.h"
#include "symbol_table_owner.h"

class CompoundNode
	: public StmNode
	, public SymbolTableOwner
{
public:
	CompoundNode()
		: StmNode(StmNode::Id::BLOCK)
	{
	}

	virtual void print(std::ostream &os, size_t level = 0) const override;

private:
};

#endif
