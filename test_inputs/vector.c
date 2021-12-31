#include <stdio.h>

double sum(double a[], int n)
{
	int i;
	double sum;
	for (sum = 0.0, i = 0; i < n; )
		sum = sum + a[i++];
	return sum;
}

void load(double a[], int n)
{
	while (n-- != 0)
		scanf("%lf", a++);
}

int main(void)
{
	enum {NUM_ELEMENTS = 5};
	double vector[NUM_ELEMENTS];
	printf("Adj meg %d elemet: ", NUM_ELEMENTS);
	load(vector, NUM_ELEMENTS);
	printf("Az osszeg: %f\n", sum(vector, NUM_ELEMENTS));
	return 0;
}
