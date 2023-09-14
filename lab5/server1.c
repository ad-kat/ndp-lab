#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_MSG_LEN 1024
#define MAX_CLIENTS 2

void handleClient(int clientSocket, const char* message) {
    int fd;
    char buffer[MAX_MSG_LEN];
    
    // Open the file for appending
    fd = open("output.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    
    // Write the message to the file
    write(fd, message, strlen(message));
    close(fd);

    // Display the message and client's socket address
    printf("%s\n", message);
    
    // Close the client socket
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int connectedClients = 0;

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

    while (connectedClients < MAX_CLIENTS) {
        // Accept a client connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("accept");
            continue;
        }
        
        char message[MAX_MSG_LEN];
        ssize_t bytesRead = recv(clientSocket, message, sizeof(message), 0);
        if (bytesRead == -1) {
            perror("recv");
            close(clientSocket);
            continue;
        }
        message[bytesRead] = '\0';

        // Handle the client's message
        handleClient(clientSocket, message);

        connectedClients++;
    }

    // Send "terminate session" to all connected clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        send(clientSocket, "terminate session", strlen("terminate session"), 0);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
