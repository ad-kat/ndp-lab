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
    char string1[MAX_MSG_LEN], string2[MAX_MSG_LEN];
    char response[MAX_MSG_LEN];

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

    // Prompt the user to enter two strings
    printf("Enter the first string: ");
    fgets(string1, sizeof(string1), stdin);
    printf("Enter the second string: ");
    fgets(string2, sizeof(string2), stdin);
    printf("client-strings entered.\n");

    // Send the strings to the server
    send(clientSocket, string1, strlen(string1), 0);
    send(clientSocket, string2, strlen(string2), 0);
    printf("client-strings sent\n");

    // Receive and display the response from the server
    recv(clientSocket, response, sizeof(response), 0);
    printf("Server Response: %s\n", response);

    // Close the client socket
    close(clientSocket);

    return 0;
}
