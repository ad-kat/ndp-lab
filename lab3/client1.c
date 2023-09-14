#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_MSG_LEN 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("connect");
        exit(1);
    }

    char message[MAX_MSG_LEN];

    while (1) {
        printf("Client: ");
        fgets(message, sizeof(message), stdin);

        // Send the message to the server
        send(clientSocket, message, strlen(message), 0);

        // Receive a response from the server
        recv(clientSocket, message, sizeof(message), 0);
        printf("Server: %s", message);
    }

    close(clientSocket);

    return 0;
}
