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

int ValidateServerMessage(char buffer[5])
{
	if (buffer[0] != 'O')
		return 0;
	if (buffer[1] != 'K')
		return 0;
	
	//opponent move || W(player)
	if (buffer[2] != 'A' || buffer[2] != 'B' || buffer[2] != 'C' || buffer[2] != 'W')
		return 0;
	if (buffer[3] != '1' || buffer[3] != '2' || buffer[3] != '3')
		return 0;
	
	if (buffer[4] != '\n')
		return 0;
	
	return 1;
}
