#include <stdio.h>

int plain_int;

static int static_int;
extern int extern_int;

int main(void)
{
	char a = 'a';
	char b = 'b';
	char c = '\0x23';
	char d = '\131';
	
	static int local_static_int;

	static_int = 1;
	plain_int = 2;
	local_static_int = 3;
	b = 4;

	printf("%d %d %d\n", static_int, plain_int, local_static_int);
	return 0;
}

