#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8080
#define MAX_MSG_LEN 1024

void handleClient(int clientSocket) {
    char message[MAX_MSG_LEN];
    int bytesRead;

    while (1) {
        // Receive a message from the client
        bytesRead = recv(clientSocket, message, sizeof(message), 0);
        if (bytesRead <= 0) {
            perror("recv");
            break;
        }
        message[bytesRead] = '\0';

        // Display the received message
        printf("Client: %s", message);

        // Send a response to the client
        printf("Server: ");
        fgets(message, sizeof(message), stdin);
        send(clientSocket, message, strlen(message), 0);
    }

    close(clientSocket);
    exit(0);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    pid_t childPID;

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Accept a client connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("accept");
            exit(1);
        }

        // Fork a child process to handle the client
        childPID = fork();
        if (childPID == -1) {
            perror("fork");
            exit(1);
        }

        if (childPID == 0) {
            // In child process
            close(serverSocket);
            handleClient(clientSocket);
        } else {
            // In parent process
            printf("Client connected (PID: %d, PPID: %d)\n", childPID, getpid());
            close(clientSocket);
        }
    }

    close(serverSocket);

    return 0;
}
