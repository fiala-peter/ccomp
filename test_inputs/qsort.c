#include <stdlib.h>
#include <stdio.h>

int cmp(void const *a, void const *b)
{
	return *(int const *)a - *(int const *)b;
}

void star_print(int v)
{
	int i;
	for (i = 0; i < v; ++i)
		printf("*");
	printf("\n");
}

typedef void (*print_func_ptr)(int);

void print_array(int a[], size_t n, print_func_ptr print)
{
	size_t i;
	for (i = 0; i < n; ++i)
		print(a[i]);
}

int main(void)
{
	enum { NUM_ELEMENTS = 20, MAX_ELEM = 100 };

	int a[NUM_ELEMENTS], i;
	for (i = 0; i < NUM_ELEMENTS; ++i)
		a[i] = rand() % MAX_ELEM;
	print_array(a, NUM_ELEMENTS, star_print);
	printf("\n");
	qsort(a, 20, sizeof(int), cmp);
	print_array(a, NUM_ELEMENTS, star_print);
	return 0;
}
