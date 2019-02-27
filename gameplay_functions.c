#include <stdlib.h>
#include <string.h>
#include "gameplay_header.h"

void processMove(int (*board)[3][3], char (*buffer)[2], int player)
{
	char row = (*buffer)[0];
	char column = (*buffer)[1];
	int i, j;
	if(row == 'A')
		i = 0;
	else if(row == 'B')
		i = 1;
	else
		i = 2;
		
	if(column == '1')
		j = 0;
	else if(row == '2')
		j = 1;
	else
		j = 2;
				
	if((*board)[i][j] != 0)
		strcpy((*buffer), "NO");
		
	else
	{
		strcpy((*buffer), "OK");
		(*board)[i][j] = player;
	}
}
