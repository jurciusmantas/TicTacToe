# TicTacToe
TicTacToe game with client-server-client TCP sockets

COMPILE INFO
SERVER:
gcc server.c -o server -lws2_32 -std=gnu99 gameplay_functions.c

CLIENT:
gcc client.c -o client -lws2_32 gameplay_functions.c client_functions.c
