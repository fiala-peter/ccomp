#include "code_generator.h"

#include "function_node.h"
#include "translation_unit.h"

#include <algorithm>

std::string CodeGenerator::mnemonic(std::string const &name, size_t s, Register::Type type)
{
	switch (type)
	{
	case Register::Type::INTEGER:
		switch (s)
		{
		case 1:
			return name + "b"; // byte
		case 2:
			return name + "w"; // word
		case 4:
			return name + "l"; // long word
		case 8:
			return name + "q"; // quad word
		}
		throw __FILE__ ": invalid size in mnemonic fuso";
	case Register::Type::FLOATING:
		switch (s)
		{
		case 4:
			return name + "ss"; // scalar single
		case 8:
			return name + "sd"; // scalar double
		}
		throw __FILE__ ": invalid size in mnemonic fuso";
	}
	throw __FILE__ ": invalid register type in mnemonic fuso";
}

Label CodeGenerator::generate_label()
{
	return Label(++m_label_counter, m_scope_counter);
}

void CodeGenerator::print_code_line(std::string const &mnemonic, std::string const &op1, std::string const &op2, std::string const &comment) const
{
	std::string ops = op1;
	if (op2 != "")
		ops += (", " + op2);
	m_os << "\t" << std::setw(8) << std::left << mnemonic;
	m_os << " " << std::setw(24) << std::left << ops;
	if (comment != "")
		m_os << " " << std::setw(8) << std::left << ("# " + comment);
	m_os << std::endl;
}

void CodeGenerator::mov(Register const &from, Register const &to, std::string const &comment) const
{
	if (from.get_size() != to.get_size())
		throw __FILE__ ": MOV error: Inconsistent register sizes";
	print_code_line(mnemonic("mov", to.get_size(), to.get_type()), from.str(), to.str(), comment);
}

void CodeGenerator::mov(long long val, Register const &to, std::string const &comment) const
{
	print_code_line(mnemonic("mov", to.get_size(), to.get_type()), immediate(val), to.str(), comment);
}

void CodeGenerator::add(long long val, Register const &to, std::string const &comment) const
{
	print_code_line(mnemonic("add", to.get_size(), to.get_type()), immediate(val), to.str(), comment);
}

void CodeGenerator::sub(long long val, Register const &to, std::string const &comment) const
{
	print_code_line(mnemonic("sub", to.get_size(), to.get_type()), immediate(val), to.str(), comment);
}

void CodeGenerator::push(Register const &reg, std::string const &comment) const
{
	if (reg.get_size() != 8)
		throw __FILE__ ": Possible erronous push";
	if (reg.get_type() == Register::Type::INTEGER)
		print_code_line(mnemonic("push", reg.get_size()), reg.str(), "", comment);
	else
	{
		Register sp(Register::Id::SP);
		sub(8, sp);
		print_code_line(mnemonic("mov", reg.get_size(), Register::Type::FLOATING), Register(reg).str(), indirect(sp));
	}
}

void CodeGenerator::pop(Register const &reg, std::string const &comment) const
{
	if (reg.get_size() != 8)
		throw __FILE__ ": Possible erronous pop";
	if (reg.get_type() == Register::Type::INTEGER)
		print_code_line(mnemonic("pop", reg.get_size()), reg.str(), "", comment);
	else
	{
		Register sp(Register::Id::SP);
		print_code_line(mnemonic("mov", reg.get_size(), Register::Type::FLOATING), indirect(sp), Register(reg).str());
		add(8, sp);
	}
}

void CodeGenerator::cmp(Register const &rhs_reg, Register const &lhs_reg, std::string const &comment) const
{
	if (lhs_reg.get_type() == Register::Type::INTEGER)
		print_code_line(mnemonic("cmp", lhs_reg.get_size(), lhs_reg.get_type()), rhs_reg.str(), lhs_reg.str(), comment);
	else
		print_code_line(mnemonic("comi", lhs_reg.get_size(), lhs_reg.get_type()), rhs_reg.str(), lhs_reg.str(), comment);
}

void CodeGenerator::print_label(std::string const &label, std::string const &comment) const
{
	m_os << label << ":";
	if (comment != "")
		m_os << "\t"
			 << "# " << comment;
	m_os << std::endl;
}

Register CodeGenerator::int2int_cast(Register const &reg, Type const &t_from, Type const &t_to)
{
	size_t s_to = t_to.get_size_in_bytes();
	size_t s_from = t_from.get_size_in_bytes();

	if (s_to > s_from) // size extension
	{
		std::string opcode = t_from.is_signed_integer() ? "movs" : "movz";

		// result register is the same as source but with different size
		Register reg_to = reg;
		reg_to.set_size(s_to);
		std::string comment = "upcasting from " + reg.str() + " to " + reg_to.str();
		print_code_line(mnemonic(mnemonic(opcode, s_from), s_to), reg.str(), reg_to.str(), comment);
		return reg_to;
	}
	else if (s_to < s_from) // size reduction
	{
		// simply downcast with size reduction
		Register ret = reg;
		ret.set_size(s_to);
		return ret;
	}
	return reg; // same sizes, nothing to do
}

Register CodeGenerator::int2floating_cast(Register const &reg_from, Type const &t_from, Type const &t_to)
{
	if (t_from == Type::int_type())
	{
		std::string opcode = "cvtsi2";
		Register reg_to = m_reg_allocator.allocate(Register::Type::FLOATING);
		reg_to.set_size(t_to.get_size_in_bytes());
		print_code_line(mnemonic(opcode, t_to.get_size_in_bytes(), Register::Type::FLOATING), reg_from.str(), reg_to.str());
		m_reg_allocator.release(reg_from);
		return reg_to;
	}
	else
		throw __FILE__ ": Unimplemented integer to floating cast";
}

Register CodeGenerator::floating2int_cast(Register const &reg_from, Type const &t_from, Type const &t_to)
{
	if (t_from == Type::double_type() && t_to == Type::int_type())
	{
		std::string opcode = "cvtsd2si";
		Register reg_to = m_reg_allocator.allocate(Register::Type::INTEGER);
		reg_to.set_size(t_to.get_size_in_bytes());
		print_code_line(opcode, reg_from.str(), reg_to.str());
		m_reg_allocator.release(reg_from);
		return reg_to;
	}
	else
		throw __FILE__ ": Unimplemented integer to floating cast";
}

CodeGenerator::CodeGenerator(std::ostream &os)
	: m_os(os), m_label_counter(0), m_scope_counter(0)
{
	m_reg_allocator.reset();

	m_callee_saved = {Register::Id::R12, Register::Id::R13, Register::Id::R14, Register::Id::R15, Register::Id::BX};

	m_caller_saved = {Register::Id::CX, Register::Id::DX, Register::Id::SI, Register::Id::DI,
					  Register::Id::R8, Register::Id::R9, Register::Id::R10, Register::Id::R11,
					  Register::Id::XMM0, Register::Id::XMM1, Register::Id::XMM2, Register::Id::XMM3,
					  Register::Id::XMM4, Register::Id::XMM5, Register::Id::XMM6, Register::Id::XMM7,
					  Register::Id::XMM8, Register::Id::XMM9, Register::Id::XMM10, Register::Id::XMM11,
					  Register::Id::XMM12, Register::Id::XMM13, Register::Id::XMM14, Register::Id::XMM15};

#ifdef _WIN32
	m_integer_parameters = {Register::Id::CX, Register::Id::DX, Register::Id::R8, Register::Id::R9};
	m_floating_parameters = {Register::Id::XMM0, Register::Id::XMM1, Register::Id::XMM2, Register::Id::XMM3};

#else
	m_integer_parameters = {Register::Id::DI, Register::Id::SI, Register::Id::DX,
							Register::Id::CX, Register::Id::R8, Register::Id::R9};

	m_floating_parameters = {Register::Id::XMM0, Register::Id::XMM1, Register::Id::XMM2,
							 Register::Id::XMM3, Register::Id::XMM4, Register::Id::XMM5};
#endif
}

void CodeGenerator::generate_statement(StmNode const &root)
{
	switch (root.get_id())
	{
	case StmNode::Id::BLOCK:
		return generate_block(static_cast<CompoundNode const &>(root));
	case StmNode::Id::IF:
		return generate_if(root);
	case StmNode::Id::WHILE:
		return generate_while(root);
	case StmNode::Id::DO:
		return generate_do(root);
	case StmNode::Id::FOR:
		return generate_for(root);
	case StmNode::Id::XPR:
		return generate_expression_stm(root);
	case StmNode::Id::RETURN:
		return generate_return(root);
	case StmNode::Id::BREAK:
	case StmNode::Id::CONTINUE:
		return generate_break_continue(root);
	case StmNode::Id::EMPTY:
		return;
	}
	throw __FILE__ ": Unhandled statement for code generation";
}

void CodeGenerator::generate_translation_unit(TransUnitNode *trans)
{
	if (trans == nullptr)
		return;

	// generate data segment
	print_code_line(".data");

	// generate initialized data segment

	// generate string constants
	for (auto const &s : trans->get_string_literals())
		m_string_table.push_back(std::make_pair(s, generate_label()));
	for (auto const &[str, lab] : m_string_table)
	{
		print_label(lab.str());
		print_code_line(".string", "\"" + str + "\"");
	}

	// generate floating point constants
	for (auto const &s : trans->get_floating_constants())
		m_float_table.push_back(std::make_pair(s, generate_label()));
	for (auto const &[d, lab] : m_float_table)
	{
		print_code_line(".align 8");
		print_label(lab.str());
		std::string comment = std::to_string(d);
		for (int i = 0; i < sizeof(double) / sizeof(int); ++i)
		{
			int32_t i32 = *(reinterpret_cast<int32_t const *>(&d) + i);
			print_code_line(".long", std::to_string(i32), "", i == 0 ? comment : "");
		}
	}

	// generate bss segment
	print_code_line(".bss");
	for (auto const &entry : trans->get_symbol_pointer()->get_symbols())
	{
		if (!entry.is_object())
			continue;

		std::string const &id = entry.get_id();
		Type const &type = entry.get_type();

		if (!entry.is_extern())
		{
			if (!entry.is_static())
				print_code_line(".globl", id);
			print_code_line(".align", std::to_string(type.get_alignment_in_bytes()));
			print_code_line(".type", id, "@object");
			print_code_line(".size", id, std::to_string(type.get_size_in_bytes()));
			print_label(id);
			print_code_line(".zero", std::to_string(type.get_size_in_bytes()));
		}

		// these objects are available through their id-s
		m_local_table.push(0, id, 0, 0);
	}

	// generate text segment with functions
	print_code_line(".text");
	for (auto s : trans->get_functions())
		generate_function(s);
}

void CodeGenerator::generate_function_prolog(FunctionNode *function)
{
	Register sp(Register::Id::SP), bp(Register::Id::BP);

	print_code_line(".globl", function->get_identifier());
#ifdef _WIN32
#else
	print_code_line(".type", function->get_identifier(), "@function");
#endif
	print_label(function->get_identifier());

	// save caller's frame pointer
	push(bp, "save caller stack frame base");

	// save callee-saved registers
	for (auto const &id : m_callee_saved)
		push(Register(id), "save callee-saved register to stack");

	if (m_callee_saved.size() % 2 != 0)
		sub(8, sp, "ensure 16 byte alignment");

	mov(sp, bp, "establish new stack frame");

	print_code_line("", "", "", "------end of function prolog");
}

void CodeGenerator::generate_function_epilog(FunctionNode *function)
{
	print_code_line("", "", "", "------start of function epilog");
	Register sp(Register::Id::SP), bp(Register::Id::BP);
	mov(bp, sp, "restore caller stack pointer");

	// reload callee-saved registers
	if (m_callee_saved.size() % 2 != 0)
		add(8, sp, "remove 16 byte padding");
	for (auto it = m_callee_saved.rbegin(); it != m_callee_saved.rend(); ++it)
		pop(Register(*it), "restore callee-saved registers");

	pop(bp, "restore caller stack frame base");

	print_code_line("ret", "", "", "end of FUNCTION");
}

void CodeGenerator::generate_function(FunctionNode *function)
{
	if (function == nullptr)
		return;

	// generate the function prolog
	generate_function_prolog(function);

	// generate the label for return jumps
	m_actual_function_return_label = generate_label();

	// enter scope of parameters
	enter_scope(function->get_symbol_pointer());

	// copy parameters from registers to stack
	auto const &symbols = function->get_symbol_pointer()->get_symbols();
	// traverse the objects in reverse order as in he "objects" table is a stack
#ifdef _WIN32
	size_t i = 0;
	for (auto it = symbols.crbegin(); it != symbols.crend(); ++it)
	{
		if (!it->is_object())
			continue;
		std::string const &parname = it->get_id();
		std::string memname = m_local_table.lookup(parname);
		size_t siz = it->get_type().get_size_in_bytes();
		Register reg_from;
		if (it->get_type().is_floating())
			reg_from = Register(m_floating_parameters[i++], siz);
		else
			reg_from = Register(m_integer_parameters[i++], siz);
		std::string comment = "save " + parname + " to stack";
		print_code_line(mnemonic("mov", reg_from.get_size()), reg_from.str(), memname, comment);
	}
#else
	size_t i_idx = 0, f_idx = 0;
	for (auto it = symbols.crbegin(); it != symbols.crend(); ++it)
	{
		if (!it->is_object())
			continue;
		std::string const &parname = it->get_id();
		std::string memname = m_local_table.lookup(parname);
		size_t siz = it->get_type().get_size_in_bytes();
		Register reg_from;
		if (it->get_type().is_floating())
			reg_from = Register(m_floating_parameters[f_idx++], siz);
		else
			reg_from = Register(m_integer_parameters[i_idx++], siz);
		std::string comment = "save " + parname + " to stack";
		print_code_line(mnemonic("mov", reg_from.get_size()), reg_from.str(), memname, comment);
	}
#endif

	// generate compound statement
	generate_block(function->get_block());

	// exit scope of parameters
	exit_scope();

	print_label(m_actual_function_return_label.str(), "return point of function");

	generate_function_epilog(function);
}

void CodeGenerator::enter_scope(SymbolNode const *symbol_pointer)
{
	++m_scope_counter;

	// get table of automatic objects to be allocated on the stack
	auto const &symbols = symbol_pointer->get_symbols();
	// traverse the objects in reverse order as in he "objects" table is a stack
	size_t before_size = m_local_table.get_size();
	for (auto it = symbols.crbegin(); it != symbols.crend(); ++it)
	{
		if (!it->is_object())
			continue;
		size_t size = it->get_type().get_size_in_bytes();
		size_t alignment = it->get_type().get_alignment_in_bytes();
		std::string const &id = it->get_id();
		if (it->get_type().is_object())
			m_local_table.push(m_scope_counter, id, size, alignment);
	}
	// get total size of local variable table and adjust stack pointer accordingly
	size_t after_size = m_local_table.get_size();
	Register sp(Register::Id::SP);
	if (after_size != before_size)
		sub(after_size - before_size, sp, "alloc for automatic variables on stack");
}

void CodeGenerator::exit_scope()
{
	Register sp(Register::Id::SP);
	size_t before_size = m_local_table.get_size();
	m_local_table.purge(m_scope_counter);
	size_t after_size = m_local_table.get_size();
	// adjust stack pointer
	if (after_size != before_size)
		add(before_size - after_size, sp, "remove automatic variables from stack");

	--m_scope_counter;
}

void CodeGenerator::generate_goto(Label const &lab) const
{
	Register sp(Register::Id::SP);
	size_t before_offset = m_local_table.get_size();
	size_t after_offset = m_local_table.get_size_at_scope(lab.get_scope());
	if (after_offset != before_offset)
		add(before_offset - after_offset, sp, "rewind stack before jump");
	print_code_line("jmp", lab.str());
}

void CodeGenerator::generate_block(CompoundNode const &block)
{
	enter_scope(block.get_symbol_pointer());

	// generate statements
	for (auto s : block.get_substms())
		generate_statement(*s);

	exit_scope();
}

void CodeGenerator::enter_loop(Label const &continue_label, Label const &break_label)
{
	m_continue_stack.push_front(continue_label);
	m_break_stack.push_front(break_label);
}

void CodeGenerator::exit_loop()
{
	m_continue_stack.pop_front();
	m_break_stack.pop_front();
}

bool CodeGenerator::is_relational_expression(XprNode::Id id)
{
	return id == XprNode::Id::LESS || id == XprNode::Id::LESS_EQUAL || id == XprNode::Id::GREATER || id == XprNode::Id::GREATER_EQUAL || id == XprNode::Id::EQUAL || id == XprNode::Id::NOT_EQUAL;
}

void CodeGenerator::goto_if_false(XprNode const *cond, Label const &label)
{
	XprNode::Id id = cond->get_id();
	if (is_relational_expression(id))
	{
		XprNode const *lhs_xpr = cond->get_subxpr(0), *rhs_xpr = cond->get_subxpr(1);
		// remark: this choice is intentional, floating points must be compared as unsigned
		bool is_signed = lhs_xpr->get_xpr_type().is_signed_integer();
		std::string opcode;
		switch (id)
		{
		case XprNode::Id::LESS:
			opcode = is_signed ? "jge" : "jae";
			break;
		case XprNode::Id::LESS_EQUAL:
			opcode = is_signed ? "jg" : "ja";
			break;
		case XprNode::Id::GREATER:
			opcode = is_signed ? "jle" : "jbe";
			break;
		case XprNode::Id::GREATER_EQUAL:
			opcode = is_signed ? "jl" : "jb";
			break;
		case XprNode::Id::EQUAL:
			opcode = "jne";
			break;
		case XprNode::Id::NOT_EQUAL:
			opcode = "je";
			break;
		}
		Register lhs_reg = generate_xpr(lhs_xpr);
		Register rhs_reg = generate_xpr(rhs_xpr);
		m_reg_allocator.release(lhs_reg);
		m_reg_allocator.release(rhs_reg);
		cmp(rhs_reg, lhs_reg);
		print_code_line(opcode, label.str());
	}
	else if (id == XprNode::Id::LOGICAL_AND)
	{
		XprNode const *lhs_xpr = cond->get_subxpr(0), *rhs_xpr = cond->get_subxpr(1);
		goto_if_false(lhs_xpr, label);
		goto_if_false(rhs_xpr, label);
	}
	else
	{
		Register reg = generate_xpr(cond);
		print_code_line(mnemonic("test", reg.get_size()), reg.str(), reg.str());
		m_reg_allocator.release(reg);
		print_code_line("jz", label.str());
	}
}

void CodeGenerator::generate_while(StmNode const &root)
{
	Label condition_label = generate_label(), done_label = generate_label();
	enter_loop(condition_label, done_label);
	print_label(condition_label.str(), "condition of WHILE statement");
	// generate code for expression and testing code
	goto_if_false(root.get_subxpr(0), done_label);
	// generate code for statement
	generate_statement(root.get_substm(0));
	// generate loop jump codes
	print_code_line("jmp", condition_label.str());
	print_label(done_label.str(), "WHILE statement done");
	exit_loop();
}

void CodeGenerator::generate_do(StmNode const &root)
{
	Label start_label = generate_label(), condition_label = generate_label(), done_label = generate_label();
	enter_loop(condition_label, done_label);
	print_label(start_label.str(), "beginning of DO statement");
	generate_statement(root.get_substm(0));
	print_label(condition_label.str(), "condition of DO statement");
	goto_if_false(root.get_subxpr(0), done_label);
	print_code_line("jmp", start_label.str());
	print_label(done_label.str(), "DO statement done");
	exit_loop();
}

void CodeGenerator::generate_for(StmNode const &root)
{
	Label condition_label = generate_label(), step_label = generate_label(), done_label = generate_label();
	enter_loop(step_label, done_label);
	// generate code for initialization
	if (root.get_subxpr(0) != nullptr)
		m_reg_allocator.release(generate_xpr(root.get_subxpr(0)));
	print_label(condition_label.str(), "condition of FOR statement");
	// generate code for expression and testing code
	goto_if_false(root.get_subxpr(1), done_label);
	// generate code for statement
	generate_statement(root.get_substm(0));
	// generate code for step expression
	print_label(step_label.str(), "step expression of FOR statement");
	if (root.get_subxpr(2))
		m_reg_allocator.release(generate_xpr(root.get_subxpr(2)));
	// generate loop jump codes
	print_code_line("jmp", condition_label.str());
	print_label(done_label.str(), "FOR statement done");
	exit_loop();
}

void CodeGenerator::generate_if(StmNode const &root)
{
	bool has_else_branch = root.get_num_substms() > 1 && root.get_substms()[1] != nullptr;
	Label else_label = generate_label(), done_label = generate_label();
	// generate code for expression and testing code
	goto_if_false(root.get_subxpr(0), has_else_branch ? else_label : done_label);
	// generate code for true branch
	generate_statement(root.get_substm(0));
	// generate code for else branch
	if (has_else_branch)
	{
		print_code_line("jmp", done_label.str());
		print_label(else_label.str(), "else branch of IF statement");
		generate_statement(root.get_substm(1));
	}
	print_label(done_label.str(), "end of IF statement");
}

void CodeGenerator::generate_return(StmNode const &root)
{
	if (root.get_num_subxprs() != 0)
	{
		// generate code for return expression
		Register xpr_reg = generate_xpr(root.get_subxpr(0));
		Register::Id return_id = xpr_reg.get_type() == Register::Type::FLOATING ? Register::Id::XMM0 : Register::Id::AX;
		Register ret_reg = m_reg_allocator.allocate(return_id);
		ret_reg.set_size(xpr_reg.get_size());
		mov(xpr_reg, ret_reg);
		m_reg_allocator.release(ret_reg);
		m_reg_allocator.release(xpr_reg);
	}
	// generate "jump to function end" code
	generate_goto(m_actual_function_return_label);
}

void CodeGenerator::generate_break_continue(StmNode const &root)
{
	if (root.get_id() == StmNode::Id::BREAK)
		generate_goto(m_break_stack.front());
	else if (root.get_id() == StmNode::Id::CONTINUE)
		generate_goto(m_continue_stack.front());
}

void CodeGenerator::generate_expression_stm(StmNode const &root)
{
	m_reg_allocator.release(generate_xpr(root.get_subxpr(0)));
}

Register CodeGenerator::generate_xpr(XprNode const *xpr_node)
{
	switch (xpr_node->get_id())
	{
	case XprNode::Id::INTEGER_XPR:
		return generate_integer_constant(xpr_node);
	case XprNode::Id::ASSIGN:
		return generate_assignment(xpr_node);
	case XprNode::Id::IDENTIFIER:
		return generate_identifier_rvalue(static_cast<IdentifierXprNode const *>(xpr_node));
	case XprNode::Id::BINARY_PLUS:
	case XprNode::Id::BINARY_MINUS:
		return generate_binary_additive(xpr_node);
	case XprNode::Id::CAST:
		return generate_cast(xpr_node);
	case XprNode::Id::DEREFERENCE:
		return generate_dereference(xpr_node);
	case XprNode::Id::LESS:
	case XprNode::Id::LESS_EQUAL:
	case XprNode::Id::GREATER:
	case XprNode::Id::GREATER_EQUAL:
	case XprNode::Id::NOT_EQUAL:
	case XprNode::Id::EQUAL:
		return generate_relational(xpr_node);
	case XprNode::Id::LOGICAL_AND:
	case XprNode::Id::LOGICAL_OR:
		return generate_logical_andor(xpr_node);
	case XprNode::Id::MOD:
	case XprNode::Id::PER:
		return generate_divmod(xpr_node);
	case XprNode::Id::TIMES:
		return generate_times(xpr_node);
	case XprNode::Id::ARRAY_SUBSCRIPT:
		return generate_array_subscript_rvalue(xpr_node);
	case XprNode::Id::STRUCTURE_PTR_MEMBER:
	case XprNode::Id::STRUCTURE_MEMBER:
		return generate_structure_member_rvalue(xpr_node);
	case XprNode::Id::CONDITIONAL:
		return generate_conditional(xpr_node);
	case XprNode::Id::FUNCTION_CALL:
		return generate_function_call(xpr_node);
	case XprNode::Id::STRING_LITERAL:
		return generate_string_literal(*dynamic_cast<StringLiteralNode const *>(xpr_node));
	case XprNode::Id::FLOATING_XPR:
		return generate_floating_constant(*dynamic_cast<FloatingConstant const *>(xpr_node));
	case XprNode::Id::ADDRESS_OF:
		return generate_address_of(*xpr_node);
	case XprNode::Id::UNARY_MINUS:
	case XprNode::Id::UNARY_PLUS:
		return generate_unary_mp(*xpr_node);
	case XprNode::Id::PREINCREMENT:
	case XprNode::Id::PREDECREMENT:
	case XprNode::Id::POSTINCREMENT:
	case XprNode::Id::POSTDECREMENT:
		return generate_crement(*xpr_node);
	case XprNode::Id::COMMA:
		return generate_comma(*xpr_node);
	case XprNode::Id::PLUS_ASSIGN:
		return generate_plusassignment(xpr_node);
	default:
		throw __FILE__ ": Unimplemented expression.";
	}
}

Register CodeGenerator::generate_integer_constant(XprNode const *xpr)
{
	size_t s = xpr->get_xpr_type().get_size_in_bytes();
	long long val = dynamic_cast<IntegerConstant const *>(xpr)->evaluate_constant();
	Register reg = m_reg_allocator.allocate(Register::Type::INTEGER);
	reg.set_size(s);
	mov(val, reg);
	return reg;
}

Register CodeGenerator::generate_assignment(XprNode const *xpr)
{
	XprNode const *lhs_xpr = xpr->get_subxpr(0), *rhs_xpr = xpr->get_subxpr(1);
	Type const &value_type = xpr->get_xpr_type();

	Register lhs_addr;
	if (lhs_xpr->get_id() == XprNode::Id::IDENTIFIER)
	{
		IdentifierXprNode const *idxpr = static_cast<IdentifierXprNode const *>(lhs_xpr);
		size_t ptr_size = Type::int_type().pointer_to().get_size_in_bytes();
		std::string varname = idxpr->get_identifier();
		std::string memname = m_local_table.lookup(varname);
		lhs_addr = m_reg_allocator.allocate(Register::Type::INTEGER);
		lhs_addr.set_size(ptr_size);
		std::string comment = "load address of " + varname + " into " + lhs_addr.str();
		print_code_line(mnemonic("lea", ptr_size), memname, lhs_addr.str(), comment);
	}
	else if (lhs_xpr->get_id() == XprNode::Id::DEREFERENCE)
		lhs_addr = generate_xpr(lhs_xpr->get_subxpr(0));
	else if (lhs_xpr->get_id() == XprNode::Id::ARRAY_SUBSCRIPT)
		lhs_addr = generate_pointer_shift(lhs_xpr->get_subxpr(0), lhs_xpr->get_subxpr(1));
	else if (lhs_xpr->get_id() == XprNode::Id::STRUCTURE_PTR_MEMBER || lhs_xpr->get_id() == XprNode::Id::STRUCTURE_MEMBER)
		lhs_addr = generate_structure_member_address(lhs_xpr->get_subxpr(0),
													 static_cast<IdentifierXprNode const *>(lhs_xpr->get_subxpr(1)));
	else
		throw __FILE__ ":  Unimplemented assignment case";

	// evaluate rhs into register
	Register rhs_reg = generate_xpr(rhs_xpr);
	size_t result_size = value_type.get_size_in_bytes();
	if (value_type.is_structure())
	{
		// copy structure in slices
		size_t block_size = 8;
		size_t to_copy = result_size;
		size_t offset = 0;
		Register tmp = m_reg_allocator.allocate(Register::Type::INTEGER);
		while (to_copy > 0)
		{
			while (to_copy < block_size)
				block_size /= 2;
			tmp.set_size(block_size);
			print_code_line(mnemonic("mov", block_size), std::to_string(offset) + indirect(rhs_reg), tmp.str());
			print_code_line(mnemonic("mov", block_size), tmp.str(), std::to_string(offset) + indirect(lhs_addr));
			offset += block_size;
			to_copy -= block_size;
		}
		m_reg_allocator.release(tmp);
	}
	else
	{
		std::string comment = "store " + rhs_reg.str() + " in *" + lhs_addr.str();
		print_code_line(mnemonic("mov", result_size, rhs_reg.get_type()), rhs_reg.str(), indirect(lhs_addr), comment);
	}
	m_reg_allocator.release(lhs_addr);

	// result of expression is in rhs register
	return rhs_reg;
}

Register CodeGenerator::generate_plusassignment(XprNode const *xpr)
{
	XprNode const *lhs_xpr = xpr->get_subxpr(0), *rhs_xpr = xpr->get_subxpr(1);
	Type const &value_type = xpr->get_xpr_type();
	Type const &lhs_type = lhs_xpr->get_xpr_type();
	Type const &rhs_type = rhs_xpr->get_xpr_type();

	Register lhs_addr;
	if (lhs_xpr->get_id() == XprNode::Id::IDENTIFIER)
	{
		IdentifierXprNode const *idxpr = static_cast<IdentifierXprNode const *>(lhs_xpr);
		size_t ptr_size = Type::int_type().pointer_to().get_size_in_bytes();
		std::string varname = idxpr->get_identifier();
		std::string memname = m_local_table.lookup(varname);
		lhs_addr = m_reg_allocator.allocate(Register::Type::INTEGER);
		lhs_addr.set_size(ptr_size);
		std::string comment = "load address of " + varname + " into " + lhs_addr.str();
		print_code_line(mnemonic("lea", ptr_size), memname, lhs_addr.str(), comment);
	}
	else if (lhs_xpr->get_id() == XprNode::Id::DEREFERENCE)
		lhs_addr = generate_xpr(lhs_xpr->get_subxpr(0));
	else if (lhs_xpr->get_id() == XprNode::Id::ARRAY_SUBSCRIPT)
		lhs_addr = generate_pointer_shift(lhs_xpr->get_subxpr(0), lhs_xpr->get_subxpr(1));
	else if (lhs_xpr->get_id() == XprNode::Id::STRUCTURE_PTR_MEMBER || lhs_xpr->get_id() == XprNode::Id::STRUCTURE_MEMBER)
		lhs_addr = generate_structure_member_address(lhs_xpr->get_subxpr(0),
													 static_cast<IdentifierXprNode const *>(lhs_xpr->get_subxpr(1)));
	else
		throw __FILE__ ":  Unimplemented plusassignment case";

	// evaluate rhs into register
	Register rhs_reg = generate_xpr(rhs_xpr);
	size_t result_size = value_type.get_size_in_bytes();

	if (lhs_type.is_pointer())
		throw __FILE__ ": Pointer += is unimplemented yet";

	// add *lhs to rhs
	std::string comment = "add *" + lhs_addr.str() + " to " + rhs_reg.str();
	print_code_line(mnemonic("add", result_size, rhs_reg.get_type()), indirect(lhs_addr), rhs_reg.str(), comment);
	// mov rhs to *lhs
	comment = "mov " + rhs_reg.str() + " to *" + lhs_addr.str();
	print_code_line(mnemonic("mov", result_size, rhs_reg.get_type()), rhs_reg.str(), indirect(lhs_addr), comment);

	m_reg_allocator.release(lhs_addr);

	// result of expression is in rhs register
	return rhs_reg;
}

Register CodeGenerator::generate_identifier_rvalue(IdentifierXprNode const *xpr)
{
	std::string opcode;
	size_t result_size;
	Type const &xpr_type = xpr->get_xpr_type();
	std::string const &idname = xpr->get_identifier();
	std::string memname;

	if (xpr_type.is_function()) // the funcion's address needs to be evaluated
	{
		result_size = xpr_type.pointer_to().get_size_in_bytes();
		opcode = "mov";
		memname = "$" + idname;
	}
	else if (xpr_type.is_array()) // if id is array, its address (pointer) is evaluated
	{
		result_size = xpr_type.element_type().pointer_to().get_size_in_bytes();
		opcode = "lea";
		memname = m_local_table.lookup(idname);
	}
	else if (xpr_type.is_structure()) // if id is structure, its address (pointer) is evaluated
	{
		result_size = xpr_type.pointer_to().get_size_in_bytes();
		opcode = "lea";
		memname = m_local_table.lookup(idname);
	}
	else // else its value is moved to a register
	{
		result_size = xpr_type.get_size_in_bytes();
		opcode = "mov";
		memname = m_local_table.lookup(idname);
	}
	// allocate register to store appropriate size
	Register reg = m_reg_allocator.allocate(xpr_type.is_floating() ? Register::Type::FLOATING : Register::Type::INTEGER);
	reg.set_size(result_size);
	std::string comment = "load " + idname + " to " + reg.str();
	print_code_line(mnemonic(opcode, result_size, reg.get_type()), memname, reg.str(), comment);
	return reg;
}

Register CodeGenerator::generate_structure_member_address(XprNode const *str_ptr_xpr, IdentifierXprNode const *field_xpr)
{
	// structure base address (works for struct and struct pointer as well)
	Register addr_reg = generate_xpr(str_ptr_xpr);
	// increase with offset
	Type structure_type = str_ptr_xpr->get_xpr_type();
	if (structure_type.is_pointer())
		structure_type = structure_type.referenced_type();
	std::string const &fieldname = field_xpr->get_identifier();
	size_t offset = structure_type.lookup_structure_field_offset(fieldname);
	add(offset, addr_reg, "Apply offset of member " + fieldname);
	return addr_reg;
}

Register CodeGenerator::generate_pointer_shift(XprNode const *lhs_xpr, XprNode const *rhs_xpr, bool is_plus)
{
	// make sure that lhs is the pointer and rhs is the integer
	if (lhs_xpr->get_xpr_type().is_integer())
		std::swap(lhs_xpr, rhs_xpr);

	Type lhs_t = lhs_xpr->get_xpr_type();
	Type rhs_t = rhs_xpr->get_xpr_type();

	// convert types to pointer from array if needed
	if (lhs_t.is_array())
		lhs_t = lhs_t.element_type().pointer_to();
	if (rhs_t.is_array())
		rhs_t = rhs_t.element_type().pointer_to();

	size_t lhs_size = lhs_t.get_size_in_bytes();
	size_t rhs_size = rhs_t.get_size_in_bytes();

	// generate both sides into registers
	Register lhs_reg = generate_xpr(lhs_xpr);
	Register rhs_reg = generate_xpr(rhs_xpr);

	// byte extend integer with sign extension
	rhs_reg = int2int_cast(rhs_reg, rhs_t, lhs_t);
	rhs_size = rhs_reg.get_size();
	// referenced size of pointer
	size_t elsiz = lhs_t.referenced_type().get_size_in_bytes();
	if (elsiz == 1 || elsiz == 2 || elsiz == 4 || elsiz == 8)
	{
		// for simple sizes perform addition/subtraction by computing effective addres
		if (!is_plus)
			print_code_line(mnemonic("neg", rhs_size), rhs_reg.str());
		print_code_line(mnemonic("lea", lhs_size), indirect(lhs_reg, rhs_reg, elsiz), lhs_reg.str());
	}
	else
	{
		// for other element sizes perform multiplication and addition/subtraction
		print_code_line(mnemonic("imul", rhs_reg.get_size()), immediate(elsiz), rhs_reg.str());
		print_code_line(mnemonic(is_plus ? "add" : "sub", lhs_reg.get_size()), rhs_reg.str(), lhs_reg.str());
	}
	m_reg_allocator.release(rhs_reg);

	return lhs_reg;
}

Register CodeGenerator::generate_binary_additive(XprNode const *xpr)
{
	bool isplus = xpr->get_id() == XprNode::Id::BINARY_PLUS;

	XprNode const *lhs_xpr = xpr->get_subxpr(0);
	XprNode const *rhs_xpr = xpr->get_subxpr(1);
	Type lhs_type = lhs_xpr->get_xpr_type();
	Type rhs_type = rhs_xpr->get_xpr_type();
	size_t lhs_size = lhs_type.get_size_in_bytes();
	size_t rhs_size = rhs_type.get_size_in_bytes();

	// there are 4 cases:
	// 1: integer +- integer
	// 2: floating +- floating
	// 3: pointer - pointer
	// 4: pointer +- integer or in reverse order

	// cases 1-2
	if (lhs_type.is_integer() && rhs_type.is_integer() || lhs_type.is_floating() && rhs_type.is_floating())
	{
		Register lhs_reg = generate_xpr(lhs_xpr);
		Register rhs_reg = generate_xpr(rhs_xpr);
		print_code_line(mnemonic(isplus ? "add" : "sub", lhs_size, lhs_reg.get_type()), rhs_reg.str(), lhs_reg.str());
		m_reg_allocator.release(rhs_reg);
		return lhs_reg;
	}

	// case 3: pointer - pointer
	// convert types to pointer from array if needed
	if (lhs_type.is_array())
		lhs_type = lhs_type.element_type().pointer_to();
	if (rhs_type.is_array())
		rhs_type = rhs_type.element_type().pointer_to();

	if (lhs_type.is_pointer() && rhs_type.is_pointer())
	{
		Register lhs_reg = generate_xpr(lhs_xpr);
		Register rhs_reg = generate_xpr(rhs_xpr);
		std::string comment = "compute pointer difference in bytes";
		print_code_line(mnemonic("sub", lhs_size), rhs_reg.str(), lhs_reg.str(), comment);

		size_t referenced_size = lhs_type.referenced_type().get_size_in_bytes();
		if (referenced_size != 1)
		{
			comment = "divide difference by element size";
			Register ax = m_reg_allocator.allocate(Register::Id::AX);
			ax.set_size(lhs_size);
			Register dx = m_reg_allocator.allocate(Register::Id::DX);
			dx.set_size(lhs_size);
			mov(lhs_reg, ax, comment);
			if (lhs_size == 8)
				print_code_line("cqo");
			else if (lhs_size == 4)
				print_code_line("cdq");
			else if (lhs_size == 2)
				print_code_line("cwd");
			mov(referenced_size, rhs_reg);
			print_code_line(mnemonic("idiv", lhs_size), rhs_reg.str());
			mov(ax, lhs_reg);
			m_reg_allocator.release(dx);
			m_reg_allocator.release(ax);
		}
		m_reg_allocator.release(rhs_reg);
		return lhs_reg;
	}

	// case 4 by auxilliary function
	return generate_pointer_shift(lhs_xpr, rhs_xpr, isplus);
}

Register CodeGenerator::fun2ptr_cast(Register const &reg)
{
	return reg;
}

Register CodeGenerator::generate_cast(XprNode const *xpr_to)
{
	XprNode const *xpr_from = xpr_to->get_subxpr(0);
	Type const &tfrom = xpr_from->get_xpr_type();
	Type const &tto = xpr_to->get_xpr_type();
	// pointer to array conversion is transparent at this level
	if (tto.is_pointer() && tfrom.is_array())
		return generate_xpr(xpr_from);
	// pointer to pointer conversion is transparent at this level
	if (tto.is_pointer() && tfrom.is_pointer())
		return generate_xpr(xpr_from);
	// integer to integer conversion
	if (tfrom.is_integer() && tto.is_integer())
		return int2int_cast(generate_xpr(xpr_from), tfrom, tto);
	// integer to floating point
	if (tfrom.is_integer() && tto.is_floating())
		return int2floating_cast(generate_xpr(xpr_from), tfrom, tto);
	// floating to integer cast
	if (tfrom.is_floating() && tto.is_integer())
		return floating2int_cast(generate_xpr(xpr_from), tfrom, tto);
	// function to pointer cast
	if (tfrom.is_function() && tto.is_pointer() && tto.referenced_type() == tfrom)
		return fun2ptr_cast(generate_xpr(xpr_from));
	// integer to pointer cast
	if (tfrom.is_integer() && tto.is_pointer())
		return int2int_cast(generate_xpr(xpr_from), tfrom, tto);
	throw __FILE__ ": Unimplemented cast expression";
}

Register CodeGenerator::generate_relational(XprNode const *xpr)
{
	XprNode const *lhs = xpr->get_subxpr(0), *rhs = xpr->get_subxpr(1);
	Label true_label = generate_label(), done_label = generate_label();
	std::string opcode;
	// intentional, floating point must be handled as unsigned
	bool is_signed = lhs->get_xpr_type().is_signed_integer();
	switch (xpr->get_id())
	{
	case XprNode::Id::LESS:
		opcode = is_signed ? "jl" : "jb";
		break;
	case XprNode::Id::LESS_EQUAL:
		opcode = is_signed ? "jle" : "jbe";
		break;
	case XprNode::Id::GREATER:
		opcode = is_signed ? "jg" : "ja";
		break;
	case XprNode::Id::GREATER_EQUAL:
		opcode = is_signed ? "jge" : "jae";
		break;
	case XprNode::Id::EQUAL:
		opcode = "je";
		break;
	case XprNode::Id::NOT_EQUAL:
		opcode = "jne";
		break;
	}
	Register lhs_reg = generate_xpr(lhs);
	Register rhs_reg = generate_xpr(rhs);
	cmp(rhs_reg, lhs_reg);
	m_reg_allocator.release(lhs_reg);
	m_reg_allocator.release(rhs_reg);
	// for floating point arguments a new register needs to be allocated
	Register res_reg = m_reg_allocator.allocate(Register::Type::INTEGER);
	print_code_line(opcode, true_label.str());
	// place 0/1 result in lreg, resized to int
	res_reg.set_size(Type::int_type().get_size_in_bytes());
	mov(0, res_reg);
	print_code_line("jmp", done_label.str());
	print_label(true_label.str());
	mov(1, res_reg);
	print_label(done_label.str());
	return res_reg;
}

void CodeGenerator::generate_short_circuit_rec(XprNode const *xpr, XprNode::Id id, std::string const &jump_cond, Label const &lab)
{
	if (xpr->get_id() != id)
	{
		Register reg = generate_xpr(xpr);
		size_t s = xpr->get_xpr_type().get_size_in_bytes();
		print_code_line(mnemonic("test", s), reg.str(), reg.str());
		print_code_line(jump_cond, lab.str());
		m_reg_allocator.release(reg);
	}
	else
		for (auto x : xpr->get_subxprs())
			generate_short_circuit_rec(x, id, jump_cond, lab);
}

Register CodeGenerator::generate_logical_andor(XprNode const *xpr)
{
	XprNode::Id id = xpr->get_id();
	Label shortcut_label = generate_label(), done_label = generate_label();
	if (id == XprNode::Id::LOGICAL_AND)
		generate_short_circuit_rec(xpr, id, "jz", shortcut_label);
	else // LOGICAL_OR
		generate_short_circuit_rec(xpr, id, "jnz", shortcut_label);
	Register ret = m_reg_allocator.allocate(Register::Type::INTEGER);
	ret.set_size(Type::int_type().get_size_in_bytes());
	if (xpr->get_id() == XprNode::Id::LOGICAL_AND)
		mov(1, ret);
	else // LOGICAL_OR
		mov(0, ret);
	print_code_line("jmp", done_label.str());
	print_label(shortcut_label.str());
	if (xpr->get_id() == XprNode::Id::LOGICAL_AND)
		mov(0, ret);
	else // LOGICAL_OR
		mov(1, ret);
	print_label(done_label.str());
	return ret;
}

Register CodeGenerator::generate_divmod(XprNode const *xpr)
{
	bool isdiv = xpr->get_id() == XprNode::Id::PER;
	XprNode const *lhs_xpr = xpr->get_subxpr(0);
	XprNode const *rhs_xpr = xpr->get_subxpr(1);
	Type const &lhs_type = lhs_xpr->get_xpr_type();
	size_t lhs_size = lhs_type.get_size_in_bytes();

	// evaluate both arguments
	Register lhs_reg = generate_xpr(lhs_xpr);
	Register rhs_reg = generate_xpr(rhs_xpr);
	m_reg_allocator.release(rhs_reg);

	// result will be stored in lhs
	if (isdiv && lhs_type.is_floating())
		print_code_line(mnemonic("div", lhs_size, lhs_reg.get_type()), rhs_reg.str(), lhs_reg.str());
	else
	{
		// division is performed in ax:dx
		Register ax = m_reg_allocator.allocate(Register::Id::AX);
		ax.set_size(lhs_size);
		Register dx = m_reg_allocator.allocate(Register::Id::DX);
		dx.set_size(lhs_size);
		mov(lhs_reg, ax);
		if (lhs_size == 2)
			print_code_line("cwd");
		else if (lhs_size == 4)
			print_code_line("cdq");
		else if (lhs_size == 8)
			print_code_line("cqo");
		std::string divcode = lhs_type.is_signed_integer() ? "idiv" : "div";
		print_code_line(mnemonic(divcode, lhs_size), rhs_reg.str());
		mov(isdiv ? ax : dx, lhs_reg);
		m_reg_allocator.release(ax);
		m_reg_allocator.release(dx);
	}
	return lhs_reg;
}

Register CodeGenerator::generate_times(XprNode const *xpr)
{
	XprNode const *lhs_xpr = xpr->get_subxpr(0);
	XprNode const *rhs_xpr = xpr->get_subxpr(1);
	Type const &lhs_type = lhs_xpr->get_xpr_type();
	size_t s = lhs_type.get_size_in_bytes();
	Register lhs_reg = generate_xpr(lhs_xpr);
	Register rhs_reg = generate_xpr(rhs_xpr);
	// result will be stored in lhs
	if (lhs_type.is_signed_integer())
		print_code_line(mnemonic("imul", s), rhs_reg.str(), lhs_reg.str());
	else if (lhs_type.is_unsigned_integer())
	{
		Register ax = m_reg_allocator.allocate(Register::Id::AX);
		ax.set_size(s);
		mov(lhs_reg, ax);
		print_code_line(mnemonic("mul", s), rhs_reg.str());
		mov(ax, lhs_reg);
		m_reg_allocator.release(ax);
	}
	else if (lhs_type.is_floating())
		print_code_line(mnemonic("mul", s, lhs_reg.get_type()), rhs_reg.str(), lhs_reg.str());
	m_reg_allocator.release(rhs_reg);
	return lhs_reg;
}

Register CodeGenerator::generate_dereference(XprNode const *xpr)
{
	// evaluate pointer
	Register addr_reg = generate_xpr(xpr->get_subxpr(0));

	Type const &referenced_type = xpr->get_xpr_type();
	if (referenced_type.is_array())
		return addr_reg;
	if (referenced_type.is_structure())
		return addr_reg;

	size_t s = referenced_type.get_size_in_bytes();
	// allocate register for dereferenced value
	Register val = m_reg_allocator.allocate(referenced_type.is_floating() ? Register::Type::FLOATING : Register::Type::INTEGER);
	val.set_size(s);
	print_code_line(mnemonic("mov", s), indirect(addr_reg), val.str());
	m_reg_allocator.release(addr_reg);
	return val;
}

Register CodeGenerator::generate_array_subscript_rvalue(XprNode const *xpr)
{
	// generate pointer shifted address
	Register addr_reg = generate_pointer_shift(xpr->get_subxpr(0), xpr->get_subxpr(1));

	// For the case of multi-D arrays the pointer is returned
	Type const &elem_type = xpr->get_xpr_type();
	if (elem_type.is_array())
		return addr_reg;
	if (elem_type.is_structure())
		return addr_reg;

	// for the case of single-d-arrays the value is returned
	m_reg_allocator.release(addr_reg);
	Register res_reg = m_reg_allocator.allocate(elem_type.is_floating() ? Register::Type::FLOATING : Register::Type::INTEGER);
	res_reg.set_size(elem_type.get_size_in_bytes());
	print_code_line(mnemonic("mov", res_reg.get_size(), res_reg.get_type()), indirect(addr_reg), res_reg.str());
	return res_reg;
}

Register CodeGenerator::generate_structure_member_rvalue(XprNode const *xpr)
{
	XprNode const *struct_xpr = xpr->get_subxpr(0);
	IdentifierXprNode const *field_xpr = static_cast<IdentifierXprNode const *>(xpr->get_subxpr(1));
	Register addr_reg = generate_structure_member_address(struct_xpr, field_xpr);

	Type const &field_type = field_xpr->get_xpr_type();
	if (field_type.is_array())
		return addr_reg;
	if (field_type.is_structure())
		return addr_reg;

	// for other cases the value is returned
	m_reg_allocator.release(addr_reg);
	Register res_reg = m_reg_allocator.allocate(field_type.is_floating() ? Register::Type::FLOATING : Register::Type::INTEGER);
	res_reg.set_size(field_type.get_size_in_bytes());
	print_code_line(mnemonic("mov", res_reg.get_size(), res_reg.get_type()), indirect(addr_reg), res_reg.str());
	return res_reg;
}

Register CodeGenerator::generate_conditional(XprNode const *xpr)
{
	Label false_label = generate_label(), done_label = generate_label();
	XprNode const *cond = xpr->get_subxpr(0), *tr = xpr->get_subxpr(1), *fl = xpr->get_subxpr(2);

	Type const &ct = cond->get_xpr_type();

	goto_if_false(cond, false_label);

	// generate true to reg, this will be the result register
	Register reg = generate_xpr(tr);
	print_code_line("jmp", done_label.str());

	// generate false somewhere
	print_label(false_label.str());
	Register f = generate_xpr(fl);
	m_reg_allocator.release(f);
	// move false result to return register
	mov(f, reg);

	print_label(done_label.str());
	return reg;
}

std::vector<Register::Id> CodeGenerator::assign_registers_to_parameters(XprNode const *xpr) const
{
	std::vector<Register::Id> regs;

#ifdef _WIN32
	size_t i = 0;
	for (size_t s = 1; s < xpr->get_num_subxprs(); ++s)
	{
		XprNode const *param_xpr = xpr->get_subxpr(s);
		Type const &param_type = param_xpr->get_xpr_type();
		if (param_type.is_integer() || param_type.is_pointer() || param_type.is_array())
			regs.push_back(m_integer_parameters[i++]);
		else if (param_type.is_floating())
			regs.push_back(m_floating_parameters[i++]);
		else
			throw __FILE__ ": parameter register unknown type";
	}
#else
	size_t i_cntr = 0, f_cntr = 0;
	for (int s = 1; s < xpr->get_num_subxprs(); ++s)
	{
		XprNode const *param_xpr = xpr->get_subxpr(s);
		Type const &param_type = param_xpr->get_xpr_type();
		if (param_type.is_integer() || param_type.is_pointer() || param_type.is_array())
		{
			if (i_cntr >= m_integer_parameters.size())
				regs.push_back(Register::Id::NO_REGISTER);
			else
				regs.push_back(m_integer_parameters[i_cntr++]);
		}
		else if (param_type.is_floating())
		{
			if (f_cntr >= m_floating_parameters.size())
				regs.push_back(Register::Id::NO_REGISTER);
			else
				regs.push_back(m_floating_parameters[f_cntr++]);
		}
		else
			regs.push_back(Register::Id::NO_REGISTER);
	}
#endif
	return regs;
}

Register CodeGenerator::generate_function_call(XprNode const *xpr)
{
	Register sp(Register::Id::SP);
	std::string comment;

	// the function type
	Type const &function_type = xpr->get_subxpr(0)->get_xpr_type().referenced_type();
	bool is_vararg = function_type.is_vararg();

	// select caller-saved registers in use. They need to be preserved.
	std::vector<Register::Id> registers_to_save;
	for (auto reg : m_caller_saved)
	{
		if (m_reg_allocator.is_reserved(reg))
		{
			m_reg_allocator.release(reg);
			registers_to_save.push_back(reg);
			push(reg, "Preserve value in register" + Register(reg).str());
		}
	}
	size_t bytes_pushed = 8 * registers_to_save.size();

	// determine which arguments are passed through registers or stack
	std::vector<Register::Id> param_regs = assign_registers_to_parameters(xpr);

	// determine size of arguments on stack
	for (size_t s = xpr->get_num_subxprs() - 1; s > 0; s--)
		if (param_regs[s - 1] == Register::Id::NO_REGISTER)
			bytes_pushed += xpr->get_subxpr(s)->get_xpr_type().get_size_in_bytes();

	// align stack to 16 byte boundary
	size_t framesize = m_local_table.get_size() + bytes_pushed;
	size_t alignment_padding_bytes = (m_stack_alignment - framesize % m_stack_alignment) % m_stack_alignment;
	if (alignment_padding_bytes != 0)
		sub(alignment_padding_bytes, sp, "Stack alignment before 64-bit function call");

	// pass arguments through stack
	for (size_t s = xpr->get_num_subxprs() - 1; s > 0; s--)
	{
		size_t r = s - 1;
		Register xpr_reg = generate_xpr(xpr->get_subxpr(s));
		if (param_regs[r] == Register::Id::NO_REGISTER)
			push(xpr_reg, "Push argument #" + std::to_string(r));
		else
		{
			// allocate register defined by calling convention
			Register param_reg = m_reg_allocator.allocate(param_regs[r]);
			param_reg.set_size(xpr_reg.get_size());
			// copy value and release expression register
			comment = "Move argument #" + std::to_string(r) + " to " + param_reg.str();
			mov(xpr_reg, param_reg, comment);
#ifdef _WIN32
			if (is_vararg && param_reg.get_type() == Register::Type::FLOATING)
			{
				Register pair = m_integer_parameters[s - 1];
				pair.set_size(param_reg.get_size());
				mov(xpr_reg, pair);
			}
#endif
		}
		m_reg_allocator.release(xpr_reg);
	}

#ifndef _WIN32
	// for Unix vararg functions the number of vector arguments should be written into AL
	if (is_vararg)
	{
		size_t num_vector_parameters = 0;
		for (auto const &id : param_regs)
			if (id != Register::Id::NO_REGISTER && Register(id).get_type() == Register::Type::FLOATING)
				++num_vector_parameters;
		Register ax = m_reg_allocator.allocate(Register::Id::AX);
		ax.set_size(4);
		comment = "Number of vector arguments for vararg functions";
		mov(num_vector_parameters, ax, comment);
		m_reg_allocator.release(ax);
	}
#endif

#ifdef _WIN32
	// shadow space
	sub(32, sp, "shadow space");
#endif

	// function call
	Register func_reg = generate_xpr(xpr->get_subxpr(0));
	print_code_line(mnemonic("call", func_reg.get_size()), "*" + func_reg.str());
	m_reg_allocator.release(func_reg);

#ifdef _WIN32
	// remove shadow space
	add(32, sp, "shadow space");
#endif

	// release parameter registers
	for (auto reg : param_regs)
		if (reg != Register::Id::NO_REGISTER)
			m_reg_allocator.release(reg);

	// realign stack
	if (alignment_padding_bytes != 0)
		add(alignment_padding_bytes, sp, "Stack realignment after 64-bit function call");

	// pop to-save registers from stack
	// ax and xmm0 are intentionally excluded from to-save list
	for (auto it = registers_to_save.rbegin(); it != registers_to_save.rend(); ++it)
	{
		Register reg = Register(*it);
		pop(reg, "Restore value in register " + reg.str());
		m_reg_allocator.allocate(reg.get_id());
	}

	// result is in eax or xmm0, return to caller
	if (!xpr->get_xpr_type().is_void())
	{
		Register::Id ret_id = xpr->get_xpr_type().is_floating() ? Register::Id::XMM0 : Register::Id::AX;
		Register ret_reg = m_reg_allocator.allocate(ret_id);
		ret_reg.set_size(xpr->get_xpr_type().get_size_in_bytes());
		Register ret = m_reg_allocator.allocate(xpr->get_xpr_type().is_floating() ? Register::Type::FLOATING : Register::Type::INTEGER);
		ret.set_size(xpr->get_xpr_type().get_size_in_bytes());
		std::string comment = "move return value to " + ret.str();
		mov(ret_reg, ret, comment);
		m_reg_allocator.release(ret_reg);
		return ret; // dummy allocation for result
	}

	// TODO void functions return a dummy allocated register
	return m_reg_allocator.allocate(Register::Type::INTEGER);
}

Register CodeGenerator::generate_floating_constant(FloatingConstant const &xpr)
{
	auto value = xpr.get_value();
	for (auto const &[d, lab] : m_float_table)
	{
		if (d == value)
		{
			Register reg = m_reg_allocator.allocate(Register::Type::FLOATING);
			reg.set_size(xpr.get_xpr_type().get_size_in_bytes());
			print_code_line(mnemonic("mov", reg.get_size(), reg.get_type()), lab.str() + "(%rip)", reg.str());
			return reg;
		}
	}
	throw __FILE__ ": Did not find double constant in floating table :(";
}

Register CodeGenerator::generate_string_literal(StringLiteralNode const &xpr)
{
	auto const &xpr_str = xpr.get_string();
	for (auto const &[str, lab] : m_string_table)
	{
		if (str == xpr_str)
		{
			Register reg = m_reg_allocator.allocate(Register::Type::INTEGER);
			reg.set_size(xpr.get_xpr_type().get_size_in_bytes());
			print_code_line(mnemonic("lea", reg.get_size()), lab.str() + "(%rip)", reg.str());
			return reg;
		}
	}
	throw __FILE__ ": Did not find string literal in string table :(";
}

Register CodeGenerator::generate_unary_mp(XprNode const &xpr)
{
	auto arg_xpr = xpr.get_subxpr(0);
	Type const &arg_type = arg_xpr->get_xpr_type();
	size_t arg_size = arg_type.get_size_in_bytes();

	Register reg = generate_xpr(arg_xpr);
	if (xpr.get_id() == XprNode::Id::UNARY_MINUS)
	{
		if (arg_type.is_floating())
		{
			// we compute 0.0 - reg
			Register result_reg = m_reg_allocator.allocate(Register::Type::FLOATING);
			result_reg.set_size(arg_size);
			print_code_line("pxor", result_reg.str(), result_reg.str());
			print_code_line(mnemonic("sub", result_reg.get_size(), result_reg.get_type()), reg.str(), result_reg.str(), "floating point negate (0.0 - " + reg.str() + ")");
			m_reg_allocator.release(reg);
			reg = result_reg;
		}
		else
			print_code_line(mnemonic("neg", reg.get_size()), reg.str());
	}
	return reg;
}

Register CodeGenerator::generate_address_of(XprNode const &xpr)
{
	size_t ptr_size = Type::int_type().pointer_to().get_size_in_bytes(); // pointer size
	auto lvalue = xpr.get_subxpr(0);									 // the lvalue expression

	if (lvalue->get_id() == XprNode::Id::IDENTIFIER)
	{
		IdentifierXprNode const *id_xpr = static_cast<IdentifierXprNode const *>(lvalue);
		// if lvalue is an id, then its address is stored in a newly allocated register
		// allocate register and set to pointer size
		Register reg = m_reg_allocator.allocate(Register::Type::INTEGER);
		reg.set_size(ptr_size);
		// lookup the memory address of the id in the local variable table
		std::string const &varname = id_xpr->get_identifier();
		std::string memname = m_local_table.lookup(varname);
		// load effective address into register
		std::string comment = "load  &" + varname + " to " + reg.str();
		print_code_line(mnemonic("lea", ptr_size), memname, reg.str(), comment);
		return reg;
	}
	else if (lvalue->get_id() == XprNode::Id::DEREFERENCE)
		return generate_xpr(lvalue->get_subxpr(0));
	else if (lvalue->get_id() == XprNode::Id::ARRAY_SUBSCRIPT)
		return generate_pointer_shift(lvalue->get_subxpr(0), lvalue->get_subxpr(1));
	else if (lvalue->get_id() == XprNode::Id::STRUCTURE_PTR_MEMBER || lvalue->get_id() == XprNode::Id::STRUCTURE_MEMBER)
	{
		XprNode const *struct_xpr = lvalue->get_subxpr(0);
		IdentifierXprNode const *field_xpr = static_cast<IdentifierXprNode const *>(lvalue->get_subxpr(1));
		return generate_structure_member_address(struct_xpr, field_xpr);
	}
	throw __FILE__ ": unprocessed lvalue type for address-of operator";
}

Register CodeGenerator::generate_crement(XprNode const &xpr)
{
	auto child_xpr = xpr.get_subxpr(0);
	std::string memname, comment;
	Type const &result_type = child_xpr->get_xpr_type();
	size_t result_size = result_type.get_size_in_bytes();
	XprNode::Id op_id = xpr.get_id();

	bool is_increment = op_id == XprNode::Id::PREINCREMENT || op_id == XprNode::Id::POSTINCREMENT;

	// determine memory location of lvalue
	if (child_xpr->get_id() == XprNode::Id::IDENTIFIER)
	{
		IdentifierXprNode const *id_xpr = static_cast<IdentifierXprNode const *>(child_xpr);
		std::string varname = id_xpr->get_identifier();
		memname = m_local_table.lookup(varname);
		comment = (is_increment ? "++ " : "-- ") + varname;
	}
	else if (child_xpr->get_id() == XprNode::Id::DEREFERENCE || child_xpr->get_id() == XprNode::Id::ARRAY_SUBSCRIPT)
	{
		Register addr_reg;
		if (child_xpr->get_id() == XprNode::Id::DEREFERENCE)
			addr_reg = generate_xpr(child_xpr->get_subxpr(0));
		else
			addr_reg = generate_pointer_shift(child_xpr->get_subxpr(0), child_xpr->get_subxpr(1));
		memname = indirect(addr_reg);
		m_reg_allocator.release(addr_reg);
		comment = (is_increment ? "++ " : "-- ") + memname;
	}

	Register result_reg;
	if (op_id == XprNode::Id::POSTINCREMENT || op_id == XprNode::Id::POSTDECREMENT)
	{
		// fetch result into register
		result_reg = m_reg_allocator.allocate(Register::Type::INTEGER);
		result_reg.set_size(result_size);
		comment = "fetch result before in/decrement as expression value";
		print_code_line(mnemonic("mov", result_reg.get_size()), memname, result_reg.str(), comment);
	}

	// perform increment / decrement
	if (result_type.is_floating())
		throw __FILE__ ": Floating increment is unimplemented";
	size_t incr_size = result_type.is_pointer() ? result_type.referenced_type().get_size_in_bytes() : 1;
	print_code_line(mnemonic(is_increment ? "add" : "sub", result_size), immediate(incr_size), memname, comment);

	if (op_id == XprNode::Id::PREINCREMENT || op_id == XprNode::Id::PREDECREMENT)
	{
		// fetch result into register
		result_reg = m_reg_allocator.allocate(Register::Type::INTEGER);
		result_reg.set_size(result_size);
		comment = "fetch in/decremented result as expression value";
		print_code_line(mnemonic("mov", result_reg.get_size()), memname, result_reg.str(), comment);
	}

	return result_reg;
}

Register CodeGenerator::generate_comma(XprNode const &xpr)
{
	m_reg_allocator.release(generate_xpr(xpr.get_subxpr(0)));
	return generate_xpr(xpr.get_subxpr(1));
}
