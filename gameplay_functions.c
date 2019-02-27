#include <stdlib.h>
#include <string.h>
#include "gameplay_header.h"

void validateAndProcess(int (*board)[3][3], char (*buffer)[2], 
	char (*res)[2], int player)
{
	char row = (*buffer)[0];
	char column = (*buffer)[1];
	//printf("[DEBUG] processMove row = %c column = %c\n", row, column);
	int i, j;
	if(row == 'A')
		i = 0;
	else if(row == 'B')
		i = 1;
	else
		i = 2;
		
	if(column == '1')
		j = 0;
	else if(column == '2')
		j = 1;
	else
		j = 2;
				
	if((*board)[i][j] != 0)
		strcpy((*res), "NO");
		
	else
	{
		strcpy((*res), "OK");
		//printf("[DEBUG] processMove i = %d j = %d\n", i, j);
		(*board)[i][j] = player;
	}
}

void checkForWinner(int (*board)[3][3], char (*res)[2])
{	
	// CHECKING ROWS
	if ( ( (*board)[0][0] != 0 && (*board)[0][0] == (*board)[0][1] && (*board)[0][0] == (*board)[0][2])
	|| ( (*board)[1][0] != 0 && (*board)[1][0] == (*board)[1][1] && (*board)[1][0] == (*board)[1][2])
	|| ( (*board)[2][0] != 0 && (*board)[2][0] == (*board)[2][1] && (*board)[2][0] == (*board)[2][2])
	// CHECKING COLUMNS
	|| ( (*board)[0][0] != 0 && (*board)[0][0] == (*board)[1][0] && (*board)[0][0] == (*board)[2][0])
	|| ( (*board)[0][1] != 0 && (*board)[0][1] == (*board)[1][1] && (*board)[0][1] == (*board)[2][1]) 
	|| ( (*board)[0][2] != 0 && (*board)[0][2] == (*board)[1][2] && (*board)[0][2] == (*board)[2][2]) 
	// CHECKING X
	|| ( (*board)[0][0] != 0 && (*board)[0][0] == (*board)[1][1] && (*board)[0][0] == (*board)[2][2]) 
	|| ( (*board)[0][2] != 0 && (*board)[0][2] == (*board)[1][1] && (*board)[0][2] == (*board)[2][0]) )
	{
		(*res)[0] = 'W';
	}
}


