#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#define PORT 8080
#define MAX_MSG_LEN 1024

int areAnagrams(const char *str1, const char *str2) {
    int count[256] = {0};
printf("inside\n");
    // Lengths of both strings must be the same for them to be anagrams
    if (strlen(str1) != strlen(str2)) {
        return 0;
    }

    // Count characters in str1
    for (int i = 0; str1[i]; i++) {
        count[(int)str1[i]]++;
    }

    // Decrement count for characters in str2
    for (int i = 0; str2[i]; i++) {
        count[(int)str2[i]]--;
        printf("procedure\n");
    }

    // Check if all character counts are zero
    for (int i = 0; i < 256; i++) {
        if (count[i] != 0) {
            return 0;
        }
    }
    
printf("success\n");
    return 1;
}

void handleClient(int clientSocket, struct sockaddr_in *clientAddr) {
    char string1[MAX_MSG_LEN], string2[MAX_MSG_LEN];
    char response[MAX_MSG_LEN];
    char addrStr[INET_ADDRSTRLEN];
    time_t currentTime;
    struct tm *timeInfo;

    // Get the client's IP address and port
    inet_ntop(AF_INET, &(clientAddr->sin_addr), addrStr, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddr->sin_port);

    // Get the current time
    currentTime = time(NULL);
    timeInfo = localtime(&currentTime);

    // Display date and time along with client's socket address
    printf("Connected to client at %s:%d, Time: %s", addrStr, clientPort, asctime(timeInfo));
    
    printf("receiving strings\n");

    // Receive two strings from the client
    recv(clientSocket, string1, sizeof(string1), 0);
    recv(clientSocket, string2, sizeof(string2), 0);
	printf("received strings\n");
    // Check if the strings are anagrams
    if (areAnagrams(string1, string2)) {
        snprintf(response, sizeof(response), "anagrams.");
    } else {
        snprintf(response, sizeof(response), "not anagrams.");
    }
    printf("sending  response\n");

    // Send the response to the client
    send(clientSocket, response, strlen(response), 0);

    // Close the client socket
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

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

    while (1) {
        // Accept a client connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("accept");
            continue;
        }

        // Handle the client
        handleClient(clientSocket, &clientAddr);
        printf("still in while");
    }

    close(serverSocket);

    return 0;
}
