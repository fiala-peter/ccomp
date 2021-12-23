#include "local_table.h"

void LocalTable::push(int scope, std::string const &var, size_t size, size_t align)
{
	if (scope == 0)
	{
		push_front(LocalTableEntry(var, 0, scope));
		return;
	}
	size_t offset = empty() ? 0 : front().get_offset();
	offset += size;
	if (offset % align != 0)
		offset = (offset / align + 1) * align;
	push_front(LocalTableEntry(var, offset, scope));
}

size_t LocalTable::offset(std::string const &id) const
{
	for (auto s : *this)
		if (s.get_id() == id)
			return s.get_offset();
	throw __FILE__ ": variable not found in local table";
}

std::string LocalTable::lookup(std::string const &id) const
{
	for (auto s : *this)
		if (s.get_id() == id)
			if (s.get_scope() == 0) // global
				return id + "(%rip)";
			else // local
				return std::to_string(-(long long)s.get_offset()) + "(%rbp)";
	throw __FILE__ ": variable not found in local table";
}
