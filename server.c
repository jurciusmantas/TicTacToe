#include <winsock2.h>
#include <windows.h>
#define socklen_t int
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gameplay_header.h"

typedef struct _SOCKET_PAIR_INFORMATION {
   
   char Buffer1[5];
   SOCKET Socket1;
   char Buffer2[5];
   
   SOCKET Socket2;
   BOOL IsPairReady;
   int Board[3][3];
   
} SOCKET_PAIR_INFORMATION, * LPSOCKET_PAIR_INFORMATION;

BOOL AssignPairForSocket(SOCKET s);
BOOL CreateSocketInformation(SOCKET s);
void FreeSocketInformation(DWORD Index);

//Global
DWORD TotalSocketPairs = 0;
LPSOCKET_PAIR_INFORMATION SocketArray[FD_SETSIZE];

int main(int argc, char *argv[])
{
	WSADATA data;
	unsigned int port;
	int l_socket;
	
	int c_socket;
	
	int i,j;
	int player = 1;
	unsigned long NonBlock;
	FD_SET ReadSet;
	FD_SET WriteSet;
	
	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr1;
	struct sockaddr_in clientaddr2;
	
	socklen_t clientaddr1len;
	socklen_t clientaddr2len;
	
	int s_len;
	int r_len;
	
	char hiConn1[] = "SERVER: CONNECTED PLAYER 1";
	char hiConn2[] = "SERVER: CONNECTED PLAYER 2";
	char temp[2];
	
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
	else
		printf("CREATING SOCKET - OK\n");
	
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
	else
		printf("LISTEN SOCKET CREATING - OK\n");
	
	//STEP 3
	//Server Socket LISTENing with
	//listen() call
	if (listen(l_socket, 6) < 0)
	{
		printf("ERROR: LISTEN SOCKET \n");
		exit(1);
	}
	else
		printf("LISTEN SOCKET - OK\n");
	
	NonBlock = 1;
	if (ioctlsocket(l_socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
	{
		printf("ERROR: MAKING LISTEN SOCKET NONBLOCKING FAILED\n");
		exit(1);
	}
	else
		printf("MAKING LISTEN SOCKET NONBLOCKING - OK\n");
	
	while(TRUE)
	{
		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);
		
		for(i = 0; i < TotalSocketPairs; i++)
		{
			if (SocketArray[i]->Buffer1[0] == '\n')
				FD_SET(SocketArray[i]->Socket1, &ReadSet);
			else
				FD_SET(SocketArray[i]->Socket1, &WriteSet);
			
			if(SocketArray[i]->Socket2 != 0)
			{
				if (SocketArray[i]->Buffer2[0] == '\n')
					FD_SET(SocketArray[i]->Socket2, &ReadSet);
				else
					FD_SET(SocketArray[i]->Socket2, &WriteSet);
			}
		}
		
		FD_SET(l_socket, &ReadSet);
		
		if (select(0, &ReadSet, &WriteSet, NULL, NULL) == SOCKET_ERROR)
		{
			printf("ERROR: SELECT RETURNED SOCKET_ERROR");
			exit(1);
		}
		else
			printf("SELECT - OK\n");

		//Checking for arriving connections
		if (FD_ISSET(l_socket, &ReadSet))
		{	
			//STEP 5
			//Server Socket LISTENing with
			//listen() call
		
			if ((c_socket = accept(l_socket, NULL, NULL)) != INVALID_SOCKET )
			{
				// Make client socket non blocking
				NonBlock = 1;
				if (ioctlsocket(c_socket, FIONBIO, &NonBlock) == SOCKET_ERROR)
				{
					printf("ERROR: MAKING CLIENT SOCKET NONBLOCKING\n");
					exit(1);
				}
				else
					printf("CLIENT SOCKET NONBLOCKING - OK\n");
				
				if (player == 1)
				{
					if(CreateSocketInformation(c_socket) == FALSE)
					{
						printf("ERROR: CreateSocketInformation()\n");
						exit(1);
					}
					else
						printf("CreateSocketInformation() - OK\n");
					
					send(c_socket, hiConn1, strlen(hiConn1), 0);
					player++;
					SocketArray[TotalSocketPairs]->Buffer1[0] = '\n';
				}
				else
				{
					if (AssignPairForSocket(c_socket) == FALSE)
					{
						printf("ERROR: ASSIGNING PAIR\n");
						exit(1);
					}
					else
						printf("ASSIGNING PAIR - OK\n");
					
					send(c_socket, hiConn2, strlen(hiConn1), 0);
					player--;
					strcpy(SocketArray[TotalSocketPairs-1]->Buffer2, SocketArray[TotalSocketPairs-1]->Buffer1);
				}
			}
			else
			{
				printf("ERROR: ACCEPTING SOCKET\n");
				exit(1);
			}
		}
		
		//****************************\\
		//***********PLAYER 1*********\\
		//****************************\\
		//Check for ReadSet and WriteSet 
		//For every player 1 socket in pairs
		printf("Checking player 1 sockets\n");
		for (i = 0; i < TotalSocketPairs; i++)
		{	
			if (FD_ISSET(SocketArray[i]->Socket1, &ReadSet))
			{	
				r_len = recv(SocketArray[i]->Socket1, SocketArray[i]->Buffer1, 2, 0);
				if(r_len == SOCKET_ERROR)
				{
					printf("ERROR: recv() WITH PLAYER 1 PAIR NUMBER %d\n", i);
				}
				else if (r_len == 0)
					printf("ASSERT: Gracefull close? Player 1 pair %d\n", i);
				else
					printf("recv() - OK. PLAYER 1 FROM PAIR %d SENT %s\n", i, SocketArray[i]->Buffer1);
				
				if (ValidateMove(SocketArray[i]->Buffer1) != 1)
				{
					printf("ERROR: SERVER SENT BAD MOVE");
					exit(1);
				}
				
				ProcessMove(&(SocketArray[i]->Board), Socket
				strcpy(SocketArray[i]->Buffer1, "OK");
				continue;
			}
			
			if (FD_ISSET(SocketArray[i]->Socket1, &WriteSet))
			{
				//temp - when client
				//will be rewritten then
				//send full buffer - len 5
				s_len = send(SocketArray[i]->Socket1, SocketArray[i]->Buffer1, 2, 0);
				
				if(s_len == SOCKET_ERROR)
				{
					printf("ERROR: send() WITH PLAYER 1 PAIR NUMBER %d\n", i);
				}
				else
				{
					printf("send() WITH PLAYER 1 PAIR NUMBER %d - OK\n", i);
				}

				SocketArray[i]->Buffer1[0] = '\n';
				
				//SERVER PLACE TO
				//PROCESS SERVER MSG
			}
		}
		printf("Player 1 sockets - OK\n");
		printf("Checking player 2 sockets\n");
		
		//****************************\\
		//***********PLAYER 2*********\\
		//****************************\\
		//Check for ReadSet and WriteSet 
		//For every player 2 socket in pairs
		for (i = 0; i < TotalSocketPairs; i++)
		{
			if (FD_ISSET(SocketArray[i]->Socket2, &ReadSet))
			{	
				r_len = recv(SocketArray[i]->Socket2, SocketArray[i]->Buffer2, 2, 0);
				if(r_len == SOCKET_ERROR)
				{
					printf("ERROR: recv() WITH PLAYER 2 PAIR NUMBER %d\n", i);
				}
				else if (r_len == 0)
					printf("ASSERT: Gracefull close? Player 2 pair %d\n", i);
				else
					printf("recv() - OK. PLAYER 2 FROM PAIR %d SENT %s\n", i, SocketArray[i]->Buffer2);
				

				//VALIDATE CLIENT MOVE??
				strcpy(SocketArray[i]->Buffer2, "OK");
				
				if (ValidateMove(SocketArray[i]->Buffer2)
				
				continue;
			}
			
			if (FD_ISSET(SocketArray[i]->Socket2, &WriteSet))
			{
				//temp - when client
				//will be rewritten then
				//send full buffer - len 5
				s_len = send(SocketArray[i]->Socket2, SocketArray[i]->Buffer2, 2, 0);
				
				if(s_len == SOCKET_ERROR)
				{
					printf("ERROR: send() WITH PLAYER 2 PAIR NUMBER %d\n", i);
				}
				else
				{
					//FD_SET(SocketArray[i]->Socket2, &ReadSet);
					printf("send() WITH PLAYER 2 PAIR NUMBER %d - OK\n", i);
				}
				
				SocketArray[i]->Buffer2[0] = '\n';
				
				//SERVER PLACE TO
				//PROCESS SERVER MSG
			}
		}
		printf("Player 2 sockets - OK\n\n--\n");
		
		
		//****************************\\
		//***********OLD LOGIC********\\
		//****************************\\
		//STEP 6
		//Client Socket RECEIVE with
		//recv() call
		/*
		int connected = 1;
		int player = 1;
		char processMoveRes[2];
		char buffer[2];
		
		while(connected)
		{
			memset(&buffer, 0, sizeof(buffer));
			//LISTEN MODE
			if(player == 1)
				r_len = recv(c_socket, buffer, 2, 0);
			else if (player == 2)
				r_len = recv(c2_socket, buffer, 2, 0);

			printf("MESSAGE PROCESSING : %c%c FROM PLAYER %d\n", buffer[0],buffer[1], player);
			//COMMANDS:
			//DC - Disconnect
			if (r_len == 2 && strstr(buffer,"DC") != NULL)
			{
				if(player==1)
				{
					printf("CLIENT 1 DISCONNECTED");
					send(c2_socket, "OD", 2, 0);
				}
				else
				{
					printf("CLIENT 2 DISCONNECTED");
					send(c_socket, "OD", 2, 0);
				}
				
				connected = 0;
			}
			
			//VALIDATE MESSAGE FORMAT
			//MESSAGE LENGTH = 2
			//MOVE FORMAT - [A-C][1-3]
			else if (r_len != 2 || buffer[0] < 'A' || buffer[0] > 'C' 
				|| buffer[1] < '1' || buffer[1] > '3')
			{
				printf("ERROR: BAD MOVE FORMAT FROM PLAYER %d \n", player);
				exit(1);
			}
			
			//PROCESS MOVE
			else
			{	
				validateAndProcess(&board, &buffer, &processMoveRes, player);
				checkForWinner(&board, &processMoveRes);
				
				if(processMoveRes[0] == 'O' && processMoveRes[1] == 'K')
				{
					//SEND PLAYER MOVE - TO OPPONENT
					//SEND OK - 	   - TO PLAYER (if no win)
					//SEND W{player}   - TO BOTH IF WIN
					if (player == 1)
					{
						send(c_socket, processMoveRes, 2, 0);
						send(c2_socket, buffer, 2, 0);
						player = 2;
					}
					else
					{
						send(c2_socket, processMoveRes, 2, 0);
						send(c_socket, buffer, 2, 0);
						player = 1;
					}
				}
				else if (processMoveRes[0] == 'W')
				{
					printf("WINNER IS PLAYER ");
					// Send looser opponent move
					if (player == 1)
						send(c2_socket, processMoveRes, 2, 0);
					else
						send(c_socket, processMoveRes, 2, 0);
					
					// Send both message that
					// winner is noticed
					if (player == 1)
					{
						printf("1!\n"); 
						processMoveRes[1] = player + '0';
						send(c2_socket, buffer, 2, 0);
						send(c_socket, processMoveRes, 2, 0);
					}
					else
					{
						printf("2!\n");
						processMoveRes[1] = player + '0';
						send(c_socket, buffer, 2, 0);
						send(c2_socket, processMoveRes, 2, 0);
					}
					printf("WAITING ACTIONS FROM BOTH PLAYERS...\n");
					
					r_len = recv(c_socket, buffer, 2, 0);
					s_len = recv(c2_socket, processMoveRes, 2, 0);
					
					if(s_len != 2 || r_len != 2)
					{
						printf("ERROR: CLIENT DISCONNECTED\n");
						exit(1);
					}
					
					else if (buffer[0] == '1' && buffer[1] == '1' 
						&& processMoveRes[0] == '2' && processMoveRes[1] == '1')
					{
						memset(&board, 0, sizeof(board));
						memset(&processMoveRes, 0, sizeof(processMoveRes));
						
						strcpy(buffer, "NG");
						send(c_socket, buffer, 2, 0);
						send(c2_socket, buffer, 2, 0);
						
						player = 1;
					}
					else if (buffer[0] == '1' && buffer[1] == '2')
					{
						printf("CLIENT 1 DISCONNECTED.\n");
						connected = 0;
					}
					else if (processMoveRes[0] == '1' && processMoveRes[1] == '2')
					{
						printf("CLIENT 2 DISCONNECTED.\n");
						connected = 0;
					}
					else
					{
						printf("ERROR\n");
						connected = 0;
					}	
				}
				else
				{
					//process NO
				}
			}
		}
		return 0;
		*/
	}
}


BOOL CreateSocketInformation(SOCKET s)
{
   LPSOCKET_PAIR_INFORMATION SI;
   printf("Accepted socket number %d\n", s);
   
   if ((SI = (LPSOCKET_PAIR_INFORMATION) GlobalAlloc(GPTR, sizeof(SOCKET_PAIR_INFORMATION))) == NULL)
   {
      printf("ERROR: CREATING GLOBAL SPACE FOR SOCKET %d INFO\n", s);
      return FALSE;
   }
   else
      printf("CREATING GLOBAL SPACE FOR SOCKET %d INFO - OK\n", s);

   // Prepare structure for use
   SI->Socket1 = s;
   
   SI->Socket2 = 0;
   
   SI->IsPairReady = FALSE;
   memset(SI->Board, 0, sizeof(SI->Board));
   
   SocketArray[TotalSocketPairs] = SI;
   return(TRUE);
}

BOOL AssignPairForSocket(SOCKET s)
{
	printf("Accepted socket number %d\n", s);
	
	//send first player notification that pair is found???
	
	SocketArray[TotalSocketPairs]->Socket2 = s;
	TotalSocketPairs++;
}

void FreeSocketInformation(DWORD Index)
{
   LPSOCKET_PAIR_INFORMATION SI = SocketArray[Index];
   DWORD i;

   closesocket(SI->Socket1);
   closesocket(SI->Socket2);
   printf("Closing sockets number %d and %d\n", SI->Socket1, SI->Socket2);
   GlobalFree(SI);

   // Squash the socket array
   for (i = Index; i < TotalSocketPairs; i++)
   {
      SocketArray[i] = SocketArray[i + 1];
   }

   TotalSocketPairs--;
}