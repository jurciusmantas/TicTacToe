#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "client_header.h"
#include "gameplay_header.h"
#define HANDSHAKE "SERVER: CONNECTED PLAYER  "

int main(int argc, char *argv[])
{
	WSADATA data;
	
	unsigned int port;
	int s_socket;
	struct sockaddr_in servaddr;
	
	int board[3][3];
	memset(&board, 0, sizeof(board));
	
	if(argc != 3)
	{
		printf("USAGE: %s <ip> <port>\n", argv[0]);
		exit(1);
	}
	
	port = atoi(argv[2]);
	
	if ((port < 1) || (port > 65535))
	{
		printf("ERROR: INVALID PORT");
		exit(1);
	}
	
	WSAStartup(MAKEWORD(2,2), &data);
	
	//STEP 1
	//Server Socket CREATION with
	//socket() call
	if ((s_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("ERROR: CANNOT CREATE SERVER SOCKET");
		exit(1);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//STEP 4
	//Server Socket CONNECT with
	//connect() call
	if (connect(s_socket, (struct sockaddr*)&servaddr, sizeof(servaddr))<0)
	{
		printf("ERROR: CONNECTING TO SERVER");
		exit(1);
	}
	
	char handshake[strlen(HANDSHAKE)];
	memset(&handshake, 0, sizeof(HANDSHAKE));
	
	//Wait for handshake
	recv(s_socket, handshake, strlen(HANDSHAKE), 0);
	printf("%s\n", handshake);
	
	int player = handshake[strlen(HANDSHAKE) - 1] - '0';
	int opponent = player + 1 > 2 ? 1 : 2;
	char buffer[3];
	char temp[2];
	char validateResponse[2];
	char clear = '\n';
	int r_len, s_len;
	printf("PLAYER %d\n", player);
	if(player == 2)
	{
		printf("WAITING FOR OPPONENT MOVE...\n");
		r_len = recv(s_socket, buffer, 2, 0);
		if (r_len != 2)
		{
			printf("ERROR: SERVER SENT BAD FORMAT MOVE");
			exit(1);
		}
		temp[0] = buffer[0];
		temp[1] = buffer[1];
		validateAndProcess(&board, &temp, &validateResponse, 1);
		printBoard(&board);
	}
	
	int connected = 1;
	while(connected)
	{
		//CLEAR STDIN
		while(clear != '\n')
			clear = fgetc(stdin);
		
		
		memset(&buffer, 0, sizeof(buffer));
		printf("Enter move: \n");
		fread(buffer, sizeof(buffer), 1, stdin);
		
		//COMMAND DC - DISCONNECT
		if(strstr(buffer,"DC\n") != NULL)
		{
			printf("DISCONNECTING...\n");
			send(s_socket, "DC", 2, 0);
			connected = 0;
		}
		
		//VALIDATE MESSAGE FORMAT
		else if(buffer[0] < 'A' || buffer[0] > 'C' || buffer[1] < '1'
			|| buffer[1] > '3' || buffer[2] != '\n')
		{
			printf("ERROR: BAD MESSAGE FORMAT!\n");
			continue;
		}
		else
		{
			//TODO: 
			//0. VALIDATE MOVE
			//1. SEND SERVER MOVE
			//2. GET OK
			//3. GET OPPONENT MOVE
			
			//0			
			temp[0] = buffer[0];
			temp[1] = buffer[1];
			validateAndProcess(&board, &temp, &validateResponse, player);
			if (strcmp(validateResponse,"OK") != 0)
			{
				printf("ERROR:BAD MOVE\n");
				continue;
			}
			printBoard(&board);
			
			//1
			send(s_socket, buffer, 2, 0);
			
			//2
			memset(&buffer, 0, sizeof(buffer));			
			printf("WAITING FOR OK...\n");
			r_len = recv(s_socket, buffer, 2, 0);
			
			printf("SERVER SENT - %s\n", buffer);
			
			if (buffer[0] == 'W')
			{
				//winner
				int tempWhile = 1;
				char tempChar;
				printf("CONGRATULATIONS, YOU WON!\n");
				
				while(tempWhile)
				{
					printf("ENTER 1 TO PLAY AGAIN.\n");
					printf("ENTER 2 TO DISCONNECT.\n");
					tempChar = fgetc(stdin);
					if ( (tempChar == '1' || tempChar == '2') && fgetc(stdin) == '\n')
						tempWhile = 0;
					
					else
					{
						printf("BAD COMMAND!\n");
						tempChar = fgetc(stdin);
						while(tempChar != '\n')
							tempChar = fgetc(stdin);
					}
				}

				if (tempChar == '1')
				{
					//memset everything to 0
					//maybe swap sides??
				}
				else if (tempChar == '2')
				{
					printf("DISCONNECTING...\n");
					connected = 0;
				}
				buffer[0] = player + '0';
				buffer[1] = tempChar;
				send(s_socket, buffer, 2, 0);
			}
			
			else if (strcmp(buffer, "OD") == 0)
			{
				printf("OPPONENT DISCONNECTED. YOU WIN!\n");
				connected = 0;
			}
			else if (strcmp(buffer, "NO") == 0)
			{
				printf("ERROR: BAD MOVE\n");
			}
			//3
			else if (strcmp(buffer, "OK") == 0)
			{
				printf("WAITING FOR OPPONENT MOVE...\n");
				r_len = recv(s_socket, buffer, 2, 0);
				
				if (buffer[0] == 'W')
				{
					//looser
					int tempWhile = 1;
					char tempChar;
					printf("UNFORTUNATELY, YOU LOST...\n");
				
					while(tempWhile)
					{
						printf("ENTER 1 TO PLAY AGAIN.\n");
						printf("ENTER 2 TO DISCONNECT.\n");
						tempChar = fgetc(stdin);
						if ( (tempChar == '1' || tempChar == '2') && fgetc(stdin) == '\n')
							tempWhile = 0;
					
						else
						{
							printf("BAD COMMAND!\n");
							tempChar = fgetc(stdin);
							while(tempChar != '\n')
								tempChar = fgetc(stdin);
						}
					}


					if (tempChar == '1')
					{
						//memset everything to 0
						//maybe swap sides??
					}
					else if (tempChar == '2')
					{
						printf("DISCONNECTING...\n");
						connected = 0;
					}
					buffer[0] = player + '0';
					buffer[1] = tempChar;
					send(s_socket, buffer, 2, 0);					
				}
				else
				{
					temp[0] = buffer[0];
					temp[1] = buffer[1];
					validateAndProcess(&board, &temp, &validateResponse, opponent);
					printBoard(&board);
				}
			}
		}
		//clear = fgetc(stdin);
	}
	printf("DISCONNECTED");
	close(s_socket);
	return 0;
}