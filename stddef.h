#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED

#define NULL ((void *)0)
typedef unsigned long size_t;
typedef long ptrdiff_t;
typedef long max_align_t;
typedef int wchar_t;

#define offsetof(st, m) ((size_t)&(((st *)0)->m))

#endif // STDDEF_H_INCLUDED
