#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include <stddef.h>

extern void *malloc(size_t size);
extern void free(void *ptr);
extern void qsort(void *, size_t, size_t, int (*)(void const *, void const *));
extern int rand(void);

#endif
