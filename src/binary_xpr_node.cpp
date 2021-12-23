#include "binary_xpr_node.h"

#include "integer_constant.h"

int BinaryXprNode::precedence(XprNode::Id op)
{
	switch (op)
	{
	case XprNode::Id::MOD:
	case XprNode::Id::PER:
	case XprNode::Id::TIMES:
		return 3;
	case XprNode::Id::BINARY_PLUS:
	case XprNode::Id::BINARY_MINUS:
		return 4;
	case XprNode::Id::SHL:
	case XprNode::Id::SHR:
		return 5;
	case XprNode::Id::LESS:
	case XprNode::Id::LESS_EQUAL:
	case XprNode::Id::GREATER:
	case XprNode::Id::GREATER_EQUAL:
		return 6;
	case XprNode::Id::EQUAL:
	case XprNode::Id::NOT_EQUAL:
		return 7;
	case XprNode::Id::BITWISE_AND:
		return 8;
	case XprNode::Id::BITWISE_XOR:
		return 9;
	case XprNode::Id::BITWISE_OR:
		return 10;
	case XprNode::Id::LOGICAL_AND:
		return 11;
	case XprNode::Id::LOGICAL_OR:
		return 12;
	case XprNode::Id::CONDITIONAL:
		return 13;
	case XprNode::Id::ASSIGN:
		return 14;
	case XprNode::Id::COMMA:
		return 15;
	}
	throw __FILE__ ": Undefined precedence of some operator";
}

XprNode::Id BinaryXprNode::token_to_xpr(Token::Id tt)
{
	switch (tt)
	{
	case Token::Id::ASTERISK:
		return XprNode::Id::TIMES;
	case Token::Id::PER:
		return XprNode::Id::PER;
	case Token::Id::MOD:
		return XprNode::Id::MOD;
	case Token::Id::PLUS:
		return XprNode::Id::BINARY_PLUS;
	case Token::Id::MINUS:
		return XprNode::Id::BINARY_MINUS;
	case Token::Id::SHR:
		return XprNode::Id::SHR;
	case Token::Id::SHL:
		return XprNode::Id::SHL;
	case Token::Id::LESS:
		return XprNode::Id::LESS;
	case Token::Id::LESS_EQUAL:
		return XprNode::Id::LESS_EQUAL;
	case Token::Id::GREATER:
		return XprNode::Id::GREATER;
	case Token::Id::GREATER_EQUAL:
		return XprNode::Id::GREATER_EQUAL;
	case Token::Id::EQ:
		return XprNode::Id::EQUAL;
	case Token::Id::NEQ:
		return XprNode::Id::NOT_EQUAL;
	case Token::Id::AMPERSAND:
		return XprNode::Id::BITWISE_AND;
	case Token::Id::BITWISE_XOR:
		return XprNode::Id::BITWISE_XOR;
	case Token::Id::BITWISE_OR:
		return XprNode::Id::BITWISE_OR;
	case Token::Id::LOGICAL_AND:
		return XprNode::Id::LOGICAL_AND;
	case Token::Id::LOGICAL_OR:
		return XprNode::Id::LOGICAL_OR;
	case Token::Id::COMMA:
		return XprNode::Id::COMMA;
	case Token::Id::ASSIGN:
		return XprNode::Id::ASSIGN;
	}
	throw __FILE__ ": Undefined syntax tree element";
}

bool BinaryXprNode::type_check_impl()
{
	Type l = lhs()->get_xpr_type(), r = rhs()->get_xpr_type(), ret;
	if (m_op == nullptr)
		throw __FILE__ ": binary operator unimplemented.";
	if (!m_op->type_check(&l, &r, &ret))
	{
		std::cerr << "Failed to type check binary operator" << std::endl;
		return false;
	}
	set_subxpr(0, conditional_cast(lhs(), l));
	set_subxpr(1, conditional_cast(rhs(), r));
	set_xpr_type(ret);
	return true;
}

void BinaryXprNode::set_op()
{
	switch (get_id())
	{
	case XprNode::Id::BINARY_PLUS:
		m_op = std::make_unique<BinaryPlusOperator>();
		break;
	case XprNode::Id::BINARY_MINUS:
		m_op = std::make_unique<BinaryMinusOperator>();
		break;
	case XprNode::Id::TIMES:
		m_op =std::make_unique<BinaryTimesOperator>();
		break;
	case XprNode::Id::PER:
		m_op = std::make_unique<BinaryDivOperator>();
		break;
	case XprNode::Id::MOD:
		m_op = std::make_unique<BinaryModOperator>();
		break;
	case XprNode::Id::EQUAL:
		m_op = std::make_unique<BinaryEqOperator>();
		break;
	case XprNode::Id::NOT_EQUAL:
		m_op = std::make_unique<BinaryNeqOperator>();
		break;
	case XprNode::Id::LESS:
		m_op = std::make_unique<BinaryLessOperator>();
		break;
	case XprNode::Id::LESS_EQUAL:
		m_op = std::make_unique<BinaryLessEqualOperator>();
		break;
	case XprNode::Id::GREATER:
		m_op = std::make_unique<BinaryGreaterOperator>();
		break;
	case XprNode::Id::GREATER_EQUAL:
		m_op = std::make_unique<BinaryGreaterEqualOperator>();
		break;
	case XprNode::Id::LOGICAL_AND:
		m_op = std::make_unique<BinaryLogicalAndOperator>();
		break;
	case XprNode::Id::LOGICAL_OR:
		m_op = std::make_unique<BinaryLogicalOrOperator>();
		break;
	case XprNode::Id::COMMA:
		m_op = std::make_unique<BinaryCommaOperator>();
		break;
		throw __FILE__ ": Unimplemented binary operator";
	}
}

XprNode *BinaryXprNode::clone()
{
	BinaryXprNode *c = new BinaryXprNode(get_id());
	c->set_op();
	for (size_t i = 0; i < get_num_subxprs(); ++i)
		c->set_subxpr(i, get_subxpr(i)->clone());
	return c;
}
