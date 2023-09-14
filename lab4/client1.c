#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_MSG_LEN 1024

struct Student {
    char registrationNumber[50];
    char name[100];
    char subjectCode[10];
    char address[100];
    char enrollmentDetails[200];
    char marks[50];
};

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    struct Student student;
    int option;

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

    printf("Select an option:\n");
    printf("1. Registration Number\n");
    printf("2. Name of the Student\n");
    printf("3. Subject Code\n");
    scanf("%d", &option);

    // Send the selected option to the server
    send(clientSocket, &option, sizeof(option), 0);

    // Receive and display the result from the server
    recv(clientSocket, &student, sizeof(student), 0);

    if (option == 1) {
        printf("Student Name: %s\n", student.name);
        printf("Residential Address: %s\n", student.address);
    } else if (option == 2) {
        printf("Student Enrollment Details: %s\n", student.enrollmentDetails);
    } else if (option == 3) {
        printf("Marks Obtained: %s\n", student.marks);
    }

    close(clientSocket);

    return 0;
}
