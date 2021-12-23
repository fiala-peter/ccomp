#include <stdlib.h>
#include <stdio.h>

typedef struct list_elem {
	struct point {double x; double y; } p;
	struct list_elem *next;
} list_elem, *list_ptr;

void print_point(struct point *p)
{
	printf("(%f %f)", p->x, p->y);
}

list_ptr new_point_elem(double x, double y)
{
	list_ptr q = malloc(sizeof *q);
	q->p.x = x;
	q->p.y = y;
	q->next = NULL;
	return q;
}

int main(void)
{
	list_ptr p = NULL;
	int i;

	for (i = 0; i < 1000; ++i)
	{
		list_ptr q = new_point_elem((double)i, (double)-i);
		q->next = p;
		p = q;
	}

	while (p != NULL)
	{
		list_ptr q = p;
		print_point(&q->p);
		p = q->next;
		free(q);
	}
	return 0;
}
