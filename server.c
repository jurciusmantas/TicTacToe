#include <winsock2.h>
#define socklen_t int
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server_header.h"

int main(int argc, char *argv[])
{
	WSADATA data;
	unsigned int port;
	int l_socket;
	int c1_socket;
	int c2_socket;
	
	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr1;
	struct sockaddr_in clientaddr2;
	
	socklen_t clientaddr1len;
	socklen_t clientaddr2len;
	
	int s_len;
	int r_len;
	
	int board[3][3];
	memset(&board, 0, sizeof(board));
	
	if (argc != 2)
	{
		printf("USAGE: %s <port>\n", argv[0]);
		exit(1);
	}
	
	port = atoi(argv[1]);
	
	if ((port < 1) || (port > 65535))
	{
		printf("ERROR: INVALID PORT \n");
		exit(1);
	}
	
	WSAStartup(MAKEWORD(2,2),&data);
	
	
	//STEP 1
	//Server Socket CREATION with
	//socket() call
	l_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (l_socket < 0)
	{
		printf("ERROR: CANNOT CREATE LISTEN SOCKET \n");
		exit(1);
	}
	
	//STEP 2
	//Server Socket BINDING with
	//bind() call
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	
	if (bind(l_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) <0)
	{
		printf("ERROR: BINDING LISTEN SOCKET \n");
		exit(1);
	}
	
	//STEP 3
	//Server Socket LISTENing with
	//listen() call
	if (listen(l_socket, 2) < 0)
	{
		printf("ERROR: TOO MANY CLIENTS \n");
		exit(1);
	}
	
	for(;;)
	{
		memset(&clientaddr1, 0, sizeof(clientaddr1));
		clientaddr1len = sizeof(clientaddr1);		
		memset(&clientaddr2, 0, sizeof(clientaddr2));
		clientaddr2len = sizeof(clientaddr2);

		//STEP 5
		//Server Socket LISTENing with
		//listen() call
		char hiConn1[] = "SERVER: CONNECTED PLAYER 1";
		char hiConn2[] = "SERVER: CONNECTED PLAYER 2";
		
		//HANDSHAKE
		printf("SENDING HANDSHAKE\n");
		
		if ((c1_socket = accept(l_socket, (struct sockaddr* )&clientaddr1, &clientaddr1len)) <0 )
		{
			printf("ERROR: ACCEPTING SOCKET 1 \n");
			exit(1);
		}
		send(c1_socket, hiConn1, strlen(hiConn1), 0);
		
		if ((c2_socket = accept(l_socket, (struct sockaddr* )&clientaddr2, &clientaddr2len)) <0 )
		{
			printf("ERROR: ACCEPTING SOCKET 2 \n");
			exit(1);
		}
		send(c2_socket, hiConn2, strlen(hiConn2), 0);
		
		printf("CLIENTS CONNECTED :\n");
		printf("IP1   : %s \n", inet_ntoa(clientaddr1.sin_addr));
		printf("IP2   : %s \n", inet_ntoa(clientaddr2.sin_addr));
		
		//STEP 6
		//Client Socket RECEIVE with
		//recv() call
		int connected = 1;
		int player = 1;
		
		while(connected)
		{
			char buffer[2];
			memset(&buffer, 0, sizeof(buffer));
			//LISTEN MODE
			if(player == 1)
				s_len = recv(c1_socket, buffer, 2, 0);
			else if (player == 2)
				s_len = recv(c2_socket, buffer, 2, 0);

			printf("MESSAGE PROCESSING : %c%c FROM PLAYER %d\n", buffer[0],buffer[1], player);
			//COMMANDS:
			//DC - Disconnect
			if (strstr(buffer,"DC") != NULL)
			{
				if(player==1)
				{
					printf("CLIENT 1 DISCONNECTED");
					send(c2_socket, "OD", 2, 0);
				}
				else
				{
					printf("CLIENT 2 DISCONNECTED");
					send(c1_socket, "OD", 2, 0);
				}
				
				connected = 0;
			}
			
			//VALIDATE MESSAGE FORMAT
			//MESSAGE LENGTH = 2
			//MOVE FORMAT - [A-C][1-3]
			else if (s_len != 2 || buffer[0] < 'A' || buffer[0] > 'C' 
				|| buffer[1] < '1' || buffer[1] > '3')
			{
				printf("ERROR: BAD MOVE FORMAT FROM CLIENT \n");
				exit(1);
			}
			
			//PROCCESS MOVE
			else
			{	
				processMove(&board, &buffer, player);
				printf("BUFFER = %c %c", buffer[0], buffer[1]);
				
				if (player == 1)
				{
					send(c1_socket, buffer, 2, 0);
					player = 2;
				}
				else
				{
					send(c2_socket, buffer, 2, 0);
					player = 1;
				}
			}
		}
		close(c1_socket);
		close(c2_socket);
		return 0;
	}
}