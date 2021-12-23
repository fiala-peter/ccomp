/**
 * @file ast_node.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class AstNode
 * @version 0.1
 * @date 2021-01-25
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "type.h"
#include "serializable.h"
#include "symbol_tree.h"

#include <iostream>
#include <vector>
#include <memory> // std::shared_ptr

/**
 * @brief Forward declaration of class XprNode, as each node contains pointers to subexpressions
 */
class XprNode;

/**
 * @brief Forward declaration of class StmNode as each node contains pointers to substatements
 */
class StmNode;

/**
 * @brief This is the basic node type of the Abstract Syntax Tree. The node supports linking
 * to children and parent, and defines interfaces for constant folding optimization.
 */
class AstNode
	: public Serializable
{
public:
	using subexpression_container_t = std::vector<XprNode *>;
	using substatement_container_t = std::vector<std::shared_ptr<StmNode>>;

	/** @brief destructor providing heterogeneous data structure */
	virtual ~AstNode();

	/** @brief add a new subexpression to the node */
	void add_subxpr(XprNode *c);

	/** @brief add a new substatement to the node */
	void add_substm(std::shared_ptr<StmNode> const &s);

	/** @brief replace the i-th subexpression */
	void set_subxpr(size_t i, XprNode *c);

	/** @brief return the number of subexpressions */
	size_t get_num_subxprs() const { return m_subxprs.size(); }

	/** @brief return the number of substatements */
	size_t get_num_substms() const { return m_substms.size(); }

	/** @brief return the container of subexpressions */
	subexpression_container_t const &get_subxprs() const { return m_subxprs; }

	/** @brief return the container of substatements */
	substatement_container_t const &get_substms() const { return m_substms; }

	/** @brief return the i-th subexpression */
	XprNode *get_subxpr(size_t i) const { return m_subxprs[i]; }

	/** @brief return the i-th substatement */
	StmNode const &get_substm(size_t i) const { return *m_substms[i]; }

	/** @brief perform constant folding optimization */
	virtual void constant_fold();

private:
	subexpression_container_t m_subxprs;
	substatement_container_t m_substms;
	AstNode *m_parent;
};

#endif
