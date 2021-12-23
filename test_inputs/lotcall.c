#include <stdio.h>

int main()
{
    int t[10], i;
    scanf("%d%d%d%d%d%d%d%d", t, t+1, t+2, t+3, t+4, t+5, t+6, t+7);
    for (i = 0; i < 8; i++)
        printf("%d ", t[i]);
    return 0;
}

