#include <stdio.h>

typedef long position_t;
typedef int bool_t;

#define TRUE 1

enum
{
	BOARD_SIZE = 8, NUM_MOVES = 8
};

bool_t is_inside(position_t x, position_t y)
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool_t can_move(position_t x, position_t y, bool_t was_there[])
{
	return is_inside(x, y) && was_there[BOARD_SIZE * x + y] == 0;
}

int main(void)
{
	position_t x = 0, y = 0, moves[NUM_MOVES][2] = {
		{-2, -1},
		{-2, 1},
		{-1, -2},
		{-1, 2},
		{1, -2},
		{1, 2},
		{2, -1},
		{2, 1}};
	size_t i, cntr = 0;
	bool_t was_there[BOARD_SIZE * BOARD_SIZE];

	printf("The trapped knight\n");

	for (i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
		was_there[i] = 0;

	printf("Enter the starting coordinates:\n");
	scanf("%ld%ld", &x, &y);
	printf("The knight starts from (%ld,%ld)\n", x, y);
	was_there[BOARD_SIZE * x + y] = 1;

	while (TRUE)
	{
		size_t s;

		for (s = 0; s < NUM_MOVES; ++s)
		{
			position_t *p = moves[s], x_to = x + p[0], y_to = y + p[1];

			if (can_move(x_to, y_to, was_there))
			{
				x = x_to, y = y_to;
				was_there[BOARD_SIZE * x + y] = 1;

				++cntr;
				printf("Step %2lu: Jumped to (%ld,%ld)\n", cntr, x, y);
				break;
			}
		}
		if (s == NUM_MOVES)
			break;
	}

	printf("stuck at (%ld, %ld) :(\n", x, y);

	return 0;
}
