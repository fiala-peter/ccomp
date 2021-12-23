#include "operator.h"

bool BinaryPlusOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	/*
		either both operands shall have arithmetic type, or one operand shall be a pointer to an object type and the other shall have integral type.
	*/
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = *ret = lhs->common_real_type(*rhs);
		return true;
	}
	if (lhs->is_pointer() && lhs->referenced_type().is_complete_object() && rhs->is_integer())
	{
		*ret = *lhs;
		return true;
	}
	if (rhs->is_pointer() && rhs->referenced_type().is_complete_object() && lhs->is_integer())
	{
		*ret = *rhs;
		return true;
	}
	std::cerr << "Operands should be arithm+arithm or ptr+intgr or intgr+ptr" << std::endl;
	return false;
}

bool BinaryMinusOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	/*
		- both operands have arithmetic type;
		- both operands are pointers to qualified or unqualified versions of compatible object types; or
		- the left operand is a pointer to an object type and the right operand has integral type
	*/	
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = *ret = lhs->common_real_type(*rhs);
		return true;
	}
	if (lhs->is_pointer() && lhs->referenced_type().is_complete_object() && rhs->is_integer())
	{
		*ret = *lhs;
		return true;
	}
	if (lhs->is_pointer() && lhs->referenced_type().is_complete_object() && 
	rhs->is_pointer() && rhs->referenced_type().is_complete_object()
	&& lhs->unqualified().is_compatible_with(rhs->unqualified()))
	{
		// todo this should be ptrdiff_t
		*ret = Type::llong_type();
		return true;
	}
	std::cerr << "Operands should be arithm-arithm or ptr-intgr or ptr-ptr" << std::endl;
	return false;
}

bool BinaryTimesOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	/*
		Each of the operands shall have arithmetic type
	*/
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = *ret = lhs->common_real_type(*rhs);
		return true;
	}
	std::cerr << "Both operands should be arithmetic" << std::endl;
	return false;
}

bool BinaryDivOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	/*
		Each of the operands shall have arithmetic type
	*/
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = *ret = lhs->common_real_type(*rhs);
		return true;
	}
	std::cerr << "Both operands should be arithmetic" << std::endl;
	return false;
}

bool BinaryModOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	/*
		Each of the operands shall have integer type
	*/
	if (lhs->is_integer() && rhs->is_integer())
	{
		*lhs = *rhs = *ret = lhs->common_real_type(*rhs);
		return true;
	}
	std::cerr << "Both operands should be integers" << std::endl;
	return false;
}

bool BinaryLessOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = lhs->common_real_type(*rhs);
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && rhs->is_pointer() && lhs->referenced_type().is_compatible_with(rhs->referenced_type()))
	{
		*ret = Type::int_type();
		return true;
	}
	std::cerr << "Operands should be arithm<arithm or ptr<ptr" << std::endl;
	return false;
}

bool BinaryNeqOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = lhs->common_real_type(*rhs);
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && rhs->is_pointer() && lhs->referenced_type().is_compatible_with(rhs->referenced_type()))
	{
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && lhs->referenced_type().is_object() && rhs->is_pointer() && rhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	if (rhs->is_pointer() && rhs->referenced_type().is_object() && lhs->is_pointer() && lhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	// TODO nullpointer constant is an expression not a type
	// if (lhs->is_pointer() && rhs->is_nullpointer_constant())
	// {
	// 	*ret = Type::int_type();
	// 	return true;
	// }

	std::cerr << "Operands should be arithm!=arithm or ptr!=ptr" << std::endl;
	return false;
}

bool BinaryEqOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = lhs->common_real_type(*rhs);
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && rhs->is_pointer() && lhs->referenced_type().is_compatible_with(rhs->referenced_type()))
	{
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && lhs->referenced_type().is_object() && rhs->is_pointer() && rhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	if (rhs->is_pointer() && rhs->referenced_type().is_object() && lhs->is_pointer() && lhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	// if (lhs->is_pointer() && rhs->is_nullpointer_constant())
	// {
	// 	*ret = Type::int_type();
	// 	return true;
	// }

	std::cerr << "Operands should be arithm==arithm or ptr==ptr" << std::endl;
	return false;
}

bool BinaryGreaterOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = lhs->common_real_type(*rhs);
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && rhs->is_pointer() && lhs->referenced_type().is_compatible_with(rhs->referenced_type()))
	{
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && lhs->referenced_type().is_object() && rhs->is_pointer() && rhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	if (rhs->is_pointer() && rhs->referenced_type().is_object() && lhs->is_pointer() && lhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	// if (lhs->is_pointer() && rhs->is_nullpointer_constant())
	// {
	// 	*ret = Type::int_type();
	// 	return true;
	// }

	std::cerr << "Operands should be arithm>arithm or ptr>ptr" << std::endl;
	return false;
}

bool BinaryGreaterEqualOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = lhs->common_real_type(*rhs);
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && rhs->is_pointer() && lhs->referenced_type().is_compatible_with(rhs->referenced_type()))
	{
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && lhs->referenced_type().is_object() && rhs->is_pointer() && rhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	if (rhs->is_pointer() && rhs->referenced_type().is_object() && lhs->is_pointer() && lhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	// if (lhs->is_pointer() && rhs->is_nullpointer_constant())
	// {
	// 	*ret = Type::int_type();
	// 	return true;
	// }

	std::cerr << "Operands should be arithm>=arithm or ptr>=ptr" << std::endl;
	return false;
}

bool BinaryLessEqualOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	if (lhs->is_arithmetic() && rhs->is_arithmetic())
	{
		*lhs = *rhs = lhs->common_real_type(*rhs);
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && rhs->is_pointer() && lhs->referenced_type().is_compatible_with(rhs->referenced_type()))
	{
		*ret = Type::int_type();
		return true;
	}
	if (lhs->is_pointer() && lhs->referenced_type().is_object() && rhs->is_pointer() && rhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	if (rhs->is_pointer() && rhs->referenced_type().is_object() && lhs->is_pointer() && lhs->referenced_type().unqualified() == Type::void_type())
	{
		*ret = Type::int_type();
		return true;
	}
	// if (lhs->is_pointer() && rhs->is_nullpointer_constant())
	// {
	// 	*ret = Type::int_type();
	// 	return true;
	// }

	std::cerr << "Operands should be arithm<=arithm or ptr<=ptr" << std::endl;
	return false;
}


bool BinaryLogicalAndOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	if (lhs->is_scalar() && rhs->is_scalar())
	{
		*ret = Type::int_type();
		return true;
	}
	std::cerr << "Operands should be scalars" << std::endl;
	return false;
}

bool BinaryLogicalOrOperator::type_check(Type *lhs, Type *rhs, Type *ret) const
{
	if (lhs->is_scalar() && rhs->is_scalar())
	{
		*ret = Type::int_type();
		return true;
	}
	std::cerr << "Operands should be scalars" << std::endl;
	return false;
}
