#include <stdio.h>

typedef unsigned long number_t;

number_t fibonacci(int n)
{
	if (n <= 1)
		return (number_t)n;
	return fibonacci(n-2) + fibonacci(n-1);
}

int main(void)
{
	int n, i;
	printf("Meddig erdekelnek a Fibonacci-szamok?: ");
	scanf("%d", &n);
	for (i = 0; i < n; ++i)
		printf("F(%d) = %lld\n", i, fibonacci((number_t)i));
	return 0;
}
