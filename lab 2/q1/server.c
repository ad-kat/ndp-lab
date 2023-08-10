#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

// Function to handle client requests and perform actions based on the menu.
void handleClient(int clientSocket);

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345); // Choose an appropriate port number

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    listen(serverSocket, 5);

    printf("Server is listening...\n");

    while (1) {
        // Accept incoming client connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Handle client requests
        handleClient(clientSocket);

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}

void handleClient(int clientSocket) {
    char buffer[MAX_BUFFER_SIZE];
    FILE *file;
    char filename[MAX_BUFFER_SIZE];
    char option;
    char searchString[MAX_BUFFER_SIZE], replaceString[MAX_BUFFER_SIZE];
    int count;

    // Receive the filename from the client
    recv(clientSocket, filename, sizeof(filename), 0);

    // Open the file for reading
    file = fopen(filename, "r");
    if (file == NULL) {
        // File not present, send error message and terminate
        send(clientSocket, "File not present", strlen("File not present"), 0);
        return;
    }

    // File exists, send menu options to the client
    send(clientSocket, "1. Search\n2. Replace\n3. Reorder\n4. Exit\n", strlen("1. Search\n2. Replace\n3. Reorder\n4. Exit\n"), 0);

    // Receive option from the client
    recv(clientSocket, &option, sizeof(option), 0);

    switch (option) {
        case '1':
            // Search for a string in the file
            recv(clientSocket, searchString, sizeof(searchString), 0);
            count = 0;
            char line[MAX_BUFFER_SIZE];

            // Search the string in the file
            while (fgets(line, sizeof(line), file) != NULL) {
                if (strstr(line, searchString) != NULL) {
                    count++;
                }
            }

            // Send the count to the client
            sprintf(buffer, "Occurrences: %d\n", count);
            send(clientSocket, buffer, strlen(buffer), 0);
            break;

        case '2':
            // Replace a string in the file
            recv(clientSocket, searchString, sizeof(searchString), 0);
            recv(clientSocket, replaceString, sizeof(replaceString), 0);

            char tempFileName[] = "tempfile.txt";
            FILE *tempFile = fopen(tempFileName, "w");
            char currentLine[MAX_BUFFER_SIZE];

            // Replace the string and write to the temp file
            while (fgets(currentLine, sizeof(currentLine), file) != NULL) {
                char *pos;
                while ((pos = strstr(currentLine, searchString)) != NULL) {
                    int index = pos - currentLine;
                    strncpy(buffer, currentLine, index);
                    buffer[index] = '\0';
                    strcat(buffer, replaceString);
                    strcat(buffer, pos + strlen(searchString));
                    strcpy(currentLine, buffer);
                }
                fputs(currentLine, tempFile);
            }

            fclose(file);
            fclose(tempFile);

            // Replace the original file with the temp file
            remove(filename);
            rename(tempFileName, filename);

            // Send message to the client
            send(clientSocket, "String replaced", strlen("String replaced"), 0);
            break;

        case '3':
            // TODO: Implement the reorder option to sort the file content
            break;

        case '4':
            // Exit option, no action needed
            break;

        default:
            send(clientSocket, "Invalid option", strlen("Invalid option"), 0);
    }

    // Close the file
    fclose(file);
}