#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_MSG_LEN 1024

void sortCharsDescending(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            if (str[j] < str[j + 1]) {
                char temp = str[j];
                str[j] = str[j + 1];
                str[j + 1] = temp;
            }
        }
    }
}

void sortNumbersAscending(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            if (isdigit(str[j]) && isdigit(str[j + 1]) && str[j] > str[j + 1]) {
                char temp = str[j];
                str[j] = str[j + 1];
                str[j + 1] = temp;
            }
        }
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char message[MAX_MSG_LEN];

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

    socklen_t clientAddrLen = sizeof(clientAddr);

    while (1) {
        // Accept a client connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("accept");
            continue;
        }

        pid_t childPID = fork();

        if (childPID == -1) {
            perror("fork");
            exit(1);
        }

        if (childPID == 0) {
            // In child process
            close(serverSocket);

            // Receive the message from the client
            ssize_t bytesRead = recv(clientSocket, message, sizeof(message), 0);
            if (bytesRead == -1) {
                perror("recv");
                exit(1);
            }

            message[bytesRead] = '\0';

            // Sort numbers in ascending order
            sortNumbersAscending(message);

            // Send the sorted numbers to the client
            send(clientSocket, message, strlen(message), 0);

            printf("Output at the child process of the server (PID: %d): %s\n", getpid(), message);

            close(clientSocket);
            exit(0);
        } else {
            // In parent process
            // Wait for the child process to finish
            wait(NULL);

            // Sort characters in descending order
            sortCharsDescending(message);

            // Send the sorted characters to the client
            send(clientSocket, message, strlen(message), 0);

            printf("Output at the parent process of the server (PID: %d): %s\n", getpid(), message);

            close(clientSocket);
        }
    }

    close(serverSocket);

    return 0;
}
