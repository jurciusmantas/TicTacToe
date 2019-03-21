#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "client_header.h"
#include "gameplay_header.h"
#define HANDSHAKE "SERVER: CONNECTED PLAYER  "

typedef struct _SOCKET_INFORMATION {
   
   char Buffer[5];
   SOCKET Socket;
   int Board[3][3];
   
} SOCKET_INFORMATION, * SOCKETINFORMATION;

int main(int argc, char *argv[])
{
	WSADATA data;
	SOCKETINFORMATION SocketInfo;
	unsigned int port;
	int s_socket;
	struct sockaddr_in servaddr;
	FD_SET WriteSet;
	FD_SET ReadSet;
	
	SocketInfo = malloc(sizeof(SOCKETINFORMATION));
	
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

	if ((SocketInfo->Socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
	if (connect(SocketInfo->Socket, (struct sockaddr*)&servaddr, sizeof(servaddr))<0)
	{
		printf("ERROR: CONNECTING TO SERVER");
		exit(1);
	}
	
	char handshake[strlen(HANDSHAKE)];
	memset(&handshake, 0, sizeof(HANDSHAKE));
	
	//Wait for handshake
	recv(SocketInfo->Socket, handshake, strlen(HANDSHAKE), 0);
	printf("%s\n", handshake);
	
	int player = handshake[strlen(HANDSHAKE) - 1] - '0';
	int opponent = player == 1 ? 1 : 2;
	int expectingOk = 0;
	
	SocketInfo->Buffer[0]='\n';
	char validateResponse[2];
	int r_len, s_len;
	printf("PLAYER %d\n", player);
	memset(SocketInfo->Board, 0, sizeof(SocketInfo->Board));
	printBoard(&(SocketInfo->Board));
	if(player == 2)
	{
		printf("WAITING FOR OPPONENT MOVE...\n");
		r_len = recv(SocketInfo->Socket, SocketInfo->Buffer, 5, 0);
		if (r_len != 5 || ValidateMove(SocketInfo->Buffer) != 1)
		{
			printf("ERROR: SERVER SENT BAD FORMAT MOVE");
			exit(1);
		}
		
		if (ProcessMove(&(SocketInfo->Board), SocketInfo->Buffer, player) != 1)
		{
			printf("ERROR: CANNOT PROCESS MOVE");
			exit(1);
		}
		
		printBoard(&(SocketInfo->Board));
	}
	
	memset(&(SocketInfo->Buffer), 0, sizeof(SocketInfo->Buffer));
	strcpy(SocketInfo->Buffer, "\n");
	printf("[DEBUG] SocketInfo->Buffer = %s\n", SocketInfo->Buffer);
	
	int connected = 1;
	
	while(connected)
	{
		FD_ZERO(&WriteSet);
		FD_ZERO(&ReadSet);
		
		if (SocketInfo->Buffer[0] == '\n')
			FD_SET(SocketInfo->Socket, &WriteSet);
		else
			FD_SET(SocketInfo->Socket, &ReadSet);
		
		if (FD_ISSET(SocketInfo->Socket, &WriteSet))
		{	
			while (1)
			{
				printf("Enter your move: ");
				gets(SocketInfo->Buffer);
				if (SocketInfo->Buffer[3] != 0 && ValidateMove(SocketInfo->Buffer) != 1)
				{
					printf("BAD MOVE FORMAT!\n");
					continue;
				}
				
				if (ProcessMove(&(SocketInfo->Board), SocketInfo->Buffer, player) != 1)
				{
					printf("CELL IS ALREADY TAKEN!\n");
					continue;
				}
			
				break;
			}
			
			printBoard(&(SocketInfo->Board));
			send(SocketInfo->Socket, SocketInfo->Buffer, 5, 0);
		}
		
		if (FD_ISSET(SocketInfo->Socket, &ReadSet))
		{
			printf("Receiving from server...\n");
			r_len = recv(SocketInfo->Socket, SocketInfo->Buffer, 5, 0);

			if (r_len == SOCKET_ERROR)
			{
				printf("ERROR: SERVER SOCKET\n");
				exit(1);
			}
			
			if (r_len == 0)
			{
				printf("Graceful close?\n");
				exit(1);
			}
			

			if (r_len == 5)
			{
				//SERVER MSG:
				//opponent move + OK + \n
				if (ValidateServerMessage(SocketInfo->Buffer) == 1)
				{
					if (ProcessMove(&(SocketInfo->Board), SocketInfo->Buffer, opponent) != 1)
					{
						printf("ERROR: Processing opponent move\n");
						exit(1);
					}
					
					printBoard(&(SocketInfo->Board));
					memset(&(SocketInfo->Buffer), 0, sizeof(SocketInfo->Buffer));
					strcpy(SocketInfo->Buffer, "\n");
				}
			}
			else
			{
				printf("ERROR: SERVER BAD MSG");
				exit(1);
			}
		}
		
		/**************\
		/***OLD LOGIC**\
		/**************\
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
			if (buffer[0] == 'O' && buffer[1] == 'D')
			{
				printf("OPPONENT DISCONNECTED\n");
				exit(1);
			}
			
			else if (buffer[0] == 'W')
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
				
				buffer[0] = player + '0';
				buffer[1] = tempChar;
				printf("AFTER GAME WINNER RESPONSE - %c-%c\n", buffer[0], buffer[1]);
				send(s_socket, buffer, 2, 0);

				if (tempChar == '1')
				{
					memset(&board, 0, sizeof(board));
					memset(&validateResponse, 0, sizeof(validateResponse));
					printf("CREATING NEW GAME...\n");
					printBoard(&board);
				
					r_len = recv(s_socket, buffer, 2, 0);
					printf("[DEBUG] RECV - %c-%c, length - %d\n", buffer[0], buffer[1], r_len);
					if (r_len != 2 || buffer[0] != 'N' || buffer[1] != 'G')
					{
						printf("ERROR: CANNOT CREATE NEW GAME!\n");
						exit(1);
					}
					
					if (player == 2)
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
				}
				else if (tempChar == '2')
				{
					printf("DISCONNECTING...\n");
					connected = 0;
				}
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
					r_len = recv(s_socket, buffer, 2, 0);
					
					temp[0] = buffer[0];
					temp[1] = buffer[1];
					validateAndProcess(&board, &temp, &validateResponse, opponent);
					printBoard(&board);
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


					buffer[0] = player + '0';
					buffer[1] = tempChar;
					send(s_socket, buffer, 2, 0);
					printf("AFTER GAME WINNER RESPONSE - %c-%c\n", buffer[0], buffer[1]);
				
					if (tempChar == '1')
					{
						memset(&board, 0, sizeof(board));
						memset(&validateResponse, 0, sizeof(validateResponse));
						printf("CREATING NEW GAME...\n");
						printBoard(&board);
				
						r_len = recv(s_socket, buffer, 2, 0);
						printf("RECV - %c-%c, length - %d\n", buffer[0], buffer[1], r_len);
						if (r_len != 2 || buffer[0] != 'N' || buffer[1] != 'G')
						{
							printf("ERROR: CANNOT CREATE NEW GAME!\n");
							exit(1);
						}
						
						if (player == 2)
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
					}
					else if (tempChar == '2')
					{
						printf("DISCONNECTING...\n");
						connected = 0;
					}					
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
		*/
	}
	printf("DISCONNECTED");
	close(SocketInfo->Socket);
	return 0;
}