#include "postfix_xpr_node.h"

#include "identifier_xpr_node.h"

bool PostfixXprNode::type_check_impl()
{
	XprNode const *first_arg = get_subxpr(0);
	Type const &first_type = first_arg->get_xpr_type();

	switch (get_id())
	{
	case Id::POSTINCREMENT:
	case Id::POSTDECREMENT:
	{
		/*
			The operand of the postfix increment or decrement operator shall have qualified or unqualified scalar type
			and shall be a modifiable lvalue.
		*/
		if (!first_arg->is_modifiable_lvalue())
		{
			std::cerr << "Argument of the postin/decrement operator should be a modifiable lvalue." << std::endl;
			return false;
		}
		if (!first_type.is_scalar())
		{
			std::cerr << "Argument of the postin/decrement operator should be real or pointer type." << std::endl;
			return false;
		}
		set_xpr_type(first_type);
		return true;
	}
	case Id::FUNCTION_CALL:
	{
		/*
			The expression that denotes the called function shall have type pointer to function returning void
			or returning an object type other than array.
			If the expression that denotes the called function has a type that includes a prototype,
			the number of arguments shall agree with the number of parameters.
			Each argument shall have a type such that its value may be assigned to an object with the unqualified
			version of the type of its corresponding parameter.
		*/

		// return type must be void or complete object
		Type const &function_type = first_type.referenced_type();
		Type const &return_type = function_type.return_type();

		if (!return_type.is_complete_object() && !return_type.is_void())
		{
			std::cerr << "Function return type must be void or complete object type";
			return false;
		}
		// first argument should be pointer to a function
		if (!first_type.is_pointer() || !first_type.referenced_type().is_function())
		{
			std::cerr << "First argument of a function call expression should be a pointer to a function" << std::endl;
			return false;
		}
		size_t num_parameters = function_type.get_num_declarations();
		bool is_vararg = function_type.is_vararg();
		// check number of arguments
		size_t num_arguments = get_num_subxprs() - 1;
		if (!is_vararg && num_parameters != num_arguments)
		{
			std::cerr << "Number of arguments should match the number of parameters" << std::endl;
			return false;
		}
		if (is_vararg && num_arguments < num_parameters)
		{
			std::cerr << "Number of arguments should not be less than number of parameters of a vararg function" << std::endl;
			return false;
		}
		// check all arguments
		for (size_t i = 0; i < num_arguments; ++i)
		{
			XprNode *arg_xpr = get_subxpr(i + 1);
			Type const &arg_type = arg_xpr->get_xpr_type();
			if (!arg_type.is_complete_object())
			{
				std::cerr << "Each argument of the function call shall be a complete object type";
				return false;
			}
		}
		// perform promotions on vararg arguments
		if (is_vararg)
		{
			for (size_t i = num_parameters; i < num_arguments; ++i)
			{
				XprNode *arg_xpr = get_subxpr(i + 1);
				Type const &arg_type = arg_xpr->get_xpr_type();
				if (arg_type.is_integer())
					set_subxpr(i + 1, conditional_cast(arg_xpr, arg_type.integer_promote()));
				if (arg_type == Type::float_type())
					set_subxpr(i + 1, conditional_cast(arg_xpr, Type::double_type()));
			}
		}
		set_xpr_type(return_type);
		return true;
	}
	case Id::ARRAY_SUBSCRIPT:
	{
		/*
			One of the expressions shall have type ``pointer to object type ,''
			the other expression shall have integral type,
			and the result has type `` type .''
		*/
		Type const &second_type = get_subxpr(1)->get_xpr_type();
		if (first_type.is_pointer() && first_type.referenced_type().is_complete_object() && second_type.is_integer())
		{
			set_xpr_type(first_type.referenced_type());
			return true;
		}
		if (first_type.is_array() && second_type.is_integer())
		{
			set_xpr_type(first_type.element_type());
			return true;
		}
		if (second_type.is_pointer() && second_type.referenced_type().is_complete_object() && first_type.is_integer())
		{
			set_xpr_type(second_type.referenced_type());
			return true;
		}
		if (second_type.is_array() && first_type.is_integer())
		{
			set_xpr_type(second_type.element_type());
			return true;
		}
		std::cerr << "One argument of an array subscript expression should be a pointer to complete object or an array, the other should be an integer" << std::endl;
		return false;
	}
	case Id::STRUCTURE_PTR_MEMBER:
	{
		/*
			The first operand of the -> operator shall have type ``pointer to qualified or unqualified structure''
			or ``pointer to qualified or unqualified union,'' and the second operand shall name a member of the type pointed to.
		*/
		if (!first_type.is_pointer() || !first_type.referenced_type().is_structure())
		{
			std::cerr << "Left hand side of the structure member operator shall be pointer to a structure type";
			return false;
		}
		Type const &structure_type = first_type.referenced_type();
		IdentifierXprNode *second_xpr = static_cast<IdentifierXprNode *>(get_subxpr(1));
		auto field_name = second_xpr->get_identifier();
		Type const *member_type = structure_type.lookup_structure_field_type(field_name);
		if (member_type == nullptr)
		{
			std::cerr << "Could not find field member " + field_name + " in structure type";
			return false;
		}
		second_xpr->set_xpr_type(*member_type);
		set_xpr_type(*member_type);
		return true;
	}
	case Id::STRUCTURE_MEMBER:
	{
		/*
			The first operand of the . operator shall have a qualified or unqualified structure or union type,
			and the second operand shall name a member of that type.
		*/
		if (!first_type.is_structure())
		{
			std::cerr << "Left hand side of the structure member operator shall be a structure type";
			return false;
		}
		Type const &structure_type = first_type;
		IdentifierXprNode *second_xpr = static_cast<IdentifierXprNode *>(get_subxpr(1));
		auto field_name = second_xpr->get_identifier();
		Type const *member_type = structure_type.lookup_structure_field_type(field_name);
		if (member_type == nullptr)
		{
			std::cerr << "Could not find field member " + field_name + " in structure type";
			return false;
		}
		second_xpr->set_xpr_type(*member_type);
		set_xpr_type(*member_type);
		return true;
	}
		throw __FILE__ ": Unimplemented type check for postfix expression";
	}

	return true;
}
