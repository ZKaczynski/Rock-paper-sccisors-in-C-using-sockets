/*
	Zbigniew Kaczynski
	

*/
#include "RServer.h"

int main(void) {
	Game mainGame;						//struct of main game 
	int server_socket_descriptor=0;	//int descriptor of server socket	
	intro_setup(&mainGame); 
	int global_number_of_players=mainGame.number_of_players;
	Player global_players[global_number_of_players];
	Player* global_players_pointers[global_number_of_players];
	for (int i=0; i<global_number_of_players;i++){		//assign all players structs to array of pointers of Players							
		global_players_pointers[i]=&global_players[i];
	}
	server_socket_descriptor = server_setup();		//setup of server socket
	wait_for_all_to_connect( global_number_of_players,global_players,  server_socket_descriptor); //wait and get name from everybody	
	game_setup(global_number_of_players, global_players_pointers, &mainGame);	
	recursive_play(&mainGame, 1);		//whole game happends basicly here
	send_final(&mainGame);					//tell clients that game has ended
	return 0;
}


void intro_setup(Game *g){
	clear();
	puts("Welcome to \"Rock, Paper, Scissors\" Server!\n");
	puts("Type number of players:\n");
	scanf("%d",&(g->number_of_players));			//get number of players in game
	if (g->number_of_players>MAX_PLAYERS){	//check if inputed value is valid
		printf("%d is maximum number of players\n", MAX_PLAYERS);
		exit(0);
	}
	if (g->number_of_players<2){
		printf("Number of players has to be bigger then 2\n");
		exit(0);
	}
}

int server_setup(){
	int server_socket_descriptor=0;
	struct sockaddr_in address; 					//prepere socket address structure
    memset(&address, 0, sizeof(address));	//set memory of address to 0
    if ((server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == 0){ 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }    
    address.sin_family = AF_INET; 				//set arguments of socket address
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( DEFAULT_PORT );    
	if (setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");   
    if (bind(server_socket_descriptor, (struct sockaddr *)&address, sizeof(address))<0){ 	//bind socket to address
        perror("bind failed"); 					
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_socket_descriptor, 3) < 0){ 	//start listing on this socket
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
	return server_socket_descriptor;			//return descriptor of server socket
}

void wait_for_all_to_connect(int num_p, Player *p, int soc_des){
	int number_of_players = num_p;
	Player *players=p;
	int server_socket_descriptor = soc_des;
	printf("Waiting for %d clients...\n", number_of_players);
	for (int i=0; i<number_of_players; i++){
		struct sockaddr_in client_address; 
		int size =sizeof(client_address);
		if ((players[i].socket_descriptor= accept(server_socket_descriptor, (struct sockaddr *)&client_address, (socklen_t*)&size))<0) { 
			perror("Error accepting connection"); 
			exit(EXIT_FAILURE); 
		}
		read( players[i].socket_descriptor , players[i].name , sizeof(char)*NAME_LEN);
		printf("%d. Player %s has connected\n",i+1, players[i].name); 	
		fflush(stdout);
	}
	puts("All players are connected");
}

void game_setup(int num_p, Player **p, Game *g){	
	Player **players=p;
	Game *currentGame =g; 
	for (int i=0; i<num_p;i++){
		players[i]->is_alive=1;
		players[i]->place=0;
		players[i]->choice=NONE;
	}
	currentGame->number_of_players=num_p;
	currentGame->num_scissors=0;
	currentGame->num_rock=0;
	currentGame->num_paper=0;
	currentGame->players=p;
}

void recursive_play(Game *currentGame, int max_place){
	if (currentGame->number_of_players==0){
		return;
	}
	
	if (currentGame->number_of_players==1){
		currentGame->players[0]->place=max_place;
		return;
	}
	else {
		wait_for_choose(currentGame->number_of_players, currentGame->players);
		calculate_who_won(currentGame);
		
		
		
		int number_of_alive=print_info(currentGame);
		int number_of_dead= currentGame->number_of_players-number_of_alive;
		send_status(currentGame, number_of_alive, number_of_dead, max_place);
		Player **winners= malloc(number_of_alive*sizeof(Player*)); 
		Player **losers=malloc(number_of_dead*sizeof(Player*));
		Player **players=currentGame->players;
		int k=0,m=0;
		for (int i=0; i<currentGame->number_of_players; i++){
			if (players[i]->is_alive==1){
				winners[k]=players[i];
				k++;
			}
			else {
				losers[m]=players[i];	
				m++;	
			}
		}
		
		Game *loserGame=malloc(sizeof(Game));
		game_setup(number_of_dead, losers, loserGame);
		
		recursive_play(loserGame, max_place+number_of_alive);
		free(loserGame);
		free(losers);
		
		Game *winnerGame=malloc(sizeof(Game));
		game_setup(number_of_alive, winners, winnerGame);
		recursive_play(winnerGame, max_place);
		free(winnerGame);
		free(winners);
		
	}
	
}

void wait_for_choose(int number_of_players, Player **p){
	Player **players=p;
	pthread_t threads[number_of_players];
	ThreadArgs threadArgs[number_of_players]; 		
	for (int i=0; i<number_of_players;i++){		
		threadArgs[i].player = players[i];
		if (pthread_create(&threads[i], NULL, ThreadMain, &threadArgs[i]) != 0) {
			perror("Thread failed");
			exit(EXIT_FAILURE); 	
		}
	}
	for (int i=0; i<number_of_players;i++){
		pthread_join(threads[i], NULL);
	}	
}

void *ThreadMain(void *threadArgs) {
	Player *player= (( ThreadArgs *) threadArgs)->player;
	int info=1;
	send( player->socket_descriptor,&info, sizeof(int), 0);
	int message;
	read( player->socket_descriptor,&message, sizeof(int));
	switch(message){
		case 1:
			player->choice=ROCK;
		break;
		case 2:
			player->choice=PAPER;
		break;
		case 3:
			player->choice=SCISSORS;		
		break;
	}
	printf("Player %s has already chosen\n", player->name);
	fflush(stdout);
	return (NULL);
}

void calculate_who_won(Game *g){
	Game *currentGame =g; 
	int number_of_players=currentGame->number_of_players;
	Player **players =currentGame->players;
	puts("GAME BEGINS");
	for (int i=0; i<number_of_players; i++){
		switch(players[i]->choice){
		case(ROCK):
			currentGame->num_rock++;
			break;
		case(PAPER):
			currentGame->num_paper++;
			break;
		case(SCISSORS):
			currentGame->num_scissors++;
			break;
		}
	}
	if ((currentGame->num_rock==0 && currentGame->num_scissors==0)||
		(currentGame->num_paper==0 && currentGame->num_scissors==0)||
		(currentGame->num_paper==0 && currentGame->num_rock==0)) {}
	else if (currentGame->num_rock==0)  kill(PAPER, currentGame);
	else if (currentGame->num_paper==0)  kill(SCISSORS, currentGame);
	else if (currentGame->num_scissors==0)  kill(ROCK, currentGame);
	else{
		if (currentGame->num_rock>currentGame->num_scissors){
			kill(SCISSORS, currentGame);
		}
		if (currentGame->num_scissors>currentGame->num_paper){
			kill(PAPER, currentGame);
		}
		if (currentGame->num_paper>currentGame->num_rock){
			kill(ROCK, currentGame);
		}
	}
}

void kill(Attack att, Game *g){
	int number_of_players=g->number_of_players;
	Player **players =g->players;
	for (int i=0; i<number_of_players; i++){
		if (players[i]->choice==att) players[i]->is_alive=0;
	}	
}

int print_info(Game *g){
	int number_of_alive =0;
	int number_of_players=g->number_of_players;
	Player **players =g->players;
	Game *currentGame =g; 
	printf ("Rock %d, Paper %d, Scissors %d\n", currentGame->num_rock, currentGame->num_paper, currentGame->num_scissors);
	for (int i=0; i<number_of_players; i++){
		if (players[i]->is_alive==0){
			printf("%d. Player %s  has died\n", i+1, players[i]->name );
		}
		else{
			number_of_alive++;
			printf("%d. Player %s  is still alive\n", i+1, players[i]->name );
		}
	}	
	return number_of_alive;
}

void send_status(Game *g, int num_alive, int num_dead, int win_place){
	for (int i=0; i<g->number_of_players; i++){
		int message=2;
		send( g->players[i]->socket_descriptor,&message, sizeof(int), 0);
		int status[3]={0}; 
		status[0]=g->players[i]->is_alive;
		if (g->players[i]->is_alive==1){
			status[1]=win_place;
			status[2]=num_alive;
		}
		else {
			status[1]=win_place+num_alive;
			status[2]=num_dead;
		}
		send( g->players[i]->socket_descriptor, status, 3*sizeof(int), 0);
	}	
}

void send_final(Game *g){
	int message0=0;
	int message3=3;
	for (int i=0; i<g->number_of_players; i++){
		printf("Player %s is on %d place\n", g->players[i]->name, g->players[i]->place);
		send( g->players[i]->socket_descriptor,&message3, sizeof(int), 0);
		int status[2]={0}; 
		status[0]=g->players[i]->place;
		status[1]=g->number_of_players;
		send( g->players[i]->socket_descriptor, status, 2*sizeof(int), 0);
		send( g->players[i]->socket_descriptor,&message0, sizeof(int), 0);
		
	}
	
	
	
	
}

