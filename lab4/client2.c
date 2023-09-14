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
    char query[MAX_MSG_LEN];

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

    printf("Enter a domain name to query: ");
    fgets(query, sizeof(query), stdin);

    // Send the query to the server
    send(clientSocket, query, strlen(query), 0);

    // Receive and display the response from the server
    char response[MAX_MSG_LEN];
    ssize_t bytesRead = recv(clientSocket, response, sizeof(response), 0);

    if (bytesRead == -1) {
        perror("recv");
    } else {
        response[bytesRead] = '\0';
        printf("Server Response: %s\n", response);
    }

    close(clientSocket);

    return 0;
}
