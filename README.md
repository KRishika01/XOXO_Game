# Multiplayer Tic-Tac-Toe Game (TCP & UDP)

## Overview  
This project implements a **multiplayer Tic-Tac-Toe game** using **networking concepts**. The game consists of a **server** that manages the game state and **two clients** that act as players competing against each other. The implementation is provided using **both TCP and UDP sockets**.

## Features  
- **3x3 Tic-Tac-Toe board**  
- **Server-managed game state**  
- **Two players (clients) playing alternately**  
- **Input validation (rejects invalid moves)**  
- **Game outcome detection** (Win, Draw)  
- **Replay option** after game completion  
- **TCP and UDP versions**  

## Game Rules  
1. The game starts when **both players connect** to the server.  
2. The server assigns **Player 1 ('X')** and **Player 2 ('O')**.  
3. Players take turns by specifying **row and column** (e.g., `1 1` for the top-left corner).  
4. The server updates and broadcasts the **game board** after every valid move.  
5. The game ends when:  
   - A player wins by forming a row, column, or diagonal.  
   - The board is full, resulting in a **draw**.  
6. After the game, players can **choose to replay** or **disconnect**.  

## Installation & Usage  
### Prerequisites  
- **C Programming Language**  
- **Linux/macOS** (for socket programming)  
- **GCC Compiler** (`gcc`)  

### Compilation  
#### TCP Version  
```sh
gcc -o server_tcp server_tcp.c  
gcc -o client_tcp client_tcp.c  
```

#### UDP Version
```sh
gcc -o server_udp server_udp.c  
gcc -o client_udp client_udp.c  
``` 
### Running the Game
#### TCP Version

##### Start the server:
```sh
./server_tcp
```

##### Start two clients in separate terminals:
```sh
    ./client_tcp
    ./client_tcp
```
#### UDP Version

##### Start the server:
```sh
./server_udp
```
##### Start two clients in separate terminals:
```sh
./client_udp
./client_udp
```