#include <stdio.h>

int main(void)
{
	int n, tmp, mirror;
	printf("Enter a number: ");
	scanf("%d", &n);
	mirror = 0;
	tmp = n;
	while (tmp != 0)
	{
		mirror = mirror * 10;
		mirror = mirror + tmp % 10;
		tmp = tmp / 10;
	}
	printf("The mirror image of %d is %d.\n", n, mirror);
	return 0;
}
