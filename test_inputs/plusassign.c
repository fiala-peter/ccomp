#include <stdio.h>

int main(void)
{
    {
        int a = 0, b = 1, c = 2;

        printf("%d %d %d\n", a, b, c);

        a += 2;
        b += c;

        printf("%d %d %d\n", a, b, c);
    }

    {
        double da = 0.0, db = 1.0, dc = 2.0;
        printf("%f %f %f\n", da, db, dc);

        da += 2.0;
        db += dc;

        printf("%f %f %f\n", da, db, dc);
    }

    {
        int a[] = {1, 2, 3, 4};
        int *p;
        p = a;
        p += 2;
        printf("%d\n", *p);
    }

    return 0;
}
