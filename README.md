# ccomp
A naive C compiler

ccomp is a C compiler written in C++.
ccomp compiles C programs based on the C90 standard and produces x86_64 assembly code as output.

ccomp compiles C code in the following stages
- The preprocessor processes a set of source files into a single translation unit
- A lexer transforms the translation unit into a linear sequence of tokens
- The parser transforms the sequence of tokens into an abstract syntax tree
- The code generator generates assebly language code from the syntax tree
