#ifndef _RCLIENT
#define _RCLIENT

#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 4000 
#define clear() printf("\033[H\033[J")
#define NAME_LEN 32
/*
 * Function:  wait_for_choose 
 * ------------------------------
 * prepares socket for work
 * returns working socket descriptor
 */
int connectToServer(void);

#endif