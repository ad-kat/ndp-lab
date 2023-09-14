#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>

#define PORT 8080
#define MAX_MSG_LEN 1024

// Function to calculate permutations and print them
void permute(char *str, int l, int r) {
    if (l == r) {
        printf("%s\n", str);
    } else {
        for (int i = l; i <= r; i++) {
            // Swap characters at positions l and i
            char temp = str[l];
            str[l] = str[i];
            str[i] = temp;

            // Recursively generate permutations
            permute(str, l + 1, r);

            // Restore the original string (backtrack)
            temp = str[l];
            str[l] = str[i];
            str[i] = temp;
        }
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char message[MAX_MSG_LEN];

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
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

    printf("Server is listening on port %d...\n", PORT);

    socklen_t clientAddrLen = sizeof(clientAddr);

    // Receive the message from the client
    ssize_t bytesRead = recvfrom(serverSocket, message, sizeof(message), 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (bytesRead == -1) {
        perror("recvfrom");
        exit(1);
    }

    message[bytesRead] = '\0';

    // Print received string and estimate permutations
    printf("Client: %s\n", message);
    printf("Estimated Permutations:\n");
    permute(message, 0, strlen(message) - 1);

    close(serverSocket);

    return 0;
}
