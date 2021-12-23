/**
 * @file serializable.h
 * @author Peter Fiala (fiala@hit.bme.hu)
 * @brief 
 * @version 0.1
 * @date 2021-01-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef SERIALIZABLE_H_INCLUDED
#define SERIALIZABLE_H_INCLUDED

#include <iostream>

/** @brief base class for the definition of the serializable interface */
struct Serializable
{
	/** \brief print the contents to an output stream */
	virtual void print(std::ostream &os, size_t level = 0) const = 0;
};

/** @brief operator to put the contents to an output stream */
std::ostream &operator<<(std::ostream &os, Serializable const &ser);

#endif
