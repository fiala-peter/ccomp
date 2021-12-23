/**
 * @file integer_constant.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class IntegerConstant
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef INTEGER_CONSTANT_H_DEFINED
#define INTEGER_CONSTANT_H_DEFINED

#include "xpr_node.h"

/** @brief Class representing compile time integer constants */
class IntegerConstant
	: public XprNode
{
public:
	/** @brief constructor */
	IntegerConstant() : XprNode(XprNode::Id::INTEGER_XPR) {}

	/**
	 * @brief Construct a new Integer Constant object
	 * 
	 * @param type The constant's type
	 * @param value The constant's value
	 */
	IntegerConstant(Type const &type, void const *val = nullptr)
		: XprNode(XprNode::Id::INTEGER_XPR)
	{
		set_xpr_type(type);
		if (val != nullptr)
			set_value(val);
	}

	/** @brief set the value */
	void set_value(void const *val);

	IntegerConstant cast(Type const &t)
	{
		IntegerConstant ret = *this;
		ret.set_xpr_type(t);
		return ret;
	}

	/** @brief indicate if it is a constant expression */
	bool is_constant_expression() const override { return true; };

	void print(std::ostream &os, size_t level = 0) const override;

	IntegerConstant evaluate_constant() const override { return *this; }

	XprNode *clone() override
	{
		return new IntegerConstant(*this);
	}

	IntegerConstant operator+(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant res(t);
		if (t == Type::int_type())
			res.m_value.Int = m_value.Int + rhs.m_value.Int;
		else if (t == Type::ullong_type())
			res.m_value.Int = m_value.ULLong + rhs.m_value.ULLong;
		else
			throw __FILE__ ": unprocessed operator + type";
		return res;
	}

	IntegerConstant operator-(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant res(t);
		if (t == Type::int_type())
			res.m_value.Int = m_value.Int - rhs.m_value.Int;
		else
			throw __FILE__ ": unprocessed operator - type";
		return res;
	}

	IntegerConstant operator*(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant res(t);
		if (t == Type::int_type())
			res.m_value.Int = m_value.Int * rhs.m_value.Int;
		else if (t == Type::ullong_type())
			res.m_value.Int = m_value.ULLong * rhs.m_value.ULLong;
		else
			throw __FILE__ ": unprocessed operator * type";
		return res;
	}

	IntegerConstant operator/(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant res(t);
		if (t == Type::int_type())
			res.m_value.Int = m_value.Int / rhs.m_value.Int;
		else if (t == Type::ullong_type())
			res.m_value.ULLong = m_value.ULLong / rhs.m_value.ULLong;
		else
			throw __FILE__ ": unprocessed operator / type";
		return res;
	}

	IntegerConstant operator%(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant res(t);
		if (t == Type::int_type())
			res.m_value.Int = m_value.Int % rhs.m_value.Int;
		else
			throw __FILE__ ": unprocessed operator % type";
		return res;
	}

	IntegerConstant operator<(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant ret(Type::int_type());
		if (t == Type::int_type())
			ret.m_value.Int = m_value.Int < rhs.m_value.Int;
		else
			throw __FILE__ ": unprocessed operator < type";
		return ret;
	}

	IntegerConstant operator<=(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant ret(Type::int_type());
		if (t == Type::int_type())
			ret.m_value.Int = m_value.Int <= rhs.m_value.Int;
		else
			throw __FILE__ ": unprocessed operator <= type";
		return ret;
	}

	IntegerConstant operator!=(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant ret(Type::int_type());
		if (t == Type::int_type())
			ret.m_value.Int = m_value.Int != rhs.m_value.Int;
		else
			throw __FILE__ ": unprocessed operator != type";
		return ret;
	}

	IntegerConstant operator==(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant ret(Type::int_type());
		if (t == Type::int_type())
			ret.m_value.Int = m_value.Int == rhs.m_value.Int;
		else
			throw __FILE__ ": unprocessed operator == type";
		return ret;
	}

	IntegerConstant operator>(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant ret(Type::int_type());
		if (t == Type::int_type())
			ret.m_value.Int = m_value.Int > rhs.m_value.Int;
		else
			throw __FILE__ ": unprocessed operator > type";
		return ret;
	}

	IntegerConstant operator>=(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant ret(Type::int_type());
		if (t == Type::int_type())
			ret.m_value.Int = m_value.Int >= rhs.m_value.Int;
		else
			throw __FILE__ ": unprocessed operator <= type";
		return ret;
	}

	IntegerConstant operator&&(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant ret(Type::int_type());
		if (t == Type::int_type())
			ret.m_value.Int = m_value.Int && rhs.m_value.Int;
		else throw __FILE__ ": unprocessed operator && type";
		return ret;
	}

	IntegerConstant operator||(IntegerConstant const &rhs) const
	{
		Type const &t = get_xpr_type();
		if (rhs.get_xpr_type() != t)
			throw __FILE__ " Cannot add integer constants of different type";
		IntegerConstant ret(Type::int_type());
		if (t == Type::int_type())
			ret.m_value.Int = m_value.Int || rhs.m_value.Int;
		else throw __FILE__ ": unprocessed operator || type";
			return ret;
	}

	template <class Integer>
	operator Integer() const
	{
		Type const &t = get_xpr_type();
		if (t == Type::int_type())
			return m_value.Int;
		if (t == Type::ulong_type())
			return m_value.ULong;
		if (t == Type::ullong_type())
			return m_value.ULLong;
		throw __FILE__ ": Unprocessed type branch";
	}

private:
	union Value
	{
		char Char;
		signed char SChar;
		unsigned char UChar;
		short int Short;
		unsigned short int UShort;
		int Int;
		unsigned int UInt;
		long int Long;
		unsigned long int ULong;
		long long int LLong;
		unsigned long long int ULLong;
	} m_value;
};

#endif // INTEGER_CONSTANT_H_DEFINED
