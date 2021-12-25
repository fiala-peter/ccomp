#include "integer_constant.h"

void IntegerConstant::set_value(void const *v)
{
	Type const &t = get_xpr_type();
	if (t == Type::char_type())
		m_value.Char = *reinterpret_cast<char const *>(v);
	else if (t == Type::int_type())
		m_value.Int = *reinterpret_cast<int const *>(v);
	else if (t == Type::long_type())
		m_value.Long = *reinterpret_cast<long const *>(v);
	else if (t == Type::ulong_type())
		m_value.ULong = *reinterpret_cast<unsigned long const *>(v);
	else if (t == Type::llong_type())
		m_value.LLong = *reinterpret_cast<long long const *>(v);
	else if (t == Type::ullong_type())
		m_value.ULLong = *reinterpret_cast<unsigned long long const *>(v);
	else
		throw __FILE__ ": Unprocessed constant type";
}

void IntegerConstant::print(std::ostream &os, size_t level) const
{
	for (size_t i = 0; i < level; i++)
		os << ((i == level - 1) ? "+---" : "    ");

	os << "INTEGER_CONSTANT"
	   << " Type: " << get_xpr_type() << ", Value: " << m_value.Int << std::endl;

	for (auto c : get_subxprs())
		c->print(os, level + 1);
}
