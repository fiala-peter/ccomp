#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED

#include <stddef.h>

typedef struct FILE FILE;

extern int printf(char const *, ...);
extern int scanf(char const *, ...);
extern int fprintf(FILE *fid, char const *, ...);
extern int fscanf(FILE *fid, char const *, ...);
extern FILE *fopen(char const *fname, char const *mode);
extern int fclose(FILE *fid);
extern char *fgets(char *str, int n, FILE *stream);
extern int atoi(char const *str);
extern int fflush(FILE *stream);

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define stdin stdin
#define stdout stdout
#define stderr stderr
#define EOF (-1)

#endif
