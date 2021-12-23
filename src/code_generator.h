/**
 * @file code_generator.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class ::CodeGenerator
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef CODE_GENERATOR_H_INCLUDED
#define CODE_GENERATOR_H_INCLUDED

#include "ast_node.h"
#include "compound_node.h"
#include "floating_constant.h"
#include "identifier_xpr_node.h"
#include "label.h"
#include "local_table.h"
#include "register.h"
#include "register_allocator.h"
#include "statement_node.h"
#include "string_literal_node.h"
#include "translation_unit.h"
#include "xpr_node.h"
#include "integer_constant.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <utility>

/**
 * @brief class responsible for generating x86_64 assembly code from a syntax tree
 */
class CodeGenerator
{
public:
	Label generate_label();

	void mov(Register const &from, Register const &to, std::string const &comment = "") const;
	void mov(long long val, Register const &to, std::string const &comment = "") const;
	void add(long long val, Register const &to, std::string const &comment = "") const;
	void sub(long long val, Register const &to, std::string const &comment = "") const;
	void push(Register const &reg, std::string const &comment = "") const;
	void pop(Register const &reg, std::string const &comment = "") const;
	void cmp(Register const &a, Register const &b, std::string const &comment = "") const;

	template <class T>
	static std::string immediate(T const &num)
	{
		return std::string("$") + std::to_string(num);
	}

	static std::string indirect(Register const &base)
	{
		return "(" + base.str() + ")";
	}

	static std::string indirect(Register const &base, Register const &offset, size_t s = 1)
	{
		if (s == 1)
			return "(" + base.str() + "," + offset.str() + ")";
		else
			return "(" + base.str() + "," + offset.str() + "," + std::to_string(s) + ")";
	}

	void print_code_line(std::string const &mnemonic = "", std::string const &op1 = "", std::string const &op2 = "", std::string const &comment = "") const;
	void print_label(std::string const &label, std::string const &comment = "") const;

	void generate_translation_unit(TransUnitNode *trans);

	void generate_function_prolog(FunctionNode *function);
	void generate_function_epilog(FunctionNode *function);

	/**
	 * @brief Generate assembly code for a function
	 * 
	 * @param[in] function The function AST node
	 */
	void generate_function(FunctionNode *function);

	void enter_loop(Label const &continue_label, Label const &break_label);
	void exit_loop();

	static bool is_relational_expression(XprNode::Id op);
	void goto_if_false(XprNode const *cond, Label const &label);

	void generate_statement(StmNode const &statement);
	void generate_block(CompoundNode const &block);
	void generate_if(StmNode const &statement_tree);
	void generate_while(StmNode const &statement_tree);
	void generate_do(StmNode const &statement_tree);
	void generate_for(StmNode const &statement_tree);
	void generate_expression_stm(StmNode const &statement_tree);
	void generate_return(StmNode const &statement_tree);
	void generate_break_continue(StmNode const &root);

	Register generate_integer_constant(XprNode const *xpr);

	Register generate_assignment(XprNode const *xpr);
	Register generate_plusassignment(XprNode const *xpr);

	Register generate_identifier_rvalue(IdentifierXprNode const *xpr);

	Register generate_pointer_shift(XprNode const *lhs, XprNode const *rhs, bool is_plus = true);

	Register generate_structure_member_address(XprNode const *str_ptr_xpr, IdentifierXprNode const *field_xpr);

	Register generate_binary_additive(XprNode const *xpr);

	Register fun2ptr_cast(Register const &reg);

	Register generate_cast(XprNode const *xpr);

	Register generate_relational(XprNode const *xpr);

	void generate_short_circuit_rec(XprNode const *xpr, XprNode::Id id, std::string const &jump_cond, Label const &lab);

	Register generate_logical_andor(XprNode const *xpr);

	Register generate_divmod(XprNode const *xpr);

	Register generate_times(XprNode const *xpr);

	Register generate_dereference(XprNode const *xpr);

	Register generate_array_subscript_rvalue(XprNode const *xpr);

	Register generate_structure_member_rvalue(XprNode const *xpr);

	Register generate_conditional(XprNode const *xpr);

	Register generate_xpr(XprNode const *xpr_tree);

	std::vector<Register::Id> assign_registers_to_parameters(XprNode const *xpr) const;

	/**
	 * @brief Generate assembly code for a function call expression
	 * 
	 * @param xpr_tree expression tree where the root is a function call node
	 * @return the register where the function value is stored
	 */
	Register generate_function_call(XprNode const *xpr_tree);

	Register generate_string_literal(StringLiteralNode const &xpr_tree);

	Register generate_floating_constant(FloatingConstant const &flc);

	Register generate_unary_mp(XprNode const &xpr_tree);

	Register generate_address_of(XprNode const &xpr_tree);

	Register generate_crement(XprNode const &xpr_tree);

	Register generate_comma(XprNode const &xpr_node);

	static std::string mnemonic(std::string const &name, size_t s, Register::Type type = Register::Type::INTEGER);

	Register int2int_cast(Register const &src, Type const &from, Type const &to);

	Register int2floating_cast(Register const &src, Type const &from, Type const &to);

	Register floating2int_cast(Register const &src, Type const &from, Type const &to);

	CodeGenerator(std::ostream &os = std::cout);

	void enter_scope(SymbolNode const *symbol_pointer);

	void exit_scope();

	void generate_goto(Label const &lab) const;

private:
	std::ostream &m_os;
	int m_label_counter;
	int m_scope_counter;
	RegisterAllocator m_reg_allocator;
	LocalTable m_local_table;
	std::vector<std::pair<std::string, Label>> m_string_table;
	std::vector<std::pair<double, Label>> m_float_table;
	Label m_actual_function_return_label;
	std::list<Label> m_continue_stack;
	std::list<Label> m_break_stack;

	std::vector<Register::Id> m_callee_saved;
	std::vector<Register::Id> m_caller_saved;
	std::vector<Register::Id> m_integer_parameters;
	std::vector<Register::Id> m_floating_parameters;

	size_t const m_stack_alignment = 16;
};

#endif
