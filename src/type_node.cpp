#include "type_node.h"

#include "declaration.h"
#include <stdexcept>

size_t TypeNode::size_in_bytes(Id arg)
{
	switch (arg)
	{
	case VOID:
	case FUNCTION:
		return 0;
	case CHAR:
	case SCHAR:
	case UCHAR:
		return 1;
	case SHORT:
	case USHORT:
		return 2;
	case INT:
	case UINT:
		return 4;
	case LONG:
	case ULONG:
	case LLONG:
	case ULLONG:
	case FLOAT:
	case DOUBLE:
	case POINTER:
		return 8;
	default:
		throw __FILE__ ": size_in_bytes called for invalid byte id";
	}
}

TypeNode::Id TypeNode::to_signed_integer(Id arg)
{
	switch (arg)
	{
	case SCHAR:
	case UCHAR:
		return SCHAR;
	case SHORT:
	case USHORT:
		return SHORT;
	case INT:
	case UINT:
		return INT;
	case LONG:
	case ULONG:
		return LONG;
	case LLONG:
	case ULLONG:
		return LLONG;
	default:
		throw __FILE__ ": to_signed_integer called for invalid byte id";
	}
}

bool TypeNode::can_represent(Id a, Id b)
{
	if (a == b)
		return true;
	if (is_signed_integer(a))
	{
		if (is_signed_integer(b))
			return size_in_bytes(b) <= size_in_bytes(a);
		else if (is_unsigned_integer(b))
			return size_in_bytes(b) < size_in_bytes(a);
		else if (is_integer(b)) // CHAR is signed in our implementation
			return true;
	}
	else if (is_unsigned_integer(a))
	{
		if (is_unsigned_integer(b))
			return size_in_bytes(b) <= size_in_bytes(a);
		else if (is_signed_integer(b))
			return size_in_bytes(b) < size_in_bytes(a);
		else if (is_integer(b)) // CHAR is signed in our implementation
			return size_in_bytes(b) < size_in_bytes(a);
	}
	throw __FILE__ ": unhandled representation case";
}

TypeNode::Id TypeNode::integer_promote(Id arg)
{
	if (can_represent(INT, arg))
		return INT;
	if (can_represent(UINT, arg))
		return UINT;
	return arg;
}

void TypeNode::print(std::ostream &os, size_t level) const
{
	switch (m_id)
	{
	case VOID:
		os << "void";
		break;
	case CHAR:
		os << "char";
		break;
	case SCHAR:
		os << "signed char";
		break;
	case UCHAR:
		os << "unsigned char";
		break;
	case SHORT:
		os << "short";
		break;
	case USHORT:
		os << "unsigned short";
		break;
	case INT:
		os << "int";
		break;
	case UINT:
		os << "unsigned";
		break;
	case LONG:
		os << "long";
		break;
	case ULONG:
		os << "unsigned long";
		break;
	case LLONG:
		os << "long long";
		break;
	case ULLONG:
		os << "unsigned long long";
		break;
	case DOUBLE:
		os << "double";
		break;
	case FLOAT:
		os << "float";
		break;
	case ARRAY:
		os << "array of " << this->m_size << " ";
		break;
	case POINTER:
		os << "pointer to ";
		break;
	case FUNCTION:
		os << "function (";
		for (size_t i = 0; i < m_n_declarations; ++i)
		{
			Declaration const &d = m_declarations[i];
			if (d.get_identifier().size() != 0)
				os << d.get_identifier() << ": ";
			os << d.get_type() << (i == m_n_declarations - 1 ? "" : ", ");
		}
		os << ") returning ";
		break;
	case ENUM:
		os << "enum " << m_tag;
		if (is_incomplete())
			os << "(incomplete)";
		else
		{
			os << "{";
			for (auto const &c : m_enum_constants)
				os << c.first << "=" << c.second << ", ";
			os << "}";
		}
		break;
	case STRUCT:
		os << "struct " << m_tag;
		if (is_incomplete())
			os << "(incomplete)";
		else
		{
			os << "{";
			for (size_t i = 0; i < m_n_declarations; ++i)
				os << m_declarations[i].get_identifier() << "; ";
			os << "}";
		}
		break;
	default:
		throw __FILE__ ": Did not define how to write type argument";
	}
}

void TypeNode::add_declarations(Declarations const &dl)
{
	m_n_declarations = dl.size();
	m_declarations = new Declaration[m_n_declarations];
	size_t i = 0;
	for (auto d : dl)
		m_declarations[i++] = d;
}

TypeNode const &TypeNode::operator=(TypeNode const &other)
{
	if (this == &other)
		return *this;
	m_id = other.m_id;
	m_size = other.m_size;
	m_alignment = other.m_alignment;
	delete[] m_declarations;
	m_declarations = nullptr;
	m_n_declarations = other.m_n_declarations;
	if (m_n_declarations > 0)
	{
		m_declarations = new Declaration[m_n_declarations];
		for (size_t i = 0; i < m_n_declarations; ++i)
			m_declarations[i] = other.m_declarations[i];
	}
	m_is_vararg = other.m_is_vararg;
	m_tag = other.m_tag;

	return *this;
}

TypeNode::Id TypeNode::common_real_type(Id lhs, Id rhs)
{
	if (!is_arithmetic(lhs) || !is_arithmetic(rhs))
		throw __FILE__ ": Common real type is only defined if both operands are arithmetic.";

	if (lhs == LDOUBLE || rhs == LDOUBLE)
		return LDOUBLE;
	if (lhs == DOUBLE || rhs == DOUBLE)
		return DOUBLE;
	if (lhs == FLOAT || rhs == FLOAT)
		return FLOAT;

	lhs = integer_promote(lhs);
	rhs = integer_promote(rhs);

	int lrank = integer_conversion_rank(lhs);
	int rrank = integer_conversion_rank(rhs);

	if (is_signed_integer(lhs) && is_signed_integer(rhs) || is_unsigned_integer(lhs) && is_unsigned_integer(rhs))
		return lrank > rrank ? lhs : rhs;
	if (is_unsigned_integer(lhs) && lrank >= rrank)
		return lhs;
	if (is_unsigned_integer(rhs) && rrank >= lrank)
		return rhs;

	if (is_signed_integer(lhs) && can_represent(lhs, rhs))
		return lhs;
	if (is_signed_integer(rhs) && can_represent(rhs, lhs))
		return rhs;

	if (is_signed_integer(lhs))
		return to_signed_integer(rhs);
	return to_signed_integer(lhs);
}

int TypeNode::integer_conversion_rank(Id arg)
{
	switch (arg)
	{
	case CHAR:
	case UCHAR:
	case SCHAR:
		return 1;
	case SHORT:
	case USHORT:
		return 2;
	case INT:
	case UINT:
		return 3;
	case LONG:
	case ULONG:
		return 4;
	case LLONG:
	case ULLONG:
		return 5;
	}
	throw __FILE__ ": Could not define integer_conversion_rank.";
}

Declaration const &TypeNode::get_declaration(size_t i) const
{
	if (i >= m_n_declarations)
		throw std::out_of_range("Overindexing list of declarations");
	return m_declarations[i];
}

TypeNode::~TypeNode()
{
	delete[] m_declarations;
}

bool TypeNode::operator==(TypeNode const &other) const
{
	// TODO this implementation compares two function types to true if their declarations are equal w.r.t. identifiers
	if (m_id != other.m_id)
		return false;
	if (m_id == ARRAY && m_size != other.m_size)
		return false;
	if (m_id == FUNCTION)
	{
		if (m_is_vararg != other.m_is_vararg || m_n_declarations != other.m_n_declarations)
			return false;
		for (size_t i = 0; i < m_n_declarations; ++i)
		{
			if (m_declarations[i].get_type() != other.m_declarations[i].get_type())
				return false;
			if (m_declarations[i].get_identifier() != other.m_declarations[i].get_identifier())
				return false;
		}
	}
	if (m_id == ENUM && m_tag != other.m_tag)
		return false;
	return true;
}

void TypeNode::add_field(std::string const &id, Type const &type)
{
	if (get_id() != STRUCT)
		throw __FILE__ ": Fields can only be added to structures";
	Declaration *tmp = new Declaration[m_n_declarations + 1];
	for (size_t i = 0; i < m_n_declarations; ++i)
		tmp[i] = m_declarations[i];
	tmp[m_n_declarations] = Declaration(type, id);
	m_n_declarations++;
	delete[] m_declarations;
	m_declarations = tmp;

	// compute new size taking alignments into account
	size_t al = type.get_alignment_in_bytes();
	m_alignment = std::max(m_alignment, al);
	size_t offset = m_size;
	if (offset % al != 0)
		offset = (offset / al) * al + al;
	size_t siz = offset + type.get_size_in_bytes();
	if (siz % m_alignment != 0)
		siz = siz / m_alignment * m_alignment + m_alignment;
	m_size = siz;
}

Type const *TypeNode::lookup_structure_field_type(std::string const &id) const
{
	if (get_id() != STRUCT)
		throw __FILE__ ": Structure field lookup only valid for structure nodes";
	for (size_t i = 0; i < m_n_declarations; ++i)
		if (m_declarations[i].get_identifier() == id)
			return &m_declarations[i].get_type();
	return nullptr;
}

size_t TypeNode::lookup_structure_field_offset(std::string const &id) const
{
	if (get_id() != STRUCT)
		throw __FILE__ ": Structure offset lookup only valid for structure nodes";
	size_t offset = 0;
	for (size_t i = 0; i < m_n_declarations; ++i)
	{
		Type const &t = m_declarations[i].get_type();
		size_t al = t.get_alignment_in_bytes();
		if (offset % al != 0)
			offset = offset / al * al + al;
		if (m_declarations[i].get_identifier() == id)
			return offset;
		offset += t.get_size_in_bytes();
	}
	throw __FILE__ ": Could not find field in structure";
}
