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
    char message[MAX_MSG_LEN];

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        perror("socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    printf("Enter a string to estimate permutations: ");
    fgets(message, sizeof(message), stdin);

    // Send the message to the server
    ssize_t bytesSent = sendto(clientSocket, message, strlen(message), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bytesSent == -1) {
        perror("sendto");
        exit(1);
    }

    printf("Message sent to server.\n");

    close(clientSocket);

    return 0;
}
