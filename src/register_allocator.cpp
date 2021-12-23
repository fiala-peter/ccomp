#include "register_allocator.h"

Register RegisterAllocator::allocate(Register::Type type)
{
	for (size_t i = 0; i < m_registers.size(); i++)
	{
		if (Register::get_type(m_registers[i].m_id) != type)
			continue;
		if (!m_registers[i].m_reserved)
		{
			m_registers[i].m_reserved = true;
			return m_registers[i].m_id;
		}
	}
	throw __FILE__ ": No free registers found!";
}

Register RegisterAllocator::allocate(Register::Id id)
{
	for (size_t i = 0; i < m_registers.size(); ++i)
	{
		if (m_registers[i].m_id != id)
			continue;
		if (m_registers[i].m_reserved)
			throw __FILE__ ":Trying to allocate a reserved register";
		m_registers[i].m_reserved = true;
		return m_registers[i].m_id;
	}
	throw __FILE__ ": The requested register is not contained in the allocator";
}

void RegisterAllocator::release(Register const &reg)
{
	for (auto &r : m_registers)
		if (r.m_id == reg.get_id())
		{
			if (!r.m_reserved)
				throw __FILE__ ": Trying to free an unreserved register";
			r.m_reserved = false;
			return;
		}
	throw __FILE__ ": No free registers found!";
}

RegisterAllocator::RegisterAllocator()
{
	m_registers.push_back(Register::Id::R10);	// caller-saved
	m_registers.push_back(Register::Id::R11);	// caller-saved
	m_registers.push_back(Register::Id::R12);	// callee-saved
	m_registers.push_back(Register::Id::R13);	// callee-saved
	m_registers.push_back(Register::Id::R14);	// callee-saved
	m_registers.push_back(Register::Id::R15);	// callee-saved
	m_registers.push_back(Register::Id::BX);	// callee-saved
	m_registers.push_back(Register::Id::R9);	// parameter register
	m_registers.push_back(Register::Id::R8);	// parameter register
	m_registers.push_back(Register::Id::CX);	// parameter register
	m_registers.push_back(Register::Id::DX);	// parameter register
	m_registers.push_back(Register::Id::SI);	// parameter register
	m_registers.push_back(Register::Id::DI);	// parameter register
	m_registers.push_back(Register::Id::AX);	// return value register
//	m_registers.push_back(Register::BP);	// stack handling register
//	m_registers.push_back(Register::SP);	// stack handling register
	m_registers.push_back(Register::Id::XMM15);
	m_registers.push_back(Register::Id::XMM14);
	m_registers.push_back(Register::Id::XMM13);
	m_registers.push_back(Register::Id::XMM12);
	m_registers.push_back(Register::Id::XMM11);
	m_registers.push_back(Register::Id::XMM10);
	m_registers.push_back(Register::Id::XMM9);
	m_registers.push_back(Register::Id::XMM8);
	m_registers.push_back(Register::Id::XMM7);
	m_registers.push_back(Register::Id::XMM6);
	m_registers.push_back(Register::Id::XMM5);
	m_registers.push_back(Register::Id::XMM4);
	m_registers.push_back(Register::Id::XMM3);
	m_registers.push_back(Register::Id::XMM2);
	m_registers.push_back(Register::Id::XMM1);
	m_registers.push_back(Register::Id::XMM0);	// return value register
}

void RegisterAllocator::print_state(std::ostream &os) const
{
	os << "Register Allocator state:" << std::endl;
	os << "Allocated registers:" << std::endl;
	for (auto reg : m_registers)
		if (reg.m_reserved)
			os << Register(reg.m_id).str() << std::endl;
}

bool RegisterAllocator::is_reserved(Register::Id reg) const
{
	for (auto &r : m_registers)
		if (r.m_id == reg)
			return r.m_reserved;
	throw __FILE__ ": register not found in allocator";
}
