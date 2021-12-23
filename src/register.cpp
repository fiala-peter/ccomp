#include "register.h"

std::string Register::str() const
{
	struct
	{
		Id id;
		char const *strs[4];
	} data[] = {
		{Id::AX, {"al", "ax", "eax", "rax"}},
		{Id::BX, {"bl", "bx", "ebx", "rbx"}},
		{Id::CX, {"cl", "cx", "ecx", "rcx"}},
		{Id::DX, {"dl", "dx", "edx", "rdx"}},
		{Id::DI, {"dil", "di", "edi", "rdi"}},
		{Id::SI, {"sil", "si", "esi", "rsi"}},
		{Id::BP, {"bpl", "bp", "ebp", "rbp"}},
		{Id::SP, {"spl", "sp", "esp", "rsp"}},
		{Id::R8, {"r8b", "r8w", "r8d", "r8"}},
		{Id::R9, {"r9b", "r9w", "r9d", "r9"}},
		{Id::R10, {"r10b", "r10w", "r10d", "r10"}},
		{Id::R11, {"r11b", "r11w", "r11d", "r11"}},
		{Id::R12, {"r12b", "r12w", "r12d", "r12"}},
		{Id::R13, {"r13b", "r13w", "r13d", "r13"}},
		{Id::R14, {"r14b", "r14w", "r14d", "r14"}},
		{Id::R15, {"r15b", "r15w", "r15d", "r15"}},
		{Id::XMM0, {NULL, NULL, "xmm0", "xmm0"}},
		{Id::XMM1, {NULL, NULL, "xmm1", "xmm1"}},
		{Id::XMM2, {NULL, NULL, "xmm2", "xmm2"}},
		{Id::XMM3, {NULL, NULL, "xmm3", "xmm3"}},
		{Id::XMM4, {NULL, NULL, "xmm4", "xmm4"}},
		{Id::XMM5, {NULL, NULL, "xmm5", "xmm5"}},
		{Id::XMM6, {NULL, NULL, "xmm6", "xmm6"}},
		{Id::XMM7, {NULL, NULL, "xmm7", "xmm7"}},
		{Id::XMM8, {NULL, NULL, "xmm8", "xmm8"}},
		{Id::XMM9, {NULL, NULL, "xmm9", "xmm9"}},
		{Id::XMM10, {NULL, NULL, "xmm10", "xmm10"}},
		{Id::XMM11, {NULL, NULL, "xmm11", "xmm11"}},
		{Id::XMM12, {NULL, NULL, "xmm12", "xmm12"}},
		{Id::XMM13, {NULL, NULL, "xmm13", "xmm13"}},
		{Id::XMM14, {NULL, NULL, "xmm14", "xmm14"}},
		{Id::XMM15, {NULL, NULL, "xmm15", "xmm15"}}};

	// determine the size's logarithm
	size_t cntr = 0, size = m_size;
	while (size != 1)
	{
		size >>= 1;
		cntr++;
	}
	for (int i = 0; i < sizeof data / sizeof data[0]; ++i)
		if (data[i].id == m_id)
			return std::string("%") + data[i].strs[cntr];
	throw __FILE__ "Invalid register id";
}
