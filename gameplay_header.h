#include <stdio.h>
#include <stdlib.h>

int ValidateMove(char buffer[5]);
	
int ProcessMove(int(*board)[3][3], char buffer[5], int player);

void checkForWinner(int (*board)[3][3], char (*res)[2]);