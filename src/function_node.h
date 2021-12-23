#ifndef FUNCTION_NODE_H_INCLUDED
#define FUNCTION_NODE_H_INCLUDED

#include "type.h"
#include "compound_node.h"
#include "serializable.h"
#include "statement_node.h"
#include "symbol_table_owner.h"

#include <utility>
#include <memory>

class FunctionNode
	: public Serializable
	, public SymbolTableOwner
{
public:
	FunctionNode()
		: m_block(nullptr)
	{
	}

	void print(std::ostream &os, size_t level = 0) const override;

	void constant_fold() { m_block->constant_fold(); }

	Type const &get_return_type() const { return m_return_type; }

	void set_return_type(Type const &t) { m_return_type = t; }

	std::string const &get_identifier() const { return m_identifier; }

	void set_identifier(std::string const &id) { m_identifier = id; }

	CompoundNode const &get_block() const { return *m_block; }

	void set_block(std::shared_ptr<CompoundNode> &&block) { m_block = std::forward<std::shared_ptr<CompoundNode>>(block); }

	void set_block(std::shared_ptr<CompoundNode> const &block) { m_block = block; }

private:
	Type m_return_type;
	std::string m_identifier;
	std::shared_ptr<CompoundNode> m_block;
};

#endif
