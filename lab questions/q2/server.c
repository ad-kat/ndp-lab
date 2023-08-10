#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int isPalindrome(const char* str) {
    int len = strlen(str);
    int i, j;
    for (i = 0, j = len - 1; i < j; i++, j--) {
        if (str[i] != str[j]) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int serverSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSock == -1) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in serverAddr, clientAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Server port
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address

    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        close(serverSock);
        return 1;
    }

    printf("UDP Server is listening for connections...\n");

    char buffer[MAX_BUFFER_SIZE];
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (1) {
        // Receive string from the client
        int bytesReceived = recvfrom(serverSock, buffer, sizeof(buffer), 0,
                                     (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (bytesReceived == -1) {
            perror("Error receiving data");
            continue; // Continue to the next iteration
        }

        buffer[bytesReceived] = '\0';
        printf("Received from client: %s\n", buffer);

        // Check if the string is a palindrome
        int isPal = isPalindrome(buffer);

        // Send back the result to the client
        char resultBuffer[MAX_BUFFER_SIZE];
        snprintf(resultBuffer, sizeof(resultBuffer), "Palindrome: %s, Length: %zu",
                 isPal ? "Yes" : "No", strlen(buffer));

        sendto(serverSock, resultBuffer, strlen(resultBuffer), 0,
               (struct sockaddr*)&clientAddr, clientAddrLen);

        // Check for the exit condition
        if (strcmp(buffer, "Halt") == 0) {
            printf("Received 'Halt' from the client. Server is terminating.\n");
            break; // Exit the loop and terminate the server
        }
    }

    close(serverSock);
    return 0;
}