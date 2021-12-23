#include "type.h"
#include "declaration.h"

std::vector<TypeTreeNode *> Type::m_pool;

size_t Type::m_tag_cntr = 0;

Type Type::function_returning(TypeNode::Declarations const &ptl, bool is_vararg) const
{
	if (is_array())
		throw __FILE__ ": Function returning array is invalid";
	TypeNode tn(TypeNode::FUNCTION);
	tn.add_declarations(ptl);
	tn.set_vararg(is_vararg);
	return attach(tn);
}

Type Type::referenced_type() const
{
	if (!is_pointer())
		throw __FILE__ ": Referenced type only valid for pointers.";
	return Type(m_ptr->get_arg());
}

Type Type::element_type() const
{
	if (!is_array())
		throw __FILE__ ": Element type only valid for arrays.";
	return Type(m_ptr->get_arg());
}

Type Type::return_type() const
{
	if (!is_function())
		throw __FILE__ ": Return type only valid for functions.";
	return Type(m_ptr->get_arg());
}

Type Type::integer_promote() const
{
	if (!is_integer())
		throw __FILE__ ": Only integers can be integer promoted.";
	return install(TypeNode(TypeNode::integer_promote(m_ptr->get_node().get_id())));
}

Type Type::attach(TypeNode const &node) const
{
	for (auto c : m_ptr->get_children())
		if (c->get_node() == node)
			return c;
	return new TypeTreeNode(node, m_ptr);
}

Type Type::replace_back_to(Type const &other) const
{
	if (m_ptr->get_arg() == nullptr)
		return other;
	Type t = Type(m_ptr->get_arg()).replace_back_to(other);
	return t.attach(m_ptr->get_node());
}

Type Type::first_empty_array_to_pointer() const
{
	if (m_ptr->get_arg() == nullptr) // root elements cannot be arrays
		return *this;
	if (is_array() && m_ptr->get_node().get_size() == 0)
		return Type(m_ptr->get_arg()).attach(TypeNode(TypeNode::POINTER));
	return Type(m_ptr->get_arg()).first_empty_array_to_pointer().attach(m_ptr->get_node());
}

void Type::print(std::ostream &os, size_t level) const
{
	if (m_ptr == nullptr)
		return;
	m_ptr->get_node().print(os);
	Type(m_ptr->get_arg()).print(os);
}

bool Type::is_compatible_with(Type const &other) const
{
	if (*this == other)
		return true;
	if (is_pointer() && other.is_pointer() && referenced_type().is_compatible_with(other.referenced_type()))
		return true;
	if (is_array() && other.is_array() && element_type().is_compatible_with(other.element_type()) && m_ptr->get_node().get_size() == other.m_ptr->get_node().get_size())
		return true;
	return false;
}

size_t Type::get_size_in_bytes() const
{
	size_t s = 1;
	auto it = m_ptr;
	for (; it != nullptr && TypeNode::is_array(it->get_node().get_id()); it = it->get_arg())
		s *= it->get_node().get_size();
	return s * it->get_node().get_size();
}

size_t Type::get_alignment_in_bytes() const
{
	if (is_array())
		return element_type().get_alignment_in_bytes();
	return m_ptr->get_node().get_alignment();
}

Type Type::install(TypeNode const &tn, bool as_new)
{
	if (!as_new)
	{
		for (auto c : m_pool)
			if (c->get_node() == tn)
				return c;
	}
	TypeTreeNode *c = new TypeTreeNode(tn);
	if (as_new && tn.get_id() == TypeNode::ENUM)
		c->get_node().set_tag(c->get_node().get_tag() + ":" + std::to_string(++m_tag_cntr));
	m_pool.push_back(c);
	return c;
}

Type Type::common_real_type(Type const &other) const
{
	TypeNode::Id common_id = TypeNode::common_real_type(m_ptr->get_node().get_id(), other.m_ptr->get_node().get_id());
	return install(TypeNode(common_id));
}

size_t Type::get_num_declarations() const
{
	if (!is_function() && !is_structure())
		throw __FILE__ ": Requesting declarations from a non-function or non-structure type";
	return m_ptr->get_node().get_num_declarations();
}

Declaration const &Type::get_declaration(size_t i) const
{
	if (!is_function() && !is_structure())
		throw __FILE__ ": Requesting declarations from a non-function or non-structure type";
	return m_ptr->get_node().get_declaration(i);
}

int Type::print_rec(TypeTreeNode *n, std::ostream &os)
{
	int label = ++m_label;
	os << "L" << label << " [ shape=box, label =\"" << n->get_node() << "\" ]" << std::endl;
	for (auto c : n->get_children())
	{
		int clab = print_rec(c, os);
		os << "L" << clab << " -> L" << label << std::endl;
	}
	return label;
}

void Type::print_pool(std::ostream &os)
{
	m_label = 0;
	os << "digraph G {" << std::endl;
	os << "graph [ rankdir=LR ]" << std::endl;
	for (auto c : m_pool)
		print_rec(c, os);
	os << "}" << std::endl;
}

int Type::m_label;