#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT_USED 8083
#define SIZE 3

char board[SIZE + 1][SIZE + 1]; 
int presentPlayer;
int players_array[2];

void creating_board() {
    for (int i = 1; i <= SIZE; i++) {
        for (int j = 1; j <= SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

void printingBoard() {
    printf("\nPresent Board:\n");
    for (int i = 1; i <= SIZE; i++) {
        for (int j = 1; j <= SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

void sending_board_to_client() {
    char board_status[1024];
    snprintf(board_status, sizeof(board_status), "Present Board:\n");
    for (int i = 1; i <= SIZE; i++) {
        for (int j = 1; j <= SIZE; j++) {
            snprintf(board_status + strlen(board_status), sizeof(board_status) - strlen(board_status), "%c ", board[i][j]);
        }
        snprintf(board_status + strlen(board_status), sizeof(board_status) - strlen(board_status), "\n");
    }
    send(players_array[0], board_status, strlen(board_status), 0);
    send(players_array[1], board_status, strlen(board_status), 0);
}

int winningGame() {
    for (int i = 1; i <= SIZE; i++) {
        if ((board[i][1] != ' ' && board[i][1] == board[i][2] && board[i][2] == board[i][3]) || (board[1][i] != ' ' && board[1][i] == board[2][i] && board[2][i] == board[3][i])) {
            return 1;
        }
    }
    if ((board[1][1] != ' ' && board[1][1] == board[2][2] && board[2][2] == board[3][3]) || (board[1][3] != ' ' && board[1][3] == board[2][2] && board[2][2] == board[3][1])) {
        return 1; 
    }
    return 0; 
}

int notwinningGame() {
    for (int i = 1; i <= SIZE; i++) {
        for (int j = 1; j <= SIZE; j++) {
            if (board[i][j] == ' ') {
                return 0; 
            }
        }
    }
    return 1; 
}

void restart_game() {
    creating_board();
    presentPlayer = 0;
}

void client_server_handling(int counter) {
    char temp[1024];
    int rows;
    int cols;

    printf("It's player %d's turn.\n", counter + 1);
    snprintf(temp, sizeof(temp), "Enter your move (row and column separated by space):\n");
    send(players_array[counter], temp, strlen(temp), 0);
    recv(players_array[counter], temp, sizeof(temp), 0);
    temp[strcspn(temp, "\n")] = '\0';


    if (sscanf(temp, "%d %d", &rows, &cols) != 2 || rows < 1 || rows > SIZE || cols < 1 || cols > SIZE || board[rows][cols] != ' ') {
        snprintf(temp, sizeof(temp), "Invalid move! Please enter a correct move.\n");
        send(players_array[counter], temp, strlen(temp), 0);
        client_server_handling(counter); 
        return; 
    }
    else if(rows >=1 && rows <= SIZE && cols >=1 && cols <= SIZE && board[rows][cols] == ' ') {
        if(counter == 0) {
            board[rows][cols] = 'X';
        }
        else {
            board[rows][cols] = 'O';
        }
        printingBoard();
        char acceptance_1[1024];
        char acceptance_2[1024];
        char output[1024];

        if(winningGame()) {
            snprintf(temp, sizeof(temp), "Player %d wins !!\n", counter + 1);
            send(players_array[0], temp, strlen(temp), 0);
            send(players_array[1], temp, strlen(temp), 0);
            printf("Player %d wins !!\n", counter + 1);
            sending_board_to_client(); 
            strcpy(output,temp);

            snprintf(temp, sizeof(temp), "Do you want to play again? (yes/no)\n");
            send(players_array[0], temp, strlen(temp), 0);
            recv(players_array[0], acceptance_1, sizeof(acceptance_1), 0);
            acceptance_1[strcspn(acceptance_1, "\n")] = '\0'; 
            
            send(players_array[1], temp, strlen(temp), 0);
            recv(players_array[1], acceptance_2, sizeof(acceptance_2), 0);
            acceptance_2[strcspn(acceptance_2, "\n")] = '\0'; 

            if(strcmp(acceptance_1,"yes") == 0 && strcmp(acceptance_2,"yes") == 0) {
                snprintf(temp, sizeof(temp), "Both players want to play the game again.Restarting the game.\n");
                send(players_array[0], temp, strlen(temp), 0);
                send(players_array[1], temp, strlen(temp), 0);
                // restart_game();
                creating_board();
                if(strcmp(output,"Player 1 wins") == 0) {
                    counter = 0;
                    client_server_handling(counter);
                }
                // return 1;
            }
            else if(strcmp(acceptance_1,"no") == 0 && strcmp(acceptance_2,"no") == 0) {
                snprintf(temp, sizeof(temp), "Both players dont want to play the game again.Closing the game.\n");
                send(players_array[0], temp, strlen(temp), 0);
                send(players_array[1], temp, strlen(temp), 0);
                exit(0);
            }
            else if(strcmp(acceptance_1,"yes") == 0 && strcmp(acceptance_2,"no") == 0) {
                snprintf(temp, sizeof(temp), "Player2 doesnt want to play the game again.Closing the game.\n");
                send(players_array[0], temp, strlen(temp), 0);
                send(players_array[1], temp, strlen(temp), 0);
                exit(0);
            }
            else if(strcmp(acceptance_1,"no") == 0 && strcmp(acceptance_2,"yes") == 0) {
                snprintf(temp, sizeof(temp), "Player1 doesnt want to play the game again.Closing the game.\n");
                send(players_array[0], temp, strlen(temp), 0);
                send(players_array[1], temp, strlen(temp), 0);
                exit(0);
            }
        }
        else if(notwinningGame()) {
            snprintf(temp, sizeof(temp), "It's a draw !!\n");
            send(players_array[0], temp, strlen(temp), 0);
            send(players_array[1], temp, strlen(temp), 0);
            printf("It's a draw!!\n");
            sending_board_to_client(); 
            strcpy(output,temp);

            snprintf(temp, sizeof(temp), "Do you want to play again? (yes/no)\n");
            send(players_array[0], temp, strlen(temp), 0);
            recv(players_array[0], acceptance_1, sizeof(acceptance_1), 0);
            acceptance_1[strcspn(acceptance_1, "\n")] = '\0'; 

            send(players_array[1], temp, strlen(temp), 0);
            recv(players_array[1], acceptance_2, sizeof(acceptance_2), 0);
            acceptance_2[strcspn(acceptance_2, "\n")] = '\0'; 

            if(strcmp(acceptance_1,"yes") == 0 && strcmp(acceptance_2,"yes") == 0) {
                snprintf(temp, sizeof(temp), "Both players want to play the game again.Restarting the game.\n");
                send(players_array[0], temp, strlen(temp), 0);
                send(players_array[1], temp, strlen(temp), 0);
                // restart_game();
                creating_board();
                if(strcmp(output,"It's a draw") == 0) {
                    counter = 0;
                    client_server_handling(counter);
                }
                
                // return 1;
            }
            else if(strcmp(acceptance_1,"no") == 0 && strcmp(acceptance_2,"no") == 0) {
                snprintf(temp, sizeof(temp), "Both players dont want to play the game again.Closing the game.\n");
                send(players_array[0], temp, strlen(temp), 0);
                send(players_array[1], temp, strlen(temp), 0);
                exit(0);
            }
            else if(strcmp(acceptance_1,"yes") == 0 && strcmp(acceptance_2,"no") == 0) {
                snprintf(temp, sizeof(temp), "Player 2 doesnt want to continue the game.Closing the game.\n");
                send(players_array[0], temp, strlen(temp), 0);
                send(players_array[1], temp, strlen(temp), 0);
                exit(0);
            }
            else if(strcmp(acceptance_1,"no") == 0 && strcmp(acceptance_2,"yes") == 0) {
                snprintf(temp, sizeof(temp), "Player 1 doesnt want to continue the game.Closing the game.\n");
                send(players_array[0], temp, strlen(temp), 0);
                send(players_array[1], temp, strlen(temp), 0);
                exit(0);
            }
        }
        sending_board_to_client();
    }
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    int server_fileD = socket(AF_INET, SOCK_STREAM, 0);
    socklen_t c_len = sizeof(client_addr);
    if (server_fileD < 0) {
        perror("Error in creating the socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_USED);

    if (bind(server_fileD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in binding");
        close(server_fileD);
        return -1;
    }

    if (listen(server_fileD, 2) < 0) {
        perror("Error in listening");
        close(server_fileD);
        return -1;
    }

    printf("Waiting for the players to connect......\n");
    for (int i = 0; i < 2; i++) {
        int new_server_fileD = accept(server_fileD, (struct sockaddr *)&client_addr, &c_len);
        if (new_server_fileD < 0) {
            perror("Error in accepting the connection");
            close(server_fileD);
            return -1;
        }
        players_array[i] = new_server_fileD;
        printf("Player %d connected\n", i + 1);

        // Notify players of their identity
        if (i == 0) {
            send(players_array[0], "You are connected and you are player 1 (X).\n", 44, 0);
        } else {
            send(players_array[1], "You are connected and you are player 2 (O).\n", 44, 0);
        }
    }

    creating_board();
    while (1) {
        client_server_handling(presentPlayer);
        presentPlayer = (presentPlayer + 1) % 2; // Switch to the next player
    }

    close(server_fileD);
    return 0;
}