typedef unsigned long size_t;

int printf(char *, ...);
int scanf(char *, ...);
void *malloc(size_t);
void free(void *);

int is_prime(int d, int primes[])
{
	int i;
	for (i = 0; primes[i] * primes[i] <= d; i++)
		if (d % primes[i] == 0)
			return 0;
	return 1;
}

int main(void)
{
	int n, *primes, i, d;

	printf("How many primes should I list?: ");
	scanf("%d", &n);
	primes = (int *)malloc(n * sizeof(int));
	primes[0] = 2, primes[1] = 3;
	d = 5;
	for (i = 2; i < n; d = d + 1)
		if (is_prime(d, primes))
		{
			primes[i++] = d;
			printf("The %5d-th prime is %5d\n", i, primes[i-1]);
		}
	free(primes);
	return 0;
}
