#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[MAX_BUFFER_SIZE];
    char filename[MAX_BUFFER_SIZE];
    char option;

    // Create TCP socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with the server IP address
    serverAddr.sin_port = htons(12345); // Replace with the server port number

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error connecting to server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // Prompt user to enter the filename
    printf("Enter the filename: ");
    fgets(filename, sizeof(filename), stdin);
    strtok(filename, "\n"); // Remove the newline character

    // Send the filename to the server
    send(clientSocket, filename, strlen(filename), 0);

    // Receive response from the server
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    // Prompt user to choose an option
    printf("Enter your choice: ");
    option = getchar();
    getchar(); // Consume the newline character

    // Send the option to the server
    send(clientSocket, &option, sizeof(option), 0);

    // Process the chosen option
    switch (option) {
        case '1':
            // Search option
            printf("Enter the string to search: ");
            fgets(buffer, sizeof(buffer), stdin);
            strtok(buffer, "\n"); // Remove the newline character

            // Send the search string to the server
            send(clientSocket, buffer, strlen(buffer), 0);

            // Receive and display the search result
            recv(clientSocket, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
            break;

        case '2':
            // Replace option
            printf("Enter the string to replace: ");
            fgets(buffer, sizeof(buffer), stdin);
            strtok(buffer, "\n"); // Remove the newline character

            // Send the search string to the server
            send(clientSocket, buffer, strlen(buffer), 0);

            printf("Enter the replacement string: ");
            fgets(buffer, sizeof(buffer), stdin);
            strtok(buffer, "\n"); // Remove the newline character

            // Send the replacement string to the server
            send(clientSocket, buffer, strlen(buffer), 0);

            // Receive and display the replacement result
            recv(clientSocket, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);
            break;

        case '3':
            // TODO: Implement the reorder option
            break;

        case '4':
            printf("Terminating the application.\n");
            break;

        default:
            printf("Invalid option\n");
            break;
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}