#include "serializable.h"

std::ostream &operator<<(std::ostream &os, Serializable const &ser)
{
	ser.print(os);
	return os;
}
