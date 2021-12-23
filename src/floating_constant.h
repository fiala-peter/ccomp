/**
 * @file floating_constant.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class FloatingConstant
 * @version 0.1
 * @date 2021-01-30
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef FLOATING_CONSTANT_H_DEFINED
#define FLOATING_CONSTANT_H_DEFINED

#include "xpr_node.h"
#include "integer_constant.h"

/** @brief Representation of floating point constant expressions */
class FloatingConstant : public XprNode
{
public:
	FloatingConstant();

	bool is_constant_expression() const override { return false; };

	void set_value(double v);

	double get_value() const { return m_value; }

	void print(std::ostream &os, size_t level = 0) const override;

	XprNode *clone() override { return new FloatingConstant(*this); }

private:
	double m_value;
};

#endif // FLOATING_CONSTANT_H_DEFINED
