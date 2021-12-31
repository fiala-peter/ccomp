#include "parser.h"

#include "assignment_xpr_node.h"
#include "binary_xpr_node.h"
#include "cast_xpr_node.h"
#include "conditional_xpr_node.h"
#include "compound_node.h"
#include "floating_constant.h"
#include "function_node.h"
#include "identifier_xpr_node.h"
#include "integer_constant.h"
#include "postfix_xpr_node.h"
#include "string_literal_node.h"
#include "translation_unit.h"
#include "unary_xpr_node.h"

#include <algorithm>
#include <sstream>
#include <string>

void Parser::error_message(std::string const &str)
{
	Coordinate const &c = m_current_token->get_coordinate();
	std::cerr << "Parsing error at " << c << ".\t";
	std::cerr << str << std::endl;
}

XprNode *Parser::parse_integer_constant()
{
	// TODO this should be generalized
	if (m_current_token->get_id() != Token::Id::INTEGER_CONSTANT)
	{
		error_message("Error parsing integer constant.");
		return nullptr;
	}
	int v = m_current_token->get_int_constant();
	IntegerConstant *st = new IntegerConstant(Type::int_type(), &v);

	next_token();

	return st;
}

XprNode *Parser::parse_character_constant()
{
	// TODO this should be generalized
	if (m_current_token->get_id() != Token::Id::CHARACTER_CONSTANT)
	{
		error_message("Error parsing character constant.");
		return nullptr;
	}
	std::string str = m_current_token->get_string();
	str = std::string(str.begin() + 1, str.end() - 1);
	int v;
	if (str[0] == '\\')
	{
		if (str == "\\'")
			v = '\'';
		else if (str == "\\\"")
			v = '\"';
		else if (str == "\\?")
			v = '\?';
		else if (str == "\\\\")
			v = '\\';
		else if (str == "\\a")
			v = '\a';
		else if (str == "\\b")
			v = '\b';
		else if (str == "\\f")
			v = '\f';
		else if (str == "\\n")
			v = '\n';
		else if (str == "\\r")
			v = '\r';
		else if (str == "\\t")
			v = '\t';
		else if (str == "\\v")
			v = '\v';
		else if (str[1] == 'x') // hexadecimal
		{
			v = 0;
			for (auto it = str.begin() + 1; it != str.end(); it++)
				v = 16 * v + (*it - '0');
		}
		else // octal
		{
			v = 0;
			for (auto it = str.begin() + 1; it != str.end(); it++)
				v = 8 * v + (*it - '0');
		}
	}
	else
		v = str[0];
	IntegerConstant *st = new IntegerConstant(Type::int_type(), &v);

	next_token();

	return st;
}

XprNode *Parser::parse_floating_constant()
{
	// TODO this should be generalized
	if (m_current_token->get_id() != Token::Id::FLOATING_CONSTANT)
	{
		error_message("Error parsing floating constant.");
		return nullptr;
	}
	FloatingConstant *st = new FloatingConstant;
	st->set_xpr_type(Type::double_type());
	st->set_value(m_current_token->get_double_constant());
	get_translation_unit()->add_floating_constant(m_current_token->get_double_constant());
	next_token();

	return st;
}

XprNode *Parser::parse_string_literal()
{
	if (m_current_token->get_id() != Token::Id::STRING_LITERAL)
	{
		error_message("Error parsing string literal");
		return nullptr;
	}
	std::string const &str = m_current_token->get_string();
	StringLiteralNode *st = new StringLiteralNode(str);
	get_translation_unit()->add_string_literal(str);
	next_token();
	return st;
}

XprNode *Parser::parse_identifier(bool type_check_needed)
{
	if (m_current_token->get_id() != Token::Id::IDENTIFIER)
	{
		error_message("Error parsing identifier");
		return nullptr;
	}
	std::string const &id = m_current_token->get_string();
	next_token();

	if (type_check_needed)
	{
		int const *pc = m_st_ptr->lookup_constant_global(id);
		if (pc != nullptr)
			return new IntegerConstant(Type::int_type(), pc);
		auto ps = m_st_ptr->lookup_global(id);
		if (ps != nullptr && (ps->is_object() || ps->is_function()))
		{
			IdentifierXprNode *st = new IdentifierXprNode(XprNode::Id::IDENTIFIER);
			st->set_identifier(id);
			st->set_xpr_type(ps->get_type());
			return st;
		}
		error_message("Undeclared identifier: " + id);
		return nullptr;
	}
	else
	{
		IdentifierXprNode *st = new IdentifierXprNode(XprNode::Id::IDENTIFIER);
		st->set_identifier(id);
		return st;
	}
}

bool Parser::is_type_specifier(Token const *t) const
{
	auto tt = t->get_id();
	if (tt == Token::Id::VOID || tt == Token::Id::CHAR || tt == Token::Id::SHORT || tt == Token::Id::INT || tt == Token::Id::LONG || tt == Token::Id::FLOAT || tt == Token::Id::DOUBLE || tt == Token::Id::SIGNED || tt == Token::Id::UNSIGNED || tt == Token::Id::STRUCT || tt == Token::Id::UNION || tt == Token::Id::ENUM)
		return true;
	if (tt == Token::Id::IDENTIFIER)
	{
		auto pste = m_st_ptr->lookup_global(t->get_string());
		if (pste != nullptr && pste->get_category() == SymbolTableEntry::TYPE)
			return true;
	}
	return false;
}

bool Parser::is_type_name(Token const *t) const
{
	return t->is_type_qualifier() || is_type_specifier(t);
}

bool Parser::parse_type_name(Type *type)
{
	Declaration decl;
	Type t;
	if (!parse_declaration_specifiers(&decl) || decl.is_typedef())
	{
		error_message("Error parsing declaration specifiers in type name. ");
		return false;
	}

	// process comma separated declarators
	parse_declarator(&decl);

	if (!decl.is_abstract())
	{
		error_message("Error parsing typename. The declarator part should be abstract.");
		return false;
	}

	*type = decl.get_type();
	return true;
}

XprNode *Parser::parse_fieldname()
{
	if (m_current_token->get_id() != Token::Id::IDENTIFIER)
		return nullptr;
	IdentifierXprNode *st = new IdentifierXprNode(XprNode::Id::FIELDNAME_XPR);
	st->set_identifier(m_current_token->get_string());
	next_token();

	return st;
}

XprNode *Parser::parse_primary_expression()
{
	/*
	primary-expression:
		identifier
		constant
		string-literal
		( expression )

	constant:
		floating-constant
		integer-constant
		enumeration-constant
		character-constant

	enumeration-constant
		identifier
	*/
	if (m_current_token->get_id() == Token::Id::IDENTIFIER)
		return parse_identifier(true);

	if (m_current_token->get_id() == Token::Id::INTEGER_CONSTANT)
		return parse_integer_constant();

	if (m_current_token->get_id() == Token::Id::CHARACTER_CONSTANT)
		return parse_character_constant();

	if (m_current_token->get_id() == Token::Id::FLOATING_CONSTANT)
		return parse_floating_constant();

	if (m_current_token->get_id() == Token::Id::STRING_LITERAL)
		return parse_string_literal();

	if (!expect(Token::Id::PARENTHESES_OPEN))
		return nullptr;

	XprNode *st = parse_expression();

	if (!expect(Token::Id::PARENTHESES_CLOSE))
	{
		delete st;
		return nullptr;
	}

	return st;
}

XprNode *Parser::parse_postfix_expression()
{
	/*
	postfix-expression:
		primary-expression
		postfix-expression [ expression ]
		postfix-expression ( argument-expression-list_opt )
		postfix-expression . identifier
		postfix-expression -> identifier
		postfix-expression ++
		postfix-expression --

	argument-expression-list:
		assignment-expression
		argument-expression-list , assignment-expression
	*/

	XprNode *ret = parse_primary_expression();
	if (ret == nullptr)
	{
		error_message("Error parsing primary expression");
		return nullptr;
	}

	while (true)
	{
		Token::Id token_id = m_current_token->get_id();
		if (token_id == Token::Id::INCREMENT || token_id == Token::Id::DECREMENT)
		{
			next_token();
			PostfixXprNode *n = new PostfixXprNode(token_id == Token::Id::INCREMENT ? XprNode::Id::POSTINCREMENT : XprNode::Id::POSTDECREMENT);
			n->add_subxpr(ret);
			if (!n->type_check())
			{
				error_message("Error type checking postincrement expression");
				delete n;
				return nullptr;
			}
			ret = n;
		}
		else if (token_id == Token::Id::BRACKET_OPEN)
		{
			expect(Token::Id::BRACKET_OPEN);
			XprNode *idx_xpr = parse_expression();
			if (idx_xpr == nullptr)
			{
				error_message("Error parsing index expression of array subscript");
				delete ret;
				return nullptr;
			}
			if (!expect(Token::Id::BRACKET_CLOSE))
			{
				delete idx_xpr;
				delete ret;
				return nullptr;
			}
			PostfixXprNode *n = new PostfixXprNode(XprNode::Id::ARRAY_SUBSCRIPT);
			n->add_subxpr(ret);
			n->add_subxpr(idx_xpr);
			if (!n->type_check())
			{
				error_message("Error while parsing array subscript expression");
				delete n;
				return nullptr;
			}
			ret = n;
		}
		else if (token_id == Token::Id::PARENTHESES_OPEN)
		{
			expect(Token::Id::PARENTHESES_OPEN);
			PostfixXprNode *function = new PostfixXprNode(XprNode::Id::FUNCTION_CALL);
			function->add_subxpr(ret);
			bool was_comma = false;
			while (was_comma || m_current_token->get_id() != Token::Id::PARENTHESES_CLOSE)
			{
				XprNode *x = parse_assignment_expression();
				if (x == nullptr)
				{
					error_message("Error parsing function argument expression");
					delete function;
					return nullptr;
				}
				function->add_subxpr(x);

				if (m_current_token->get_id() == Token::Id::COMMA)
				{
					was_comma = true;
					next_token();
				}
				else
					was_comma = false;
			}
			expect(Token::Id::PARENTHESES_CLOSE); // consume ")"
			if (!function->type_check())
			{
				error_message("Error type checking function call expression");
				delete function;
				return nullptr;
			}
			ret = function;
		}
		else if (token_id == Token::Id::POINT || token_id == Token::Id::ARROW)
		{
			next_token();
			XprNode *fld = parse_fieldname();
			PostfixXprNode *n = new PostfixXprNode(token_id == Token::Id::POINT ? XprNode::Id::STRUCTURE_MEMBER : XprNode::Id::STRUCTURE_PTR_MEMBER);
			n->add_subxpr(ret);
			n->add_subxpr(fld);
			if (!n->type_check())
			{
				error_message("Error type checking structure member or pointer to member expression");
				delete n;
				return nullptr;
			}
			ret = n;
		}
		else
			break;
	}

	return ret;
}

XprNode *Parser::parse_unary_expression()
{
	/*
		unary-expression:
			postfix-expression
			++ unary-expression
			-- unary-expression
			unary-operator cast-expression
			sizeof unary-expression
			sizeof ( type-name )
	*/

	struct
	{
		Token::Id tt;
		XprNode::Id st;
	} data1[] = {
		{Token::Id::INCREMENT, XprNode::Id::PREINCREMENT},
		{Token::Id::DECREMENT, XprNode::Id::PREDECREMENT},
		{Token::Id::NO_TOKEN}};

	for (size_t i = 0; data1[i].tt != Token::Id::NO_TOKEN; i++)
	{
		if (data1[i].tt == m_current_token->get_id())
		{
			next_token();
			XprNode *c = parse_unary_expression();
			if (c == nullptr)
			{
				error_message("Error parsing unary expression");
				return nullptr;
			}
			UnaryXprNode *st = new UnaryXprNode(data1[i].st);
			st->add_subxpr(c);
			if (!st->type_check())
			{
				error_message("Error type checking unary expression");
				return nullptr;
			}
			return st;
		}
	}

	struct
	{
		Token::Id tt;
		XprNode::Id st;
	} data[] = {
		{Token::Id::INCREMENT, XprNode::Id::PREINCREMENT},
		{Token::Id::DECREMENT, XprNode::Id::PREDECREMENT},
		{Token::Id::PLUS, XprNode::Id::UNARY_PLUS},
		{Token::Id::MINUS, XprNode::Id::UNARY_MINUS},
		{Token::Id::LOGICAL_NOT, XprNode::Id::LOGICAL_NOT},
		{Token::Id::BITWISE_NOT, XprNode::Id::BITWISE_NOT},
		{Token::Id::ASTERISK, XprNode::Id::DEREFERENCE},
		{Token::Id::AMPERSAND, XprNode::Id::ADDRESS_OF},
		{Token::Id::NO_TOKEN}};

	for (size_t i = 0; data[i].tt != Token::Id::NO_TOKEN; i++)
	{
		if (data[i].tt == m_current_token->get_id())
		{
			next_token();
			XprNode *c = parse_cast_expression();
			if (c == nullptr)
			{
				error_message("Error parsing cast expression");
				return nullptr;
			}
			UnaryXprNode *st = new UnaryXprNode(data[i].st);
			st->add_subxpr(c);
			if (!st->type_check())
			{
				error_message("Error type checking unary expression");
				return nullptr;
			}
			return st;
		}
	}

	if (m_current_token->get_id() == Token::Id::SIZEOF)
	{
		size_t val;
		next_token();
		if (m_current_token->get_id() == Token::Id::PARENTHESES_OPEN)
		{
			next_token();

			if (is_type_name(m_current_token)) // sizeof ( typename )
			{
				Type t;
				parse_type_name(&t);
				val = t.get_size_in_bytes();
			}
			else // sizeof ( expression )
			{
				XprNode *c = parse_expression();
				if (c == nullptr)
				{
					error_message("Error parsing ( expression )");
					return nullptr;
				}
				XprNode *a = new UnaryXprNode(XprNode::Id::SIZEOF);
				a->add_subxpr(c);

				if (!a->type_check())
				{
					error_message("Error type checking sizeof expression");
					delete a;
					return nullptr;
				}

				val = c->get_xpr_type().get_size_in_bytes();
				delete a;
			}

			if (!expect(Token::Id::PARENTHESES_CLOSE))
			{
				error_message("Error parsing sizeof ( typename/expression ) ");
				return nullptr;
			}
		}
		else // sizeof unary_expression
		{
			XprNode *c = parse_unary_expression();
			if (c == nullptr)
			{
				error_message("Error parsing unary expression");
				return nullptr;
			}
			XprNode *a = new UnaryXprNode(XprNode::Id::SIZEOF);
			a->add_subxpr(c);

			if (!a->type_check())
			{
				error_message("Error type checking sizeof expression");
				delete a;
				return nullptr;
			}

			val = c->get_xpr_type().get_size_in_bytes();
			delete a;
		}

		// todo return type (size_t) should be selected properly
		return new IntegerConstant(Type::ullong_type(), &val);
	}

	return parse_postfix_expression();
}

XprNode::Id Parser::syntax_token(Token::Id tt)
{
	switch (tt)
	{
	case Token::Id::COMMA:
		return XprNode::Id::COMMA;
	case Token::Id::ASSIGN:
		return XprNode::Id::ASSIGN;
	case Token::Id::NEQ:
		return XprNode::Id::NOT_EQUAL;
	case Token::Id::MOD:
		return XprNode::Id::MOD;
	case Token::Id::ASTERISK:
		return XprNode::Id::TIMES;
	case Token::Id::PER:
		return XprNode::Id::PER;
	case Token::Id::PLUS:
		return XprNode::Id::BINARY_PLUS;
	case Token::Id::MINUS:
		return XprNode::Id::BINARY_MINUS;
	}
	throw "Undefined syntax tree element";
}

XprNode *Parser::parse_conditional_expression()
{
	XprNode *cnd = parse_binary_xpr(BinaryXprNode::precedence(BinaryXprNode::Id::LOGICAL_OR));
	if (cnd == nullptr)
	{
		error_message("Error parsing logical OR expression");
		return nullptr;
	}

	if (m_current_token->get_id() != Token::Id::CONDITIONAL)
		return cnd;
	next_token();

	XprNode *xprt = parse_expression();
	if (xprt == nullptr)
	{
		delete cnd;
		return nullptr;
	}

	if (!expect(Token::Id::COLON))
	{
		delete cnd;
		delete xprt;
		return nullptr;
	}

	XprNode *xprf = parse_conditional_expression();
	if (xprf == nullptr)
	{
		delete cnd;
		delete xprt;
		return nullptr;
	}

	ConditionalXprNode *res = new ConditionalXprNode;
	res->add_subxpr(cnd);
	res->add_subxpr(xprt);
	res->add_subxpr(xprf);
	if (!res->type_check())
	{
		error_message("Error type checking conditional expression");
		delete res;
		return nullptr;
	}
	return res;
}

XprNode *Parser::parse_assignment_expression()
{
	XprNode *lhs = parse_conditional_expression();
	if (lhs == nullptr)
	{
		error_message("Error parsing conditional expression");
		return nullptr;
	}

	while (true)
	{
		struct
		{
			Token::Id tt;
			XprNode::Id st;
		} data[] = {
			{Token::Id::ASSIGN, XprNode::Id::ASSIGN},
			{Token::Id::TIMES_ASSIGN, XprNode::Id::TIMES_ASSIGN},
			{Token::Id::DIV_ASSIGN, XprNode::Id::DIV_ASSIGN},
			{Token::Id::MOD_ASSIGN, XprNode::Id::MOD_ASSIGN},
			{Token::Id::PLUS_ASSIGN, XprNode::Id::PLUS_ASSIGN},
			{Token::Id::MINUS_ASSIGN, XprNode::Id::MINUS_ASSIGN},
			{Token::Id::SHL_ASSIGN, XprNode::Id::SHL_ASSIGN},
			{Token::Id::SHR_ASSIGN, XprNode::Id::SHR_ASSIGN},
			{Token::Id::BITWISE_AND_ASSIGN, XprNode::Id::AND_ASSIGN},
			{Token::Id::BITWISE_OR_ASSIGN, XprNode::Id::OR_ASSIGNMENT},
			{Token::Id::BITWISE_XOR_ASSIGN, XprNode::Id::XOR_ASSIGN},
			{Token::Id::NO_TOKEN}};

		size_t i;
		for (i = 0; data[i].tt != Token::Id::NO_TOKEN; i++)
			if (data[i].tt == m_current_token->get_id())
				break;
		if (data[i].tt == Token::Id::NO_TOKEN)
			break;
		next_token();

		XprNode *rhs = parse_assignment_expression();
		if (rhs == nullptr)
		{
			delete lhs;
			error_message("Error parsing assignment expression");
			return nullptr;
		}

		AssignmentXprNode *n = new AssignmentXprNode(data[i].st);
		n->add_subxpr(lhs);
		n->add_subxpr(rhs);
		if (!n->type_check())
		{
			delete n;
			error_message("Error type checking assignment expression");
			return nullptr;
		}

		lhs = n;
	}

	return lhs;
}

XprNode *Parser::parse_cast_expression()
{
	/*
	cast-expression:
		unary-expression
		( type-name ) cast-expression
	*/
	if (m_current_token->get_id() == Token::Id::PARENTHESES_OPEN)
	{
		next_token();
		if (is_type_name(m_current_token))
		{
			Type t;
			if (!parse_type_name(&t))
			{
				error_message("Error parsing type name in cast expression");
				return nullptr;
			}

			if (!expect(Token::Id::PARENTHESES_CLOSE))
				return nullptr;

			XprNode *rhs = parse_cast_expression();
			if (rhs == nullptr)
			{
				error_message("Error parsing cast expression");
				return nullptr;
			}
			CastXprNode *xpr = new CastXprNode;
			xpr->add_subxpr(rhs);
			xpr->set_xpr_type(t);
			if (!xpr->type_check())
			{
				error_message("Error type checking cast expression");
				delete xpr;
				return nullptr;
			}
			return xpr;
		}
		previous_token();
	}
	return parse_unary_expression();
}

XprNode *Parser::parse_binary_xpr(size_t prec)
{
	XprNode *lhs;
	if (prec == BinaryXprNode::precedence(BinaryXprNode::Id::COMMA))
		lhs = parse_assignment_expression();
	else if (prec == BinaryXprNode::precedence(BinaryXprNode::Id::TIMES))
		lhs = parse_cast_expression();
	else
		lhs = parse_binary_xpr(prec - 1);
	if (lhs == nullptr)
		return nullptr;

	while (m_current_token->is_binop())
	{
		XprNode::Id op = BinaryXprNode::token_to_xpr(m_current_token->get_id());
		if (BinaryXprNode::precedence(op) != prec)
			break;
		next_token();

		XprNode *rhs;
		if (prec == BinaryXprNode::precedence(BinaryXprNode::Id::COMMA))
			rhs = parse_assignment_expression();
		else if (prec == BinaryXprNode::precedence(BinaryXprNode::Id::TIMES))
			rhs = parse_unary_expression();
		else
			rhs = parse_binary_xpr(prec - 1);
		if (rhs == nullptr)
		{
			delete lhs;
			return nullptr;
		}

		XprNode *res = new BinaryXprNode(op);
		res->add_subxpr(lhs);
		res->add_subxpr(rhs);
		lhs = res;
		if (!res->type_check())
		{
			error_message("Type check of binary expression failed");
			delete res;
			return nullptr;
		}
	}

	return lhs;
}

XprNode *Parser::parse_expression()
{
	return parse_binary_xpr(BinaryXprNode::precedence(BinaryXprNode::Id::COMMA));
}

XprNode *Parser::parse_constant_expression()
{
	return parse_conditional_expression();
}

bool Parser::parse_parameter_type_list(TypeNode::Declarations *ptl, bool *is_vararg)
{
	*is_vararg = false;
	while (m_current_token->get_id() != Token::Id::PARENTHESES_CLOSE)
	{
		Declaration decl;
		if (!parse_declaration_specifiers(&decl) || decl.is_typedef()) // true: with storage class specifiers
		{
			error_message("Error parsing declaration specifiers in parameter type list. ");
			return false;
		}

		parse_declarator(&decl);

		if (decl.get_type().is_void())
			break;

		decl.set_type(decl.get_type().first_empty_array_to_pointer());

		ptl->push_back(decl);

		if (m_current_token->get_id() == Token::Id::COMMA)
		{
			next_token();
			if (m_current_token->get_id() == Token::Id::ELLIPSIS)
			{
				*is_vararg = true;
				next_token();
				break;
			}
		}
	}
	return true;
}

bool Parser::parse_post_declarator(Declaration *decl)
{
	if (m_current_token->get_id() == Token::Id::BRACKET_OPEN)
	{
		size_t array_size;
		next_token();
		// TODO this conversion is only allowed in function parameters
		if (m_current_token->get_id() == Token::Id::BRACKET_CLOSE)
			array_size = 0;
		else
		{
			XprNode *xpr = parse_constant_expression();
			if (xpr == nullptr || !xpr->is_constant_expression())
			{
				error_message("Failed to parse constant expression (array subscript) in post declarator");
				return false;
			}
			array_size = (size_t)(xpr->evaluate_constant());
		}
		if (!expect(Token::Id::BRACKET_CLOSE))
		{
			error_message("Failed to parse closing bracket of array subscript");
			return false;
		}
		// process trailig post declarators
		if (!parse_post_declarator(decl))
		{
			error_message("Error parsing post declarator");
			return false;
		}
		Type const &t = decl->get_type();
		if (t.is_incomplete())
		{
			error_message("Array type has incomplete element type");
			return false;
		}
		decl->set_type(t.array_of(array_size));
	}
	else if (m_current_token->get_id() == Token::Id::PARENTHESES_OPEN)
	{
		next_token();
		TypeNode::Declarations ptl;
		bool is_vararg;
		if (!parse_parameter_type_list(&ptl, &is_vararg) || !expect(Token::Id::PARENTHESES_CLOSE))
		{
			error_message("Error parsing parameter type list");
			return false;
		}
		// no need to check here for other postdeclarators as function returning array is not allowed
		decl->set_type(decl->get_type().function_returning(ptl, is_vararg));
		return true;
	}

	return true;
}

bool Parser::parse_direct_declarator(Declaration *decl)
{
	Declaration inner;
	inner.set_type(Type::void_type());

	bool jump_to_post = false;
	if (m_current_token->get_id() == Token::Id::PARENTHESES_OPEN)
	{
		next_token();
		// at this point we need to decide whether we are parsing  ( declarator ) or ( parameter-type-list ) in an abstract function declaration.
		// in the latter case the following token is a specifier-qualifier, and we step back.
		if (is_type_specifier(m_current_token) || m_current_token->is_type_qualifier())
		{
			jump_to_post = true;
			previous_token();
		}
		else
		{
			if (!parse_declarator(&inner))
			{
				error_message("Could not parse ( declarator ) in direct declarator");
				return false;
			}
			if (!expect(Token::Id::PARENTHESES_CLOSE))
			{
				error_message("Expecting a closing parenthesis at the end of a ( declarator ) in a direct declarator");
				return false;
			}
		}
	}

	if (!jump_to_post && m_current_token->get_id() == Token::Id::IDENTIFIER)
	{
		decl->set_identifier(m_current_token->get_string());
		next_token();
	}

	if (!parse_post_declarator(decl))
	{
		error_message("Error parsing post declarator part in a direct declarator");
		return false;
	}

	decl->set_type(inner.get_type().replace_back_to(decl->get_type()));
	if (decl->get_identifier() == "" && inner.get_identifier() != "")
		decl->set_identifier(inner.get_identifier());

	return true;
}

bool Parser::parse_declarator(Declaration *decl)
{
	while (m_current_token->get_id() == Token::Id::ASTERISK)
	{
		next_token();
		decl->set_type(decl->get_type().pointer_to());
		while (m_current_token->is_type_qualifier())
		{
			std::cout << "Warning: type qualifier omitted in pointer declaration";
			next_token();
		}
	}

	return parse_direct_declarator(decl);
}

bool Parser::parse_struct_specifier(std::string *type_name)
{
	if (!expect(Token::Id::STRUCT))
	{
		error_message("Expecting a \"struct\" keyword");
		return false;
	}

	// read enum tag if it exists. Else generate a new anonymous tag
	std::string struct_tag;
	if (m_current_token->get_id() == Token::Id::IDENTIFIER)
	{
		struct_tag = m_current_token->get_string();
		next_token();
	}
	else
		struct_tag = generate_anonymous_enum_tag();

	// determine full enum name. This identifies the type in the symbol table
	std::string struct_name = "struct " + struct_tag;

	if (m_current_token->get_id() != Token::Id::BRACE_OPEN)
	{
		// if the next character is not an opening brace,
		// then the type either exists, or it needs to be installed as incomplete.
		if (m_st_ptr->lookup_global(struct_name) == nullptr)
		{
			// true: ensure new type even if the tag already exists
			Type new_type = Type::install(TypeNode(TypeNode::STRUCT, struct_tag), true);
			m_st_ptr->install_type(struct_name, new_type);
		}
	}
	else
	{
		// if the next character is an opening brace, then
		// the type either exists in this scope locally and is incomplete,
		// or needs to be installed as incomplete.
		// After this it will be completed
		next_token();

		SymbolTableEntry const *pste = m_st_ptr->lookup_local(struct_name);
		if (pste != nullptr)
		{
			// if exists, it needs to be incomplete
			if (!pste->get_type().is_incomplete())
			{
				error_message("redefinition of complete structure type");
				return false;
			}
		}
		else
		{
			// true: ensure new type even if the tag already exists in the type pool
			Type new_type = Type::install(TypeNode(TypeNode::STRUCT, struct_tag), true); // true: as new type
			m_st_ptr->install_type(struct_name, new_type);
		}

		// lookup the incomplete type and start completion
		Type *struct_type = &m_st_ptr->lookup_local(struct_name)->get_type();

		std::vector<Declaration> declarations;
		while (m_current_token->get_id() != Token::Id::BRACE_CLOSE)
			parse_struct_declaration(&declarations);

		for (auto const &d : declarations)
			struct_type->add_field(d.get_identifier(), d.get_type());

		next_token(); // consume closing brace
	}

	*type_name = struct_name;
	return true;
}

bool Parser::parse_enum_specifier(std::string *type_name)
{
	if (!expect(Token::Id::ENUM))
	{
		error_message("Expecting an \"enum\" keyword");
		return false;
	}

	// read enum tag if it exists. Else generate a new anonymous tag
	std::string enum_tag;
	if (m_current_token->get_id() == Token::Id::IDENTIFIER)
	{
		enum_tag = m_current_token->get_string();
		next_token();
	}
	else
		enum_tag = generate_anonymous_enum_tag();

	// determine full enum name. This identifies the type in the symbol table
	std::string enum_name = "enum " + enum_tag;

	if (m_current_token->get_id() != Token::Id::BRACE_OPEN)
	{
		// if the next character is not an opening brace,
		// then the type either exists, or it needs to be installed as incomplete.
		if (m_st_ptr->lookup_global(enum_name) == nullptr)
		{
			// true: ensure new type even if the tag already exists
			Type new_type = Type::install(TypeNode(TypeNode::ENUM, enum_tag), true);
			m_st_ptr->install_type(enum_name, new_type);
		}
	}
	else
	{
		// if the next character is an opening brace, then
		// the type either exists in this scope locally and is incomplete,
		// or needs to be installed as incomplete.
		// After this it will be completed
		next_token();

		auto const *pste = m_st_ptr->lookup_local(enum_name);
		if (pste != nullptr)
		{
			// if exists, it needs to be incomplete
			if (!pste->get_type().is_incomplete())
			{
				error_message("redefinition of complete enumerated type");
				return false;
			}
		}
		else
		{
			// true: ensure new type even if the tag already exists in the type pool
			Type new_type = Type::install(TypeNode(TypeNode::ENUM, enum_tag), true); // true: as new type
			m_st_ptr->install_type(enum_name, new_type);
		}

		// lookup the incomplete type and start completion
		Type *enum_type = &m_st_ptr->lookup_local(enum_name)->get_type();

		// actual enum constant value
		int c = 0;

		while (m_current_token->get_id() == Token::Id::IDENTIFIER)
		{
			std::string enum_symbol = m_current_token->get_string();
			next_token();
			if (m_current_token->get_id() == Token::Id::ASSIGN)
			{
				next_token();
				XprNode *xpr = parse_constant_expression();
				if (xpr == nullptr || !xpr->is_constant_expression())
				{
					error_message("Could not parse constant expression in enumerator");
					delete xpr;
					return false;
				}
				c = xpr->evaluate_constant();
			}

			if (enum_type->lookup_enum_constant(enum_symbol) != nullptr)
			{
				error_message("Redefinition of enumeration constant: " + enum_symbol);
				return false;
			}
			enum_type->add_enum_constant(enum_symbol, c++);

			if (m_current_token->get_id() == Token::Id::COMMA)
				next_token();
		}

		if (!expect(Token::Id::BRACE_CLOSE))
		{
			error_message("Error finishing enumerator list");
			return false;
		}
	}

	*type_name = enum_name;
	return true;
}

bool Parser::parse_declaration_specifiers(Declaration *decl)
{
	std::vector<Token::Id> type_specifiers, type_qualifiers;
	std::string typedef_name = "";

	decl->set_typedef(false);
	decl->set_storage(Storage::NO_STORAGE);
	decl->set_linkage(Linkage::UNDEFINED_LINKAGE);

	// collect specifiers and qualifiers in the corresponding vectors
	while (true)
	{
		if (m_current_token->get_id() == Token::Id::TYPEDEF)
		{
			if (decl->is_typedef())
			{
				error_message("Twice typedef");
				return false;
			}
			decl->set_typedef(true);
			next_token();
		}
		else if (m_current_token->is_storage_class_specifier())
		{
			if (decl->get_storage() != Storage::NO_STORAGE)
			{
				error_message("Double storage-class specifier");
				return false;
			}
			switch (m_current_token->get_id())
			{
			case Token::Id::EXTERN:
				decl->set_storage(Storage::EXTERN);
				break;
			case Token::Id::AUTO:
				decl->set_storage(Storage::AUTO);
				break;
			case Token::Id::STATIC:
				decl->set_storage(Storage::STATIC);
				break;
			case Token::Id::REGISTER:
				decl->set_storage(Storage::REGISTER);
				break;
			}
			next_token();
		}
		else if (typedef_name == "" && is_type_specifier(m_current_token))
		{
			// if typedef name
			if (m_current_token->get_id() == Token::Id::IDENTIFIER)
			{
				// it needs to be checked if type_specifiers is empty or not
				// if it is not empty then this id should be a declarator
				// and parsing of specifiers should be over
				if (!type_specifiers.empty())
					break;

				auto pste = m_st_ptr->lookup_global(m_current_token->get_string());
				if (pste != nullptr && pste->get_category() == SymbolTableEntry::TYPE)
				{
					typedef_name = m_current_token->get_string();
					next_token();
				}
			}
			// if enum specifier
			else if (m_current_token->get_id() == Token::Id::ENUM)
			{
				if (!parse_enum_specifier(&typedef_name))
				{
					error_message("Error parsing enum specifier");
					return false;
				}
			}
			// if struct specifier
			else if (m_current_token->get_id() == Token::Id::STRUCT)
			{
				if (!parse_struct_specifier(&typedef_name))
				{
					error_message("Error parsing enum specifier");
					return false;
				}
			}
			// base type
			else
			{
				type_specifiers.push_back(m_current_token->get_id());
				next_token();
			}
		}
		else if (m_current_token->is_type_qualifier())
		{
			type_qualifiers.push_back(m_current_token->get_id());
			next_token();
		}
		else
			break;
	}

	if (decl->is_typedef() && decl->get_storage() != Storage::NO_STORAGE)
	{
		error_message("typedef and storage-class specifier together NiHu");
		return false;
	}

	// determine base type from type specifier
	if (typedef_name != "")
	{
		if (!type_specifiers.empty())
		{
			error_message("Invalid type specifiers in declaration.");
			return false;
		}

		auto *pste = m_st_ptr->lookup_global(typedef_name);
		if (pste == nullptr)
		{
			error_message("Did not find identifier " + typedef_name + " in the type table");
			return false;
		}
		decl->set_type(pste->get_type());
		return true;
	}

	// there is no typedefname in type specifiers
	std::sort(type_specifiers.begin(), type_specifiers.end());

	struct
	{
		size_t siz;
		Token::Id pattern[4];
		Type t;
	} patterns[] = {
		{1, {Token::Id::CHAR}, Type::char_type()},
		{1, {Token::Id::DOUBLE}, Type::double_type()},
		{1, {Token::Id::INT}, Type::int_type()},
		{1, {Token::Id::LONG}, Type::long_type()},
		{1, {Token::Id::SHORT}, Type::short_type()},
		{1, {Token::Id::SIGNED}, Type::int_type()},
		{1, {Token::Id::UNSIGNED}, Type::uint_type()},
		{1, {Token::Id::VOID}, Type::void_type()},
		{2, {Token::Id::CHAR, Token::Id::SIGNED}, Type::schar_type()},
		{2, {Token::Id::CHAR, Token::Id::UNSIGNED}, Type::uchar_type()},
		{2, {Token::Id::INT, Token::Id::LONG}, Type::long_type()},
		{2, {Token::Id::INT, Token::Id::SHORT}, Type::short_type()},
		{2, {Token::Id::INT, Token::Id::SIGNED}, Type::int_type()},
		{2, {Token::Id::INT, Token::Id::UNSIGNED}, Type::uint_type()},
		{2, {Token::Id::LONG, Token::Id::LONG}, Type::llong_type()},
		{2, {Token::Id::LONG, Token::Id::SIGNED}, Type::long_type()},
		{2, {Token::Id::LONG, Token::Id::UNSIGNED}, Type::ulong_type()},
		{2, {Token::Id::SHORT, Token::Id::SIGNED}, Type::short_type()},
		{2, {Token::Id::SHORT, Token::Id::UNSIGNED}, Type::ushort_type()},
		{3, {Token::Id::INT, Token::Id::LONG, Token::Id::SIGNED}, Type::long_type()},
		{3, {Token::Id::INT, Token::Id::LONG, Token::Id::UNSIGNED}, Type::ulong_type()},
		{3, {Token::Id::INT, Token::Id::SIGNED, Token::Id::SHORT}, Type::short_type()},
		{3, {Token::Id::INT, Token::Id::SHORT, Token::Id::UNSIGNED}, Type::ushort_type()},
		{3, {Token::Id::LONG, Token::Id::LONG, Token::Id::SIGNED}, Type::llong_type()},
		{3, {Token::Id::LONG, Token::Id::LONG, Token::Id::UNSIGNED}, Type::ullong_type()},
		{4, {Token::Id::INT, Token::Id::LONG, Token::Id::LONG, Token::Id::SIGNED}, Type::llong_type()},
		{4, {Token::Id::INT, Token::Id::LONG, Token::Id::LONG, Token::Id::UNSIGNED}, Type::ullong_type()},
	};

	for (int i = 0; i < sizeof(patterns) / sizeof(patterns[0]); ++i)
	{
		if (type_specifiers.size() != patterns[i].siz)
			continue;
		size_t j;
		for (j = 0; j < patterns[i].siz; ++j)
			if (patterns[i].pattern[j] != type_specifiers[j])
				break;
		if (j == patterns[i].siz)
		{
			decl->set_type(patterns[i].t);
			return true;
		}
	}
	error_message("Could not match type specifier to standard ");

	return false;
}

bool Parser::parse_struct_declaration(std::vector<Declaration> *declarations)
{
	Type type;
	Declaration d;
	if (!parse_declaration_specifiers(&d)) // true: allow storage class specifiers
	{
		error_message("Error parsing declaration specifiers. ");
		return false;
	}

	// process comma separated declarators
	while (true)
	{
		Declaration decl = d;
		if (!parse_declarator(&decl))
		{
			error_message("Error parsing declarator in declaration");
			return false;
		}

		Type const &t = decl.get_type();
		if (t.is_incomplete())
		{
			error_message("Cannot use incomplete type as struct member");
			return false;
		}
		if (decl.is_abstract())
		{
			error_message("Structure declarator can not be abstract");
			return false;
		}
		declarations->push_back(decl);

		if (m_current_token->get_id() == Token::Id::COMMA)
		{
			next_token();
			continue;
		}
		if (m_current_token->get_id() == Token::Id::SEMICOLON)
		{
			next_token();
			return true;
		}
		error_message("Expecting comma or semicolon after declarator");
		return false;
	}
}

XprNode *Parser::parse_initializer(XprNode *lhs)
{
	Type const &type = lhs->get_xpr_type();
	if (type.is_structure())
	{
		expect(Token::Id::BRACE_OPEN);
		size_t n_members = type.get_num_declarations();
		XprNode *xpr = nullptr;
		for (size_t i_member = 0; i_member < n_members; ++i_member)
		{
			bool first = i_member == 0;

			// transform lhs into an appropriate array subscript expression
			PostfixXprNode *member = new PostfixXprNode(XprNode::Id::STRUCTURE_MEMBER);
			if (first)
				member->add_subxpr(lhs);
			else
				member->add_subxpr(lhs->clone());
			member->add_subxpr(new IdentifierXprNode(XprNode::Id::FIELDNAME_XPR, type.get_declaration(i_member).get_identifier()));
			member->type_check();

			// parse initializer
			XprNode *assign = parse_initializer(member);
			if (first)
				xpr = assign;
			else
			{ // in this case xpr is a comma xpr with lhs only
				// complete comma expression by adding rhs
				xpr->add_subxpr(assign);
				xpr->type_check();
			}

			if (i_member < n_members - 1)
			{
				expect(Token::Id::COMMA);
				XprNode *comma = new BinaryXprNode(XprNode::Id::COMMA);
				comma->add_subxpr(xpr);
				xpr = comma;
			}
		}
		expect(Token::Id::BRACE_CLOSE);
		return xpr;
	}
	else if (type.is_array())
	{
		expect(Token::Id::BRACE_OPEN);
		unsigned long element_cntr = 0;
		XprNode *xpr = nullptr;
		while (true)
		{
			bool first = element_cntr == 0;

			// transform lhs into an appropriate array subscript expression
			PostfixXprNode *arr_sbs = new PostfixXprNode(XprNode::Id::ARRAY_SUBSCRIPT);
			if (first)
				arr_sbs->add_subxpr(lhs);
			else
				arr_sbs->add_subxpr(lhs->clone());
			arr_sbs->add_subxpr(new IntegerConstant(Type::ulong_type(), &element_cntr));
			element_cntr++;
			arr_sbs->type_check();

			// parse initializer
			XprNode *assign = parse_initializer(arr_sbs);
			if (first)
				xpr = assign;
			else
			{ // in this case xpr is a comma xpr with lhs only
				// complete comma expression by adding rhs
				xpr->add_subxpr(assign);
				xpr->type_check();
			}

			if (m_current_token->get_id() == Token::Id::COMMA)
			{
				XprNode *comma = new BinaryXprNode(XprNode::Id::COMMA);
				comma->add_subxpr(xpr);
				xpr = comma;
				next_token();
				continue;
			}
			if (m_current_token->get_id() == Token::Id::BRACE_CLOSE)
			{
				next_token();
				break;
			}
		}
		return xpr;
	}
	else
	{
		XprNode *rhs = parse_assignment_expression();
		AssignmentXprNode *ret = new AssignmentXprNode(XprNode::Id::ASSIGN);
		ret->add_subxpr(lhs);
		ret->add_subxpr(rhs);
		ret->type_check();
		return ret;
	}
}

bool Parser::parse_declaration()
{
	Declaration d;
	if (!parse_declaration_specifiers(&d)) // true: with storage class specifiers
	{
		error_message("Error parsing declaration specifiers. ");
		return false;
	}

	// empty declarator is allowed
	if (m_current_token->get_id() == Token::Id::SEMICOLON)
	{
		next_token();
		return true;
	}

	// process comma separated declarators
	while (true)
	{
		Declaration decl = d;
		if (!parse_declarator(&decl))
		{
			error_message("Error parsing declarator in declaration");
			return false;
		}

		if (!check_declaration(&decl))
		{
			error_message("Error checking declarator");
			return false;
		}

		// check if initializer comes
		if (m_current_token->get_id() == Token::Id::ASSIGN)
		{
			next_token();
			IdentifierXprNode *lhs = new IdentifierXprNode(XprNode::Id::IDENTIFIER);
			lhs->set_xpr_type(decl.get_type());
			lhs->set_identifier(decl.get_identifier());
			XprNode *xpr = parse_initializer(lhs);
			auto init = std::make_shared<StmNode>(StmNode::Id::XPR);
			init->add_subxpr(xpr);
			m_current_block->add_substm(init);
		}

		if (m_current_token->get_id() == Token::Id::COMMA)
		{
			next_token();
			continue;
		}
		if (m_current_token->get_id() == Token::Id::SEMICOLON)
		{
			next_token();
			return true;
		}
		error_message("Expecting comma or semicolon after declarator");
		return false;
	}
}

bool Parser::parse_declarations()
{
	while (m_current_token->is_storage_class_specifier() || is_type_specifier(m_current_token) || m_current_token->is_type_qualifier())
	{
		if (!parse_declaration())
		{
			error_message("Could not parse declaration");
			return false;
		}
	}

	return true;
}

std::shared_ptr<StmNode> Parser::parse_return_statement()
{
	XprNode *xpr = nullptr;
	if (!expect(Token::Id::RETURN))
		return nullptr;
	if (m_current_token->get_id() != Token::Id::SEMICOLON)
	{
		xpr = parse_expression();
		if (xpr == nullptr)
		{
			error_message("Error parsing return expression");
			return nullptr;
		}
	}
	if (!expect(Token::Id::SEMICOLON))
	{
		error_message("Missing semicolon after return statement");
		delete xpr;
	}
	auto stm = std::make_shared<StmNode>(StmNode::Id::RETURN);
	if (xpr != nullptr)
		stm->add_subxpr(xpr);
	return stm;
}

std::shared_ptr<StmNode> Parser::parse_break_statement()
{
	if (!expect(Token::Id::BREAK))
		return nullptr;
	if (!expect(Token::Id::SEMICOLON))
		return nullptr;
	return std::make_shared<StmNode>(StmNode::Id::BREAK);
}

std::shared_ptr<StmNode> Parser::parse_continue_statement()
{
	if (!expect(Token::Id::CONTINUE))
		return nullptr;
	if (!expect(Token::Id::SEMICOLON))
		return nullptr;
	return std::make_shared<StmNode>(StmNode::Id::CONTINUE);
}

std::shared_ptr<StmNode> Parser::parse_if_statement()
{
	if (!expect(Token::Id::IF))
		return nullptr;

	if (!expect(Token::Id::PARENTHESES_OPEN))
		return nullptr;

	auto wh = std::make_shared<StmNode>(StmNode::Id::IF);

	auto xpr = parse_expression();

	if (!expect(Token::Id::PARENTHESES_CLOSE))
	{
		delete xpr;
		return nullptr;
	}
	wh->add_subxpr(xpr);

	auto tr_stm = parse_statement();
	if (tr_stm == nullptr)
		return nullptr;
	wh->add_substm(tr_stm);

	if (m_current_token->get_id() == Token::Id::ELSE)
	{
		expect(Token::Id::ELSE);

		auto fls_stm = parse_statement();
		if (fls_stm == nullptr)
		{
			delete xpr;
			return nullptr;
		}
		wh->add_substm(fls_stm);
	}

	return wh;
}

std::shared_ptr<StmNode> Parser::parse_while_statement()
{
	if (!expect(Token::Id::WHILE))
		return nullptr;

	if (!expect(Token::Id::PARENTHESES_OPEN))
		return nullptr;

	XprNode *xpr = parse_expression();
	if (xpr == nullptr)
	{
		error_message("Error parsing the condition of a while statement");
		return nullptr;
	}

	if (!expect(Token::Id::PARENTHESES_CLOSE))
		return nullptr;

	auto stm = parse_statement();
	if (stm == nullptr)
	{
		error_message("Error parsing the body of a while statement");
		delete xpr;
		return nullptr;
	}

	auto wh = std::make_shared<StmNode>(StmNode::Id::WHILE);
	wh->add_subxpr(xpr);
	wh->add_substm(stm);
	return wh;
}

std::shared_ptr<StmNode> Parser::parse_do_statement()
{
	if (!expect(Token::Id::DO))
	{
		error_message("Error parsing \"do\" t the beginning of the do statement");
		return nullptr;
	}

	auto stm = parse_statement();
	if (stm == nullptr)
	{
		error_message("Error parsing the body of a do-while statement");
		return nullptr;
	}

	if (!expect(Token::Id::WHILE))
	{
		error_message("Error parsing \"while\" in a do-while statement");
		return nullptr;
	}

	if (!expect(Token::Id::PARENTHESES_OPEN))
	{
		error_message("Missing opening brace in the condition of a do-while statement");
		return nullptr;
	}

	XprNode *xpr = parse_expression();
	if (xpr == nullptr)
	{
		error_message("Error parsing the condition of a do-while statement");
		return nullptr;
	}

	if (!expect(Token::Id::PARENTHESES_CLOSE))
	{
		error_message("Missing closing parenthesis \")\" in the condition of a do-while statement");
		delete xpr;
		return nullptr;
	}

	if (!expect(Token::Id::SEMICOLON))
	{
		error_message("Missing semicolon after a do-while statement");
		delete xpr;
		return nullptr;
	}

	auto dwh = std::make_shared<StmNode>(StmNode::Id::DO);
	dwh->add_subxpr(xpr);
	dwh->add_substm(stm);
	return dwh;
}

std::shared_ptr<StmNode> Parser::parse_for_statement()
{
	if (!expect(Token::Id::FOR))
		return nullptr;

	if (!expect(Token::Id::PARENTHESES_OPEN))
	{
		error_message("Expecting opening parentheses after for keyword");
		return nullptr;
	}

	XprNode *init = nullptr, *xpr = nullptr, *step = nullptr;
	if (m_current_token->get_id() != Token::Id::SEMICOLON)
	{
		init = parse_expression();
		if (init == nullptr)
		{
			error_message("Error parsing initializing expression of for statement");
			return nullptr;
		}
	}
	else
	{
		IntegerConstant *x = new IntegerConstant;
		x->set_xpr_type(Type::int_type());
		int v = 1;
		x->set_value(&v);
		init = x;
	}
	if (!expect(Token::Id::SEMICOLON))
	{
		error_message("Missing semicolon after initializing expression of for statement");
		delete init;
		return nullptr;
	}
	if (m_current_token->get_id() != Token::Id::SEMICOLON)
	{
		xpr = parse_expression();
		if (xpr == nullptr)
		{
			error_message("Error parsing condition expression in for statement");
			delete init;
			return nullptr;
		}
	}
	else
	{
		IntegerConstant *x = new IntegerConstant;
		x->set_xpr_type(Type::int_type());
		int v = 1;
		x->set_value(&v);
		xpr = x;
	}
	if (!expect(Token::Id::SEMICOLON))
	{
		error_message("Missing semicolon after condition expression of for statement");
		delete init;
		delete xpr;
		return nullptr;
	}
	if (m_current_token->get_id() != Token::Id::PARENTHESES_CLOSE)
	{
		step = parse_expression();
		if (step == nullptr)
		{
			error_message("Error parsing step expression in for statement");
			delete init;
			delete xpr;
			return nullptr;
		}
	}
	else
	{
		IntegerConstant *x = new IntegerConstant;
		x->set_xpr_type(Type::int_type());
		int v = 1;
		x->set_value(&v);
		step = x;
	}

	if (!expect(Token::Id::PARENTHESES_CLOSE))
	{
		error_message("Missing closing parenthesis after step expression of for statement");
		delete init;
		delete xpr;
		delete step;
		return nullptr;
	}
	auto stm = parse_statement();
	if (stm == nullptr)
	{
		error_message("Error parsing the body of a for statement");
		delete init;
		delete xpr;
		delete step;
		return nullptr;
	}

	auto fr = std::make_shared<StmNode>(StmNode::Id::FOR);
	fr->add_subxpr(init);
	fr->add_subxpr(xpr);
	fr->add_subxpr(step);
	fr->add_substm(stm);
	return fr;
}

std::shared_ptr<StmNode> Parser::parse_empty_statement()
{
	if (!expect(Token::Id::SEMICOLON))
		return nullptr;
	return std::make_shared<StmNode>(StmNode::Id::EMPTY);
}

std::shared_ptr<StmNode> Parser::parse_expression_statement()
{
	// expression statement
	XprNode *xpr = parse_expression();
	if (xpr == nullptr)
	{
		error_message("Error parsing expression in expression statement.");
		return nullptr;
	}
	auto xprstm = std::make_shared<StmNode>(StmNode::Id::XPR);
	xprstm->add_subxpr(xpr);
	if (!expect(Token::Id::SEMICOLON))
	{
		error_message("Expecting semicolon after expression statement");
		return nullptr;
	}
	return xprstm;
}

std::shared_ptr<StmNode> Parser::parse_statement()
{
	if (m_current_token->get_id() == Token::Id::SEMICOLON)
		return parse_empty_statement();

	if (m_current_token->get_id() == Token::Id::WHILE)
		return parse_while_statement();

	if (m_current_token->get_id() == Token::Id::DO)
		return parse_do_statement();

	if (m_current_token->get_id() == Token::Id::FOR)
		return parse_for_statement();

	if (m_current_token->get_id() == Token::Id::IF)
		return parse_if_statement();

	if (m_current_token->get_id() == Token::Id::RETURN)
		return parse_return_statement();

	if (m_current_token->get_id() == Token::Id::CONTINUE)
		return parse_continue_statement();

	if (m_current_token->get_id() == Token::Id::BREAK)
		return parse_break_statement();

	if (m_current_token->get_id() == Token::Id::BRACE_OPEN)
		return parse_compound_statement();

	return parse_expression_statement();
}

std::shared_ptr<CompoundNode> Parser::parse_compound_statement()
{
	if (!expect(Token::Id::BRACE_OPEN))
	{
		error_message("Error parsing compound statement");
		return nullptr;
	}

	enter_scope();

	auto block = std::make_shared<CompoundNode>();
	m_current_block = block;
	block->set_symbol_pointer(m_st_ptr);

	if (!parse_declarations())
	{
		error_message("Error parsing declarations at beginning of block");
		delete m_st_ptr;
		return nullptr;
	}

	while (m_current_token->get_id() != Token::Id::BRACE_CLOSE)
	{
		auto st = parse_statement();
		if (st == nullptr)
		{
			error_message("Error parsing statement in block");
			return nullptr;
		}
		block->add_substm(st);
	}

	next_token();

	exit_scope();

	return block;
}

bool Parser::check_declaration(Declaration *decl)
{
	// set scope member based on the current symbol table
	bool is_file_scope = m_st_ptr->get_parent() == nullptr;
	if (is_file_scope)
		decl->set_scope(Scope::FILE_SCOPE);
	else
		decl->set_scope(Scope::BLOCK_SCOPE);

	Type const &type = decl->get_type();

	if (is_file_scope && (decl->get_storage() == Storage::AUTO || decl->get_storage() == Storage::REGISTER))
	{
		error_message("auto and register is invalid for external declarations");
		return false;
	}

	if (is_file_scope && decl->get_storage() == Storage::STATIC)
		decl->set_linkage(Linkage::INTERNAL_LINKAGE);

	if (decl->get_storage() == Storage::EXTERN || type.is_function() && decl->get_storage() == Storage::NO_STORAGE)
	{
		// lookup linkage
		SymbolTableEntry const *ste = m_st_ptr->lookup_global(decl->get_identifier());
		if (ste != nullptr)
		{
			Linkage link = ste->get_linkage();
			if (link == Linkage::EXTERNAL_LINKAGE || link == Linkage::INTERNAL_LINKAGE)
				decl->set_linkage(link);
			else
				decl->set_linkage(Linkage::EXTERNAL_LINKAGE);
		}
		else
			decl->set_linkage(Linkage::EXTERNAL_LINKAGE);
	}

	if (is_file_scope && type.is_object() && decl->get_storage() == Storage::NO_STORAGE)
		decl->set_linkage(Linkage::EXTERNAL_LINKAGE);

	if (!type.is_function() && !type.is_object())
		decl->set_linkage(Linkage::NO_LINKAGE);
	if (!is_file_scope && type.is_object() && decl->get_storage() != Storage::EXTERN)
		decl->set_linkage(Linkage::NO_LINKAGE);

	// If an identifier has no linkage, there shall be no more than one declaration of the identifier
	// (in a declarator or type specifier) with the same scope and in the same name space
	if (decl->get_linkage() == Linkage::NO_LINKAGE && m_st_ptr->lookup_local(decl->get_identifier()) != nullptr)
	{
		if (decl->is_typedef())
		{
			auto ptr = m_st_ptr->lookup_local(decl->get_identifier());
			if (ptr != nullptr && ptr->get_type() != decl->get_type())
			{
				error_message("Redeclaration of typedef-name " + decl->get_identifier() + " with different type");
				return false;
			}
		}
		else
		{
			error_message("Redefinition of identifier " + decl->get_identifier());
			return false;
		}
	}
	if (decl->is_typedef())
		m_st_ptr->install_type(decl->get_identifier(), decl->get_type());
	else
		m_st_ptr->install_object(decl->get_identifier(), decl->get_type(), decl->get_storage());

#if 0
		if (is_file_scope)

		if (type.is_incomplete())
		{
			error_message("Trying to add symbol of incomplete type");
			return false;
		}
		if (type.is_function() && !m_st_ptr->install_function(decl->get_identifier(), type))
		{
			error_message("Redeclaration of a function with different type");
			return false;
		}
		if (!type.is_function() && !m_st_ptr->install_object(decl->get_identifier(), type, decl->get_storage()))
		{
			error_message("Redefinition of object");
			return false;
		}
#endif
	return true;
}

TransUnitNode *Parser::parse_translation_unit()
{
	// allocate space for the global symbol tables
	enter_scope();

	// create AST node and set symbol pointer
	TransUnitNode *translation_unit = new TransUnitNode;
	m_translation_unit = translation_unit;
	translation_unit->set_symbol_pointer(m_st_ptr);

	// parse external declarations
	while (m_current_token->get_id() != Token::Id::END_OF_FILE)
	{
		Declaration d;
		if (!parse_declaration_specifiers(&d))
		{
			error_message("Error parsing declaration specifiers at the beginning of new external declaration.");
			delete translation_unit;
			return nullptr;
		}

		// parse declarators
		bool in_declaration = false;
		do
		{
			// possibly empty declaration
			if (m_current_token->get_id() == Token::Id::SEMICOLON)
			{
				next_token();
				break;
			}

			Declaration decl = d;
			if (!parse_declarator(&decl))
			{
				error_message("Error parsing declarator of external declaration.");
				delete translation_unit;
				return nullptr;
			}

			// determine if function definition or declaration
			if (!in_declaration && !decl.is_typedef() && !decl.is_abstract() && decl.get_type().is_function() && m_current_token->get_id() == Token::Id::BRACE_OPEN)
			{
				// add function declaration to translation unit
				m_st_ptr->install_function(decl.get_identifier(), decl.get_type());

				// parse function definition
				enter_scope();

				Type const &fun_type = decl.get_type();
				for (size_t i = 0; i < fun_type.get_num_declarations(); ++i)
				{
					if (fun_type.get_declaration(i).get_type() == Type::void_type())
						break;
					Declaration const &decl = fun_type.get_declaration(i);
					m_st_ptr->install_object(decl.get_identifier(), decl.get_type(), decl.get_storage());
				}

				auto block = parse_compound_statement();
				if (block == nullptr)
				{
					error_message("Error parsing function block");
					delete translation_unit;
					return nullptr;
				}

				FunctionNode *function = new FunctionNode;
				function->set_return_type(decl.get_type().return_type());
				function->set_identifier(decl.get_identifier());
				function->set_block(block);
				function->set_symbol_pointer(m_st_ptr);

				exit_scope();

				translation_unit->add_function(function);
				break;
			}
			else // external declaration
			{
				in_declaration = true;

				if (!check_declaration(&decl))
				{
					error_message("Error checking declaration");
					delete translation_unit;
					return nullptr;
				}

				if (m_current_token->get_id() == Token::Id::SEMICOLON)
				{
					next_token();
					break;
				}
				else if (m_current_token->get_id() == Token::Id::COMMA)
					next_token();
				else
				{
					error_message("Expecting semicolon or comma after declarator");
					delete translation_unit;
					return nullptr;
				}
			}
		} while (in_declaration); // loop over declarators
	}							  // loop over declarations

	exit_scope();
	return translation_unit;
}

bool Parser::parse()
{
	delete m_translation_unit;
	next_token();
	m_translation_unit = parse_translation_unit();
	if (m_current_token->get_id() != Token::Id::END_OF_FILE)
	{
		error_message("Tokens remaining after parsing finished.");
		return false;
	}
	return true;
}

void Parser::next_token(void)
{
	if (m_token_list_ptr == m_token_list_end)
		m_current_token = &m_token_eof;
	else
	{
		m_current_token = &*m_token_list_ptr;
		++m_token_list_ptr;
	}
}

void Parser::previous_token(void)
{
	--m_token_list_ptr;
	--m_token_list_ptr;
	next_token();
}

bool Parser::expect(Token::Id t)
{
	if (m_current_token->get_id() != t)
	{
		error_message("Expecting token: \"" + Token(t).get_string() + "\". Receiving token: \"" + m_current_token->get_string() + "\".");
		return false;
	}
	next_token();
	return true;
}

void Parser::enter_scope(void)
{
	auto p = new SymbolNode;
	p->set_parent(m_st_ptr);
	m_st_ptr = p;
}

void Parser::exit_scope(void)
{
	m_st_ptr = m_st_ptr->get_parent();
}
