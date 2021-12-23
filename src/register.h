/**
 * @file register.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief declaration of class ::Register
 * @version 0.1
 * @date 2021-12-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef REGISTER_H_INCLUDED
#define REGISTER_H_INCLUDED

#include <string>

/**
 * @brief class ::Register is used to represent an integer or
 * floating point register of the processor. The class provides 
 * further assembly language functionalities as well.
 */
class Register
{
public:
	enum class Id
	{
		AX,
		BX,
		CX,
		DX,
		DI,
		SI,
		BP,
		SP,
		R8,
		R9,
		R10,
		R11,
		R12,
		R13,
		R14,
		R15,
		XMM0,
		XMM1,
		XMM2,
		XMM3,
		XMM4,
		XMM5,
		XMM6,
		XMM7,
		XMM8,
		XMM9,
		XMM10,
		XMM11,
		XMM12,
		XMM13,
		XMM14,
		XMM15,
		NO_REGISTER
	};

	enum class Type
	{
		INTEGER,
		FLOATING
	};

	/**
	 * @brief Get the type of the register (integer or floating point)
	 */
	Type get_type() const
	{
		return get_type(m_id);
	}

	/**
	 * @brief Get the type of a specified register
	 * 
	 * @param id the register's id
	 * @return the register's type (integer or floating point)
	 */
	static Type get_type(Id id)
	{
		return id < Id::XMM0 ? Type::INTEGER : Type::FLOATING;
	}

	/** \brief default constructor */
	Register() {}

	/** \brief Constructor from id and size
	 * \param[in] id the register's id
	 * \param[in] size the size of the stored number in bytes
	*/
	Register(Id id, size_t size = 8)
		: m_id(id), m_size(size){};

	/** \brief return the register's id */
	Id get_id() const { return m_id; }

	/** \brief return a textual identifier of the register */
	std::string str() const;

	/** \brief return the stored number's size in bítes */
	size_t get_size() const { return m_size; };

	/** \brief set the stored number's size in byates */
	void set_size(size_t size) { m_size = size; }

private:
	Id m_id;
	size_t m_size;
};

#endif
