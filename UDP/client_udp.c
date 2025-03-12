#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<arpa/inet.h>

#define SIZE 3
#define PORT_USED 8080
char board[SIZE+1][SIZE+1];

void creating_board() {
    for(int i=1;i<=SIZE;i++) {
        for(int j=1;j<=SIZE;j++) {
            board[i][j] = ' ';
        }
    }
}
 
void print_board() {
    printf("Present Board:\n");
    for(int i=1;i<=SIZE;i++) {
        for(int j=1;j<=SIZE;j++) {
            printf("%c ",board[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int b_read;
    int client_fileD;
    struct sockaddr_in client_addr;
    socklen_t c_len = sizeof(client_addr);
    char temp[1024];

    client_fileD = socket(AF_INET,SOCK_DGRAM,0);
    if(client_fileD < 0) {
        perror("Error in creating the socket");
        return -1;
    }
    memset(&client_addr,0,sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT_USED);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    snprintf(temp,sizeof(temp),"Connected to the server\n");
    sendto(client_fileD,temp,strlen(temp),0,(struct sockaddr *)&client_addr,c_len);
    while(1) {
        memset(temp,0,sizeof(temp));
        recvfrom(client_fileD,temp,sizeof(temp),0,NULL,NULL);
        printf("%s",temp);

        if(strstr(temp,"Both the players dont want to play the game") || strstr(temp,"Player 1 doesnt want to continue the game") || strstr(temp,"Player 2 doesnt want to continue the game")) {
            break;
        }

        if(strstr(temp,"Do you want to play again") != NULL) {
            printf("Please enter yes or no: ");
            fgets(temp,sizeof(temp),stdin);
            temp[strcspn(temp,"\n")] = '\0';
            sendto(client_fileD,temp,strlen(temp),0,(struct sockaddr *)&client_addr,c_len);
            continue;
            // break;
        }

        if(strstr(temp,"Enter your move (row and column seperated by the space)") != NULL) {
            // printf("Enter your move:\n");
            fgets(temp,sizeof(temp),stdin);
            sendto(client_fileD,temp,strlen(temp),0,(struct sockaddr *)&client_addr,c_len);
            continue;
        }        
    }
    close(client_fileD);
    return 0;
}
