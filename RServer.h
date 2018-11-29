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
#define MAX_PLAYERS 32

typedef enum Attack {							//enum to determine player choice in game
	ROCK,
	PAPER,
	SCISSORS,
	NONE
} Attack;					

typedef struct Player {
	Attack choice;									//last choice of the playere
	int socket_descriptor;						//socket descriptor used by this player
	int is_alive;										//equal 1 if won last game, 0 otherwise
	int place;											//place that player achived
	char name[NAME_LEN];					//name of the player
} Player;

typedef struct Game{
	Player **players;								//pointer to array of pointers of players in this game
	int number_of_players;						//number of players in this game
	int num_scissors;								//how many players chose scissors
	int num_rock;									//how many players chose rock
	int num_paper;									//how many players chooe paper
} Game;

typedef struct ThreadArgs {					//structure used to pass argument to created thread
  Player* player; 									//pointer to player thread has to handle
} ThreadArgs;



/*
 * Function:  intro_setup 
 * ------------------------------
 * prints welcoming text, asks user to type number of players,
 * assings it Game structure pointed by *g
 */
 void intro_setup(Game *g);
/*
 * Function:  server_setup 
 * ------------------------------
 * prepares socket for work
 * returns: descriptor of working server socket
 */
int server_setup();
/*
 * Function:  wait_for_all_to_connect 
 * ------------------------------
 * prepares socket for wort 
 * returns: descriptor of working server socket
 */
void wait_for_all_to_connect(int num_p, Player *p, int soc_des);
/*
 * Function:  game_setup 
 * ------------------------------
 * set all values of Players and Game
 * num_p: number of players in this game
 * **p: pointer to array of pointers of players in this game
 * *g: game of values to be set
 */
void game_setup(int num_p, Player **p, Game *g);
/*
 * Function:  recursive_play 
 * ------------------------------
 *	Whole game happens there 
 * *currentGame - 
 * max_place: maximum plas  players can get in this game
 * 
 */
void recursive_play(Game *currentGame, int max_place);
/*
 * Function:  wait_for_choose 
 * ------------------------------
 * gets choice of each player using threads
 * number_of_players: nuber of players 
 * 
 */
void wait_for_choose(int number_of_players, Player **p);
/*
 * Function:  ThreadMain 
 * ------------------------------
 * gets input from player with descriptor that is stored in threadArgs
 * *threadArgs: structure containing  player to handle
 */
void *ThreadMain(void *threadArgs);
/*
 * Function:  calculate_who_won 
 * ------------------------------
 * determines who won and who lost in this game
 * *g: all players of this game will be affected
 */
void calculate_who_won(Game *g);
/*
 * Function:  kill 
 * ------------------------------
 * sets is_alive to 0 of all players in Game *g if thay chose Attack att
 * att: if eq choice then set is_alive to 0
 * *g: all players of this game will be affected
 */
void kill(Attack att, Game *g);
/*
 * Function:  print_info 
 * ------------------------------
 * prints information -number of each choses, info about state of each player
 * *g: info of this game will be printed
 * returns: number of players that won in this game
 */
int print_info(Game *g);
/*
 * Function:  send_status 
 * ------------------------------
 * sends to clients info about their state
 * *g: all players of this game will be affected
 * num_alive - how many players won this game
 * num_dead - how many players lost this game
 * win_place - what is the highest place, players can get in this game
 */
void send_status(Game *g, int num_alive, int num_dead, int win_place);
/*
 * Function:  send_final 
 * ------------------------------
 * sends to clients info about their state at the end of the game; tells them to shut down
 * *g: all players of this game will be affected
 */
void send_final(Game *g);	
#endif