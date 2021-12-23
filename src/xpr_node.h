/**
 * @file xpr_node.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of XprNode : the base class of all expressions
 * @version 0.1
 * @date 2021-01-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef XPR_NODE_H_INCLUDED
#define XPR_NODE_H_INCLUDED

#include "ast_node.h"

class IntegerConstant;

/** @brief base class of all expressions */
class XprNode
 : public AstNode
{
public:

	/** @brief enumeration of expression identifiers */
	enum class Id
	{
		ADDRESS_OF,
		AND_ASSIGN,
		ARRAY_SUBSCRIPT,
		ASSIGN,
		BINARY_MINUS,
		BINARY_PLUS,
		BITWISE_AND,
		BITWISE_NOT,
		BITWISE_OR,
		BITWISE_XOR,
		CAST,
		COMMA,
		CONDITIONAL,
		DEREFERENCE,
		DIV_ASSIGN,
		EQUAL,
		FIELDNAME_XPR,
		FLOATING_XPR,
		FUNCTION_CALL,
		GREATER,
		GREATER_EQUAL,
		IDENTIFIER,
		INTEGER_XPR,
		LESS,
		LESS_EQUAL,
		LOGICAL_AND,
		LOGICAL_NOT,
		LOGICAL_OR,
		MINUS_ASSIGN,
		MOD_ASSIGN,
		MOD,
		NOT_EQUAL,
		OR_ASSIGNMENT,
		PER,
		POSTDECREMENT,
		POSTINCREMENT,
		PREDECREMENT,
		PREINCREMENT,
		PLUS_ASSIGN,
		SHL,
		SHR,
		SHL_ASSIGN,
		SHR_ASSIGN,
		SIZEOF,
		STRING_LITERAL,
		STRUCTURE_MEMBER,
		STRUCTURE_PTR_MEMBER,
		UNARY_PLUS,
		UNARY_MINUS,
		TIMES,
		TIMES_ASSIGN,
		XOR_ASSIGN,
	};

	XprNode(Id op) : m_opid(op) {}

	/** @brief displays the expression in an output stream */
	virtual void print(std::ostream &os, size_t level = 0) const override;

	/** @brief determines if the expression is constant expression or not */
	virtual bool is_constant_expression() const;

	/** @brief determines if the expression is a null pointer constant or not */
	bool is_nullpointer_constant() const;

	/** @brief returns the value of the expression if it is constant */
	virtual IntegerConstant evaluate_constant() const;

	/** @brief converts the expression to a new type by including a cast expression */
	static XprNode *conditional_cast(XprNode *xpr, Type const &t);

	/** @brief converts function designators to function pointers */
	static XprNode *function_to_pointer_cast(XprNode *arg);

	/** @brief converts array expressions to pointers */
	static XprNode *array_to_pointer_cast(XprNode *arg);

	void set_xpr_type(Type const &t) { m_xpr_type = t; }

	Type const &get_xpr_type() const { return m_xpr_type; }

	/** @brief determines if the expression is a modifiable lvalue */
	bool is_modifiable_lvalue() const;

	/** @brief return the expression's type identifier */
	Id get_id() const { return m_opid; }

	virtual bool type_check_impl()
	{
		throw __FILE__ ": Type check unimplemented";
	}

	/** @brief perform type checking of arguments and set expression result type */
	bool type_check();

	/** @brief deep-copy the expression */
	virtual XprNode *clone()
	{
		XprNode *c = new XprNode(*this);
		for (size_t i = 0; i < get_num_subxprs(); ++i)
			c->set_subxpr(i, get_subxpr(i)->clone());
		return c;
	}

private:
	Id m_opid;
	Type m_xpr_type;
};

#endif
