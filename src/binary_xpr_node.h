/**
 * @file binary_xpr_node.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class BinaryXprNode
 */
#ifndef BINARY_XPR_NODE_H_INCLUDED
#define BINARY_XPR_NODE_H_INCLUDED

#include "ast_node.h"
#include "operator.h"
#include "token.h"
#include "xpr_node.h"

#include <utility>

/** @brief class representing a binary expression */
class BinaryXprNode
	: public XprNode
{
public:
	/** @brief constructor */
	BinaryXprNode(XprNode::Id op)
		: XprNode(op), m_op(nullptr)
	{
		set_op();
	}

	/** @brief helper function to return the left hand side expression */
	XprNode *lhs() const { return get_subxpr(0); }

	/** @brief helper function to return the right hand side expression */
	XprNode *rhs() const { return get_subxpr(1); }

	/** @brief helper function to return the left hand type */
	Type const &ltype() const { return lhs()->get_xpr_type(); }

	/** @brief helper function to return the right hand type */
	Type const &rtype() const { return rhs()->get_xpr_type(); }

	/** @brief convert a C token to a binary expression id */
	static XprNode::Id token_to_xpr(Token::Id tt);

	/** @brief return the precedence of a binary operator */
	static int precedence(XprNode::Id op);

	/** @brief set the operator based on the expression type */
	void set_op();

	bool type_check_impl() override;

	IntegerConstant evaluate_constant() const override
	{
		return (*m_op)(lhs()->evaluate_constant(), rhs()->evaluate_constant());
	}

	virtual XprNode *clone() override;

private:
	std::unique_ptr<BinaryOperator> m_op;
};

#endif
