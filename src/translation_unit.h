#ifndef TRANSLATION_UNIT_H_INCLUDED
#define TRANSLATION_UNIT_H_INCLUDED

#include "function_node.h"
#include "serializable.h"
#include "symbol_table_owner.h"

#include <algorithm>
#include <vector>

class TransUnitNode
	: public Serializable
	, public SymbolTableOwner
{
public:
	TransUnitNode(){}

	~TransUnitNode()
	{
		for (auto f : m_functions)
			delete f;
	}

	TransUnitNode(TransUnitNode const &other) = delete;

	TransUnitNode const &operator=(TransUnitNode const &other) = delete;

	void add_function(FunctionNode *func) { m_functions.push_back(func); }

	std::vector<FunctionNode *> const &get_functions() const { return m_functions; }

	void print(std::ostream &os, size_t level = 0) const override;
	
	void constant_fold()
	{
		for (auto f : m_functions)
			f->constant_fold();
	}

	void add_string_literal(std::string const &str)
	{
		if (std::find(m_string_literals.begin(), m_string_literals.end(), str) == m_string_literals.end())
			m_string_literals.push_back(str);
	}

	void add_floating_constant(double d)
	{
		if (std::find(m_floating_constants.begin(), m_floating_constants.end(), d) == m_floating_constants.end())
			m_floating_constants.push_back(d);
	}

	std::vector<std::string> const &get_string_literals() const { return m_string_literals; }

	std::vector<double> const &get_floating_constants() const { return m_floating_constants; }

private:
	std::vector<std::string> m_string_literals;
	std::vector<double> m_floating_constants;
	std::vector<FunctionNode *> m_functions;
};

#endif
