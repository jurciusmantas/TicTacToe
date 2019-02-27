#include <stdio.h>
#include <stdlib.h>

void validateAndProcess(int (*board)[3][3], char (*buffer)[2],
	char (*res)[2], int player);

void checkForWinner(int (*board)[3][3], char (*res)[2]);