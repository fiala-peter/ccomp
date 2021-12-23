#ifndef OPERATOR_H_INCLUDED
#define OPERATOR_H_INCLUDED

#include "type.h"
#include "integer_constant.h"

class Operator
{
public:
	virtual ~Operator() {}
};

class UnaryOperator : public Operator
{
public:
	virtual bool type_check(Type *arg, Type *ret) const
	{
		return false;
	}
};

class BinaryOperator : public Operator
{
public:
	virtual IntegerConstant operator()(IntegerConstant const &lhs, IntegerConstant const &rhs) const = 0;
	virtual bool type_check(Type *lhs, Type *rhs, Type *ret) const = 0;
};

class TernaryOperator : public Operator
{
public:
	virtual bool type_check(Type *arg1, Type *arg2, Type *arg3, Type *ret) const
	{
		return false;
	}
};

#define DECLARE_BINARY_OPERATOR(x, op)                                    \
	struct x : public BinaryOperator                                      \
	{                                                                     \
		IntegerConstant operator()(IntegerConstant const &lhs, IntegerConstant const &rhs) const override \
		{                                                                 \
			return lhs op rhs;                                            \
		}                                                                 \
		bool type_check(Type *lhs, Type *rhs, Type *ret) const override;  \
	}

DECLARE_BINARY_OPERATOR(BinaryPlusOperator, +);
DECLARE_BINARY_OPERATOR(BinaryMinusOperator, -);
DECLARE_BINARY_OPERATOR(BinaryTimesOperator, *);
DECLARE_BINARY_OPERATOR(BinaryDivOperator, /);
DECLARE_BINARY_OPERATOR(BinaryModOperator, %);
DECLARE_BINARY_OPERATOR(BinaryLessOperator, <);
DECLARE_BINARY_OPERATOR(BinaryLessEqualOperator, <=);
DECLARE_BINARY_OPERATOR(BinaryNeqOperator, !=);
DECLARE_BINARY_OPERATOR(BinaryEqOperator, ==);
DECLARE_BINARY_OPERATOR(BinaryGreaterOperator, >);
DECLARE_BINARY_OPERATOR(BinaryGreaterEqualOperator, >=);
DECLARE_BINARY_OPERATOR(BinaryLogicalAndOperator, &&);
DECLARE_BINARY_OPERATOR(BinaryLogicalOrOperator, ||);

struct BinaryCommaOperator : public BinaryOperator
{
	IntegerConstant operator()(IntegerConstant const &lhs, IntegerConstant const &rhs) const override
	{
		return rhs;
	}

	bool type_check(Type *lhs, Type *rhs, Type *ret) const override
	{
		*ret = *rhs;
		return true;
	}
};

#endif
