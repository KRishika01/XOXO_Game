#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE 3
#define PORT_USED 8083

int main() {
    int client_fileD;
    struct sockaddr_in server_addr;
    socklen_t s_len = sizeof(server_addr);
    char temp[1024];

    client_fileD = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fileD < 0) {
        perror("Error in creating the socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_USED);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if (connect(client_fileD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in connecting to the server");
        close(client_fileD);
        return -1;
    }

    while (1) {
        int b_read = recv(client_fileD, temp, sizeof(temp) - 1, 0);
        if (b_read < 0) {
            perror("Error in receiving data");
            break;
        }

        temp[b_read] = '\0';
        printf("%s", temp);
        if(strstr(temp,"Both the players dont want to play the game.") || strstr(temp,"Player1 doesnt want to play the game again.") || strstr(temp,"Player2 doesnt want to play the game again.")) {
            break;
        }
        if (strstr(temp, "Enter your move")) {
            printf("Enter your move: \n");
            fgets(temp, sizeof(temp), stdin);
            send(client_fileD, temp, strlen(temp), 0);
        } else if (strstr(temp, "Do you want to play again?")) {
            printf("Enter your response: \n");
            fgets(temp, sizeof(temp), stdin);
            send(client_fileD, temp, strlen(temp), 0);
        }
    }

    close(client_fileD);
    return 0;
}
