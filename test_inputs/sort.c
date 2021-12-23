int printf(char const *, ...);
int scanf(char const *, ...);
int rand();

typedef double real_t;
typedef unsigned long size_t;

void swap(real_t *a, real_t *b)
{
    real_t x;
    x = *a;
    *a = *b;
    *b = x;
}

void bubble(real_t a[], size_t n)
{
    size_t i, j;
    for (i = 0; i < n - 1; ++i)
        for (j = 0; j < n - i - 1; ++j)
            if (a[j] >= a[j + 1])
                swap(a + j, a + j + 1);
}

void quick(real_t a[], size_t n)
{
    size_t i, j;
    real_t pivot;

    i = 0, j = n;
    pivot = a[n / 2];

    while (i < j)
    {
        if (a[i] < pivot)
            ++i;
        else
        {
            --j;
            if (a[j] <= pivot)
                swap(a + i, a + j);
        }
    }

    if (i > 1)
        quick(a, i);
    if (a[i] == pivot)
    {
        if (n - i - 1 > 1)
            quick(a + i + 1, n - i - 1);
    }
    else
    {
        if (n - i > 1)
            quick(a + i, n - i);
    }
}

int main(void)
{
    enum {SIZE = 1000, MAX_ELEM = 2000};
    size_t i;
    real_t array[SIZE];

    for (i = 0; i < SIZE; ++i)
        array[i] = rand() % MAX_ELEM;
    for (i = 0; i < SIZE; ++i)
        printf("%.0f, ", array[i]);
    printf("\n");

    quick(array, SIZE);

    for (i = 0; i < SIZE; ++i)
        printf("%.0f, ", array[i]);
    printf("\n");
    return 0;
}
