struct a
{
	int a, b, c, d, e, f, g, h;
};

struct a f(int i1, int i2, int i3)
{
	struct a b;
	b.a = i1;
	b.b = i2;
	b.c = i3;
	return b;
}

int main(void)
{
	struct a c;
	c = f(1, 2, 3);
	int q = c.a;
	int s = c.h;
	return 0;
}
