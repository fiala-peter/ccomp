int printf(char *, ...);
int scanf(char *, ...);

int main(void)
{
	double a, b, x, eps;

	printf("We search for the root of x^2-2 between a and b with precision eps.\n");
	printf("a, b, eps: ");
	scanf("%lf%lf%lf", &a, &b, &eps);

	while (b - a > eps)
	{
		double x;
		x = (a + b) / 2.0;
		if (x*x - 2.0 < 0.0)
			a = x;
		else
			b = x;
	}
	printf("The root is: %.10f\n", a);
	return 0;
}
