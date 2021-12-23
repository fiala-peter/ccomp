#ifndef LOCAL_TABLE_H_INCLUDED
#define LOCAL_TABLE_H_INCLUDED

#include <list>
#include <utility>
#include <string>

class LocalTableEntry
{
public:
	LocalTableEntry(std::string const &str, size_t offset, int scope)
		: m_id(str), m_offset(offset), m_scope(scope)
	{
	}

	int get_scope() const { return m_scope; }

	size_t get_offset() const { return m_offset; }

	std::string const &get_id() const { return m_id; }

private:
	std::string m_id;
	size_t m_offset;
	int m_scope;
};

class LocalTable : private std::list<LocalTableEntry>
{
public:
	void push(int scope, std::string const &var, size_t size, size_t align);
	
	void pop() { pop_front(); }

	std::string lookup(std::string const &id) const;

	size_t offset(std::string const &id) const;

	size_t get_size() const { return empty() ? 0 : front().get_offset(); }

	size_t get_size_at_scope(int scope) const
	{
		for (auto it = begin(); it != end(); ++it)
			if (it->get_scope() == scope)
				return it->get_offset();
		return 0;
	}

	LocalTable() {};
	
	void purge(int scope_counter)
	{
		while (!empty() && front().get_scope() == scope_counter)
			pop_front();
	}
};

#endif
