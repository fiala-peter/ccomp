#ifndef REGISTER_ALLOCATOR_H_INCLUDED
#define REGISTER_ALLOCATOR_H_INCLUDED

#include "register.h"

#include <iostream>
#include <string>
#include <vector>

class RegisterAllocator
{
public:
	void reset()
	{
		for (size_t i = 0; i < m_registers.size(); i++)
			m_registers[i].m_reserved = false;
	}

	Register allocate(Register::Type type = Register::Type::INTEGER);

	Register allocate(Register::Id id);

	void release(Register const &reg);

	RegisterAllocator();

	void print_state(std::ostream &os) const;

	bool is_reserved(Register::Id reg) const;

private:
	struct Data
	{
		Data(Register::Id id, int res = false) : m_id(id), m_reserved(res) {}
		Register::Id m_id;
		bool m_reserved;
	};

	std::vector<Data> m_registers;
};

#endif
