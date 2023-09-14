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

struct DNSRecord {
    char domain[100];
    char ip[16];
};

void searchAndSendResponse(int clientSocket, const char *query, struct DNSRecord *records, int numRecords) {
    char response[MAX_MSG_LEN] = "DNS Record not found.";

    for (int i = 0; i < numRecords; i++) {
        if (strcmp(records[i].domain, query) == 0) {
            snprintf(response, sizeof(response), "Domain: %s, IP Address: %s", records[i].domain, records[i].ip);
            break;
        }
    }

    send(clientSocket, response, strlen(response), 0);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    struct DNSRecord records[100];
    int numRecords = 0;

    // Read DNS records from a file or initialize them manually as needed
    // Example:
    // strcpy(records[0].domain, "example.com");
    // strcpy(records[0].ip, "192.168.1.1");
    // numRecords = 1;

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

    printf("DNS Server is listening on port %d...\n", PORT);

    socklen_t clientAddrLen = sizeof(clientAddr);

    while (1) {
        // Accept a client connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("accept");
            continue;
        }

        char query[MAX_MSG_LEN];
        ssize_t bytesRead = recv(clientSocket, query, sizeof(query), 0);

        if (bytesRead <= 0) {
            perror("recv");
            close(clientSocket);
            continue;
        }

        query[bytesRead] = '\0';

        // Search for the DNS record and send the response
        searchAndSendResponse(clientSocket, query, records, numRecords);

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}
