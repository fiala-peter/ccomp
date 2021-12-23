#include "statement_node.h"

#include "xpr_node.h"

void StmNode::print(std::ostream &os, size_t level) const
{
	for (size_t i = 0; i < level; i++)
		os << "    ";

	switch (m_id)
	{
	case Id::XPR:
		os << "EXPRESSION";
		break;
	case Id::WHILE:
		os << "WHILE";
		break;
	case Id::DO:
		os << "DO";
		break;
	case Id::FOR:
		os << "FOR";
		break;
	case Id::IF:
		os << "IF";
		break;
	case Id::EMPTY:
		os << "EMPTY";
		break;
	case Id::RETURN:
		os << "RETURN";
		break;
	case Id::CONTINUE:
		os << "CONTINUE";
		break;
	case Id::BREAK:
		os << "BREAK";
		break;
	default:
		throw __FILE__ ": Unhandled AST value for printing";
	}

	os << " STATEMENT\n";
	for (auto c : get_subxprs())
	{
		if (c == nullptr)
			throw __FILE__ ": Null subexpression found";
		c->print(os, level + 1);
	}
	for (auto c : get_substms())
		c->print(os, level + 1);
}
