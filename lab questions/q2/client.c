#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    int clientSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSock == -1) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Server port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    printf("UDP Client is running...\n");

    while (1) {
        char buffer[MAX_BUFFER_SIZE];
        printf("Enter a string (or 'Halt' to terminate): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Remove the trailing newline from the input
        buffer[strcspn(buffer, "\n")] = '\0';

        // Send the string to the server
        sendto(clientSock, buffer, strlen(buffer), 0,
               (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        // Check for the exit condition
        if (strcmp(buffer, "Halt") == 0) {
            printf("Sent 'Halt' to the server. Client is terminating.\n");
            break; // Exit the loop and terminate the client
        }

        char resultBuffer[MAX_BUFFER_SIZE];

        // Receive the result from the server
        int bytesReceived = recvfrom(clientSock, resultBuffer, sizeof(resultBuffer), 0, NULL, NULL);
        if (bytesReceived == -1) {
            perror("Error receiving data");
            continue; // Continue to the next iteration
        }

        resultBuffer[bytesReceived] = '\0';
        printf("Received from server: %s\n", resultBuffer);
    }

    close(clientSock);
    return 0;
}