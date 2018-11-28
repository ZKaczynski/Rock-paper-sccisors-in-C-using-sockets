#include "RClient.h"

int main(void){
	clear();
	int sock = connectToServer();
	printf("Welcome to \"Rock, Paper, Scissors\" Client!\n");
	printf("Type your nickname:\n");
	char name[NAME_LEN]={0};
	scanf("%s", name);
	send(sock , name , sizeof(char)*NAME_LEN , 0 ); 
    printf("Waiting for other players to connect\n"); 
	
	int game_is_running =1;
	
	while (game_is_running==1){
		int info_from_server;
		puts("Waiting for information from server\n");
		read(sock, &info_from_server, sizeof(int));
		switch(info_from_server){
			case 0:
				puts("Game has ended\n");
				game_is_running=0;
			break;
			case 1: ;
				int valid =0;
				int message=0;
				while (valid==0){
					clear();
					puts("Give me Rock(1), Paper(2), Scissors(3)\n");
					scanf("%d", &message);
					if (message ==1 ||message==2||message==3) valid=1;
				}
				send(sock  , &message , sizeof(int) , 0 );
				clear();
			break;
			case 2: ;
				int status[3]={0}; 		//	s[0]- 1==won round, 0==died 
													//	s[1] - max_place;
													//  s[2] - number_of_players;
				read( sock ,&status, 3*sizeof(int));
				if (status[0]==0){
					printf("You survived this round and you are still fighting for %d place\n", status[1]);	
				}
				else {
					printf("You lost this round but you are still fighting for %d place\n", status[1]);	
				}
				printf("There are %d opponents\n", status[2]);
			break;
			default:
			perror("Something went very wrong"); exit(0);
			break;
		}
	}
	
	return 0;
}

int connectToServer(void){
	int sock = 0; 
	struct sockaddr_in serv_addr; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("\n Socket creation error \n"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT);      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  { 
        perror("\nInvalid address/ Address not supported \n"); 
        exit(EXIT_FAILURE); 
    }    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
        printf("\nConnection Failed \n"); 
        exit(EXIT_FAILURE); 
    }
	return sock;
}