#include <stdlib.h>
#include <string.h>
#include "client_header.h"

void printBoard(int (*board)[3][3])
{
	int i, j;
	char row = 'A';
	for(i = 0; i < 3; i++)
	{
		printf("%c - | %d | %d | %d |\n",row++, (*board)[i][0], (*board)[i][1], (*board)[i][2]);
		if(i == 2)
			printf("      |   |   |\n    | 1 | 2 | 3 |\n");
	}
}
