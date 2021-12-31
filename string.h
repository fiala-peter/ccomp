#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <stddef.h>

extern size_t strlen(char *str);
extern char *strcpy(char *to, char const *from);
extern int strcmp(char const *str1, char const *str2);

#endif
