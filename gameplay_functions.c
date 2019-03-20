#include <stdlib.h>
#include <string.h>
#include "gameplay_header.h"

int ProcessMove(int (*board)[3][3], char buffer[5], int player)
{
	char row = buffer[0];
	char column = buffer[1];
	printf("[DEBUG] ProcessMove row = %c column = %c\n", row, column);
	int i, j;
	if(row == 'A')
		i = 0;
	else if(row == 'B')
		i = 1;
	else if (row == 'C')
		i = 2;
	else 
		return 0;
		
	if(column == '1')
		j = 0;
	else if(column == '2')
		j = 1;
	else if (column == '3')
		j = 2;
	else
		return 0;
	
	if ((*board)[i][j] != 0)
		return 0;
	else
		(*board)[i][j] = player;
	
	return 1;
}

int ValidateMove(char buffer[5])
{
	printf("[DEBUG] ValidateMove buffer = %s\n", buffer);
	if(buffer[0] < 'A' || buffer[0] > 'C' || buffer[1] < '1'
		|| buffer[1] > '3' || buffer[2] != '\n')
	{
		printf("ERROR: BAD MESSAGE FORMAT!\n");
		return 0;
	}
	return 1;
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


