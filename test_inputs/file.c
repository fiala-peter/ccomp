typedef int FILE;
typedef long unsigned size_t;

FILE *fopen(char *, char *);
int fclose(FILE *);
int printf(char *, ...);
int fprintf(FILE *, char *, ...);
int scanf(char *, ...);
void *malloc(size_t);
void free(void *);

int main(void)
{
	FILE *f;
	int *p, i, n;
	f = fopen("fajl.txt", "wb");
	fprintf(f, "Szevasz!");
	fclose(f);

	printf("hanyat?: ");
	scanf("%d", &n);

	p = malloc(n * sizeof(int));
	for (i = 0; i < n; ++i)
	{
		p[i] = i;
		printf("%p: %d\n", p + i, p[i]);
	}
	free(p);
	return 0;
}
