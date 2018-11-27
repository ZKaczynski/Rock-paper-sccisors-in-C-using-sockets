#ifndef _RSERVER
#define _RSERVER

#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <stdio.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define NAME_LEN 32 						//max number of charackters in player name
#define DEFAULT_PORT 4000 			//default port used in connection
#define clear() printf("\033[H\033[J")	//similar effect to system "clear"

typedef enum Attack {			//enum to determine player choice in game
	ROCK,
	PAPER,
	SCISSORS,
	NONE
} Attack;					

typedef struct Player {
	Attack choice;								//last choice of the playere
	int socket_descriptor;					//socket descriptor used by this player
	int is_alive;										//equal 1 if won last game, 0 otherwise
	int place;
	char name[NAME_LEN];				//name of the player
} Player;

typedef struct Game{
	Player **players;
	int number_of_players;
	int num_scissors;
	int num_rock;
	int num_paper;
} Game;

typedef struct ThreadArgs {
  Player* player; 
} ThreadArgs;

void   intro_setup(Game *g);
int server_setup();
void wait_for_all_to_connect(int num_p, Player *p, int soc_des);
void game_setup(int num_p, Player **p, Game *g);
void recursive_play(Game *currentGame, int max_place);
void wait_for_choose(int number_of_players, Player **p);
void *ThreadMain(void *threadArgs);
void calculateWhoWon(Game *g);
void kill(Attack att, Game *g);
int printInfo(Game *g);

#endif