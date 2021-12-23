int printf(char const *, ...);

int main(void)
{
	char c;

	for (c = 2; c < 8; ++c)
	{
		char d;
		for (d = 0; d < 0x10; d++)
		{
			char e;
			e = 0x10 * c + d;
			printf("%3d:%c  ", (int)e, e);
		}
		printf("\n");
	}
	return 0;
}
