#include "cast_xpr_node.h"
#include "xpr_node.h"
#include "integer_constant.h"

XprNode *XprNode::conditional_cast(XprNode *xpr, Type const &type)
{
	if (xpr->get_xpr_type() != type)
	{
		XprNode *tmp = xpr;
		xpr = new CastXprNode();
		xpr->set_xpr_type(type);
		xpr->add_subxpr(tmp);
	}
	return xpr;
}

bool XprNode::is_constant_expression() const
{
	if (get_num_subxprs() == 0)
		return false;
	for (auto xpr : get_subxprs())
		if (!xpr->is_constant_expression())
			return false;
	return true;
}

IntegerConstant XprNode::evaluate_constant() const
{
	throw __FILE__ ": Unimplemented constant operation";
}

XprNode *XprNode::function_to_pointer_cast(XprNode *arg)
{
	return conditional_cast(arg, arg->get_xpr_type().function_to_pointer_cast());
}

XprNode *XprNode::array_to_pointer_cast(XprNode *arg)
{
	return conditional_cast(arg, arg->get_xpr_type().array_to_pointer_cast());
}

void XprNode::print(std::ostream &os, size_t level) const
{
	for (size_t i = 0; i < level; i++)
		os << ((i == level - 1) ? "+---" : "    ");


	struct
	{
		Id id;
		char const *str;
	} data[] = {
		{Id::ADDRESS_OF, "ADDRESS_OF"},
		{Id::AND_ASSIGN, "AND ASSIGN"},
		{Id::ARRAY_SUBSCRIPT, "ARRAY_SUBSCRIPT"},
		{Id::ASSIGN, "ASSIGNMENT"},
		{Id::BINARY_MINUS, "BINARY MINUS"},
		{Id::BINARY_PLUS, "BINARY PLUS"},
		{Id::BITWISE_AND, "BITWISE AND"},
		{Id::BITWISE_NOT, "BITWISE NOT"},
		{Id::BITWISE_OR, "BITWISE OR"},
		{Id::BITWISE_XOR, "BITWISE XOR"},
		{Id::CAST, "CAST"},
		{Id::CONDITIONAL, "CONDITIONAL"},
		{Id::COMMA, "COMMA"},
		{Id::DEREFERENCE, "DEREFERENCE"},
		{Id::DIV_ASSIGN, "DIV ASSIGN"},
		{Id::EQUAL, "EQUAL"},
		{Id::FIELDNAME_XPR, "FIELDNAME"},
		{Id::FUNCTION_CALL, "FUNCTION_CALL"},
		{Id::GREATER, "GREATER"},
		{Id::GREATER_EQUAL, "GE"},
		{Id::IDENTIFIER, "IDENTIFIER"},
		{Id::LESS, "LESS"},
		{Id::LESS_EQUAL, "LE"},
		{Id::LOGICAL_AND, "LOGICAL AND"},
		{Id::LOGICAL_NOT, "LOGICAL NOT"},
		{Id::LOGICAL_OR, "LOGICAL OR"},
		{Id::MINUS_ASSIGN, "MINUS ASSIGN"},
		{Id::MOD, "MOD"},
		{Id::MOD_ASSIGN, "MOD ASSIGN"},
		{Id::NOT_EQUAL, "NOT EQUAL"},
		{Id::OR_ASSIGNMENT, "OR ASSIGN"},
		{Id::PER, "PER"},
		{Id::PLUS_ASSIGN, "PLUS ASSIGN"},
		{Id::PREDECREMENT, "PREDECREMENT"},
		{Id::PREINCREMENT, "PREINCREMENT"},
		{Id::POSTINCREMENT, "POSTINCREMENT"},
		{Id::SHL, "SHL"},
		{Id::SHL_ASSIGN, "SHL ASSIGN"},
		{Id::SHR, "SHR"},
		{Id::SHR_ASSIGN, "SHR ASSIGN"},
		{Id::SIZEOF, "SIZEOF"},
		{Id::STRING_LITERAL, "STRING_LITERAL"},
		{Id::STRUCTURE_PTR_MEMBER, "STRUCTURE PTR MEMBER EXPRESSION"},
		{Id::STRUCTURE_MEMBER, "STRUCTURE MEMBER EXPRESSION"},
		{Id::UNARY_MINUS, "UNARY_MINUS"},
		{Id::UNARY_PLUS, "UNARY PLUS"},
		{Id::TIMES, "TIMES"},
		{Id::TIMES_ASSIGN, "TIMES ASSIGN"},
		{Id::XOR_ASSIGN, "XOR ASSIGN"}};

	for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); ++i)
	{
		if (m_opid == data[i].id)
		{
			os << data[i].str;
			break;
		}
	}

	os << " Type: " << get_xpr_type() << "\n";

	for (auto c : get_subxprs())
		c->print(os, level + 1);
}

bool XprNode::is_modifiable_lvalue() const
{
	Type const &t = get_xpr_type();
	if (t.is_array() || t.is_function())
		return false;
	Id id = get_id();
	return id == Id::IDENTIFIER || id == Id::DEREFERENCE || id == Id::ARRAY_SUBSCRIPT || id == Id::STRUCTURE_PTR_MEMBER || id == Id::STRUCTURE_MEMBER;
}

bool XprNode::type_check()
{
	if (m_opid != Id::SIZEOF && m_opid != Id::ADDRESS_OF)
	{
		for (size_t i = 0; i < get_num_subxprs(); ++i)
		{
			if (get_subxpr(i)->get_id() == Id::FIELDNAME_XPR)
				continue;
			set_subxpr(i, function_to_pointer_cast(get_subxpr(i)));
			set_subxpr(i, array_to_pointer_cast(get_subxpr(i)));
		}
	}

	return type_check_impl();
}

bool XprNode::is_nullpointer_constant() const
{
	return is_constant_expression() && (size_t)evaluate_constant() == 0;
}
