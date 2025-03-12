// Need to add the functionality of do yoyu want play game (yes/no)?
// Some randomness while printing the error need to check it only for tcp b 
// Same for TCP
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>

#define PORT_USED 8080
#define SIZE 3

char board[SIZE+1][SIZE+1];
int players_array[2];
struct sockaddr_in players_addr[2];
socklen_t players_addr_len = sizeof(struct sockaddr_in);
int server_fileD;
int presentPlayer = 0;

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


void displayBoard(char *buffer) {
    // Clear the buffer before using it
    buffer[0] = '\0';

    // Format the board into the buffer
    for (int i = 1; i <= SIZE; i++) {
        for (int j = 1; j <= SIZE; j++) {
            // Append the current cell to the buffer
            snprintf(buffer + strlen(buffer), 1024 - strlen(buffer), " %c ", board[i][j] == ' ' ? ' ' : board[i][j]);
            if (j <= SIZE) {
                strcat(buffer, "|");  // Add vertical separator
            }
        }
        strcat(buffer, "\n");  // New line after each row
        if (i <= SIZE) {
            strcat(buffer, "---|---|---\n");  // Add horizontal separator
        }
    }
}

void sending_status_board_players() {
    char status_board[1024];
    int status_board_len = strlen(status_board);
    snprintf(status_board,sizeof(status_board),"Present Board:\n");
    for(int i=1;i<=SIZE;i++) {
        for(int j=1;j<=SIZE;j++) {
            snprintf(status_board+strlen(status_board),sizeof(status_board)-strlen(status_board),"%c ",board[i][j]);
        }
        snprintf(status_board+strlen(status_board),sizeof(status_board)-strlen(status_board),"\n");
    }

    sendto(server_fileD,status_board,strlen(status_board),0,(struct sockaddr *)&players_addr[0],players_addr_len);
    sendto(server_fileD,status_board,strlen(status_board),0,(struct sockaddr *)&players_addr[1],players_addr_len);
}

int winningGame() {
    for(int i=1;i<=SIZE;i++) {
        if((board[i][1] != ' ' && board[i][1] == board[i][2] && board[i][2] == board[i][3]) || (board[1][i] != ' ' && board[1][i] == board[2][i] && board[2][i] == board[3][i])) {
            return 1;
        }
    }
    if((board[1][1] != ' ' && board[1][1] == board[2][2] && board[2][2] == board[3][3]) || (board[1][3] != ' ' && board[1][3] == board[2][2] && board[2][2] == board[3][1])) {
        return 1;
    }
    return 0;
}

int notwinningGame() {
    for(int i=1;i<=SIZE;i++) {
        for(int j=1;j<=SIZE;j++) {
            if(board[i][j] == ' ') {
                return 0;
            }
        }
    }
    return 1;
}

void client_server_handling(int counter) {
    char temp[1024];
    int rows;
    int cols;
    char output[1024];
    char acceptance_1[1024];
    char acceptance_2[1024];

    // sendBoardToPlayers();

    printf("It's player %d's turn.\n",counter+1);
    snprintf(temp,sizeof(temp),"Enter your move (row and column seperated by the space):\n");
    sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[counter],players_addr_len);
    recvfrom(server_fileD,temp,sizeof(temp),0,(struct sockaddr *)&players_addr[counter],&players_addr_len);
    temp[strcspn(temp,"\n")] = '\0';

    if(sscanf(temp,"%d %d",&rows,&cols) != 2 || board[rows][cols] != ' ' || rows < 1 || rows > SIZE || cols < 1 || cols > SIZE) {
        snprintf(temp,sizeof(temp),"Invalid move.! Please enter correct move.\n");
        sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[counter],players_addr_len);
        client_server_handling(counter);
        // return;
    }
    if(board[rows][cols] == ' ' && rows >= 1 && rows <= SIZE && cols >= 1 && cols <= SIZE)  {
        if(counter == 0) {
            board[rows][cols] = 'X';
        }
        else {
            board[rows][cols] = 'O';
        }
        // sendBoardToClients();
        print_board();

        if(winningGame()) {
            snprintf(temp,sizeof(temp),"Player %d wins !!\n",counter+1);
            sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
            sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
            printf("Player %d wins !!\n",counter+1);
            strcpy(output,temp);
            // exit(1);
            // presentPlayer = 0;
            snprintf(temp,sizeof(temp),"Do you want to play again (yes/no)??\n");
            sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
            memset(temp,0,sizeof(temp));
            recvfrom(server_fileD,temp,sizeof(temp),0,(struct sockaddr *)&players_addr[0],&players_addr_len);
            temp[strcspn(temp,"\n")] = '\0';
            strcpy(acceptance_1,temp);

            snprintf(temp,sizeof(temp),"Do you want to play again (yes/no)??\n");
            sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
            memset(temp,0,sizeof(temp));
            recvfrom(server_fileD,temp,sizeof(temp),0,(struct sockaddr *)&players_addr[1],&players_addr_len);
            temp[strcspn(temp,"\n")] = '\0';
            // printf("%s\n",temp);
            strcpy(acceptance_2,temp);

            // printf("%s %s\n",acceptance_1,acceptance_2);

            if(strcmp(acceptance_1,"yes") == 0 && strcmp(acceptance_2,"yes") == 0) {
                snprintf(temp,sizeof(temp),"Both the players want to play the game again. Restarting the game..\n");
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
                creating_board();
                if(strstr(output,"Player 1 wins")) {
                    counter = 0;
                    client_server_handling(counter);
                }
            }
            else if(strcmp(acceptance_1,"no") == 0 && strcmp(acceptance_2,"no") == 0) {
                snprintf(temp,sizeof(temp),"Both the players dont want to play the game. Exiting the game...\n");
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
                exit(0);
            } 
            else if(strcmp(acceptance_1,"no") == 0 && strcmp(acceptance_2,"yes") == 0) {
                snprintf(temp,sizeof(temp),"Player 1 doesnt want to continue the game so exiting the game..\n");
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
                exit(0);
            }
            else {
                snprintf(temp,sizeof(temp),"Player 2 doesnt want to continue the game so exiting the game..\n");
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
                exit(0);
            }
        }
        else if(notwinningGame()) {
            snprintf(temp,sizeof(temp),"It's a draw !!\n");
            sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
            sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
            printf("It's a draw !!\n");
            strcpy(output,temp);

            snprintf(temp,sizeof(temp),"Do you want to play again (yes/no)??\n");
            sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
            memset(temp,0,sizeof(temp));
            recvfrom(server_fileD,temp,sizeof(temp),0,(struct sockaddr *)&players_addr[0],&players_addr_len);
            temp[strcspn(temp,"\n")] = '\0';
            strcpy(acceptance_1,temp);

            snprintf(temp,sizeof(temp),"Do you want to play again (yes/no)??\n");
            sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
            memset(temp,0,sizeof(temp));
            recvfrom(server_fileD,temp,sizeof(temp),0,(struct sockaddr *)&players_addr[1],&players_addr_len);
            temp[strcspn(temp,"\n")] = '\0';
            strcpy(acceptance_2,temp);

            if(strcmp(acceptance_1,"yes") == 0 && strcmp(acceptance_2,"yes") == 0) {
                snprintf(temp,sizeof(temp),"Both the players want to play the game again. Restarting the game..\n");
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
                creating_board();
                if(strstr(output,"Player 1 wins")) {
                    counter = 0;
                    client_server_handling(counter);
                }
            }
            else if(strcmp(acceptance_1,"no") == 0 && strcmp(acceptance_2,"no") == 0) {
                snprintf(temp,sizeof(temp),"Both the players dont want to play the game. Exiting the game...\n");
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
                exit(0);
            } 
            else if(strcmp(acceptance_1,"no") == 0 && strcmp(acceptance_2,"yes") == 0) {
                snprintf(temp,sizeof(temp),"Player 1 doesnt want to continue the game so exiting the game..\n");
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
                exit(0);
            }
            else {
                snprintf(temp,sizeof(temp),"Player 2 doesnt want to continue the game so exiting the game..\n");
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[0],players_addr_len);
                sendto(server_fileD,temp,strlen(temp),0,(struct sockaddr *)&players_addr[1],players_addr_len);
                exit(0);
            }
        }
        sending_status_board_players();
    }   
}

int main() {
    struct sockaddr_in server_addr;
    server_fileD = socket(AF_INET,SOCK_DGRAM,0);
    if(server_fileD < 0) {
        perror("Error in creating the socket");
        return -1;
    }

    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_USED);

    if(bind(server_fileD,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("Error in binding");
        close(server_fileD);
        return -1;
    }

    printf("Waiting for the players to connect......\n");

    for(int i=0;i<2;i++) {
        char temp[1024];
        // memset(temp,0,sizeof(temp));
        recvfrom(server_fileD,temp,sizeof(temp),0,(struct sockaddr*)&players_addr[i],&players_addr_len);
        printf("player %d connected.\n",i+1);

        if(i == 0) {
           sendto(server_fileD,"You are connected and you are player 1 (X).\n",50,0,(struct sockaddr *)&players_addr[0],players_addr_len);
        }
        else{
            sendto(server_fileD,"You are connected and you are player 2 (O).\n",50,0,(struct sockaddr *)&players_addr[1],players_addr_len);
        }
        // usleep(1000);
    }
    creating_board();

    while(1) {
        client_server_handling(presentPlayer);
        presentPlayer = (presentPlayer+1)%2;
    }
    close(server_fileD);
    return 0;
}



// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define PORT 8080
// #define BUFFER_SIZE 1024
// #define BOARD_SIZE 3

// int server_fd;
// struct sockaddr_in players[2];
// char board[BOARD_SIZE][BOARD_SIZE];
// int currentPlayer;

// void initializeBoard() {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             board[i][j] = ' ';
//         }
//     }
// }

// void printBoard() {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c", board[i][j]);
//             if (j < BOARD_SIZE - 1) printf("|");
//         }
//         printf("\n");
//         if (i < BOARD_SIZE - 1) printf("-----\n");
//     }
// }

// int checkWin() {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') {
//             return 1; // Row win
//         }
//         if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') {
//             return 1; // Column win
//         }
//     }
//     if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') {
//         return 1; // Diagonal win
//     }
//     if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') {
//         return 1; // Anti-diagonal win
//     }
//     return 0; // No win
// }

// void handleClientMove(int playerIndex) {
//     char buffer[BUFFER_SIZE];
//     int row, col;

//     // Notify current player to make a move
//     snprintf(buffer, sizeof(buffer), "Player %d's turn. Enter your move (row and column): ", playerIndex + 1);
//     sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&players[playerIndex], sizeof(players[playerIndex]));

//     // Receive move from client
//     socklen_t addrlen = sizeof(players[playerIndex]);
//     int n = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&players[playerIndex], &addrlen);
//     if (n < 0) {
//         perror("Error receiving move");
//         exit(EXIT_FAILURE);
//     }
//     buffer[n] = '\0'; // Null terminate the string
//     sscanf(buffer, "%d %d", &row, &col);

//     // Place the move on the board
//     board[row][col] = (playerIndex == 0) ? 'X' : 'O';

//     // Check for win
//     if (checkWin()) {
//         snprintf(buffer, sizeof(buffer), "Player %d wins!\n", playerIndex + 1);
//         sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&players[0], sizeof(players[0]));
//         sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&players[1], sizeof(players[1]));
//         return; // Exit to avoid further processing
//     }

//     // Check for draw
//     int draw = 1;
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             if (board[i][j] == ' ') {
//                 draw = 0; // Found an empty spot
//                 break;
//             }
//         }
//     }
//     if (draw) {
//         snprintf(buffer, sizeof(buffer), "It's a draw!\n");
//         sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&players[0], sizeof(players[0]));
//         sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&players[1], sizeof(players[1]));
//         return; // Exit to avoid further processing
//     }

//     // Print the board to console for debugging
//     printBoard();
// }

// void playAgain() {
//     char response[BUFFER_SIZE];
//     for (int i = 0; i < 2; i++) {
//         snprintf(response, sizeof(response), "Do you want to play again? (y/n): ");
//         sendto(server_fd, response, strlen(response), 0, (struct sockaddr *)&players[i], sizeof(players[i]));

//         // Receive response
//         socklen_t addrlen = sizeof(players[i]);
//         recvfrom(server_fd, response, BUFFER_SIZE, 0, (struct sockaddr *)&players[i], &addrlen);
//         if (response[0] != 'y') {
//             printf("Player %d has chosen not to play again.\n", i + 1);
//             exit(0); // Exit the server if any player chooses not to play again
//         }
//     }
// }

// int main() {
//     struct sockaddr_in server_addr;
//     socklen_t addrlen = sizeof(server_addr);

//     // Create socket
//     if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//         perror("Socket creation failed");
//         exit(EXIT_FAILURE);
//     }

//     // Bind socket to address
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
//         perror("Bind failed");
//         exit(EXIT_FAILURE);
//     }

//     printf("Server is running...\n");

//     // Accept connections for two players
//     for (int i = 0; i < 2; i++) {
//         printf("Waiting for Player %d to connect...\n", i + 1);
//         recvfrom(server_fd, NULL, 0, 0, (struct sockaddr *)&players[i], &addrlen);
//         printf("Player %d connected.\n", i + 1);
//     }

//     // Game loop
//     while (1) {
//         initializeBoard(); // Initialize the board
//         currentPlayer = 0; // Reset current player
//         while (1) {
//             handleClientMove(currentPlayer);
//             if (checkWin() || (checkWin() && currentPlayer == 1)) {
//                 break; // Break the inner loop on win or draw
//             }
//             currentPlayer = (currentPlayer + 1) % 2; // Switch player
//         }

//         // Ask if players want to play again
//         playAgain();
//     }

//     close(server_fd);
//     return 0;
// }
