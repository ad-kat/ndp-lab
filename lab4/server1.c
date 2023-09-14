#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_MSG_LEN 1024

struct Student {
    char registrationNumber[50];
    char name[100];
    char subjectCode[10];
    char address[100];
    char enrollmentDetails[200];
    char marks[50];
};

void handleRegistrationNumber(int clientSocket, struct Student *student) {
    strcpy(student->registrationNumber, "12345");
    strcpy(student->name, "John Doe");
    strcpy(student->address, "123 Main St, City");
    send(clientSocket, student, sizeof(*student), 0);
}

void handleStudentName(int clientSocket, struct Student *student) {
    strcpy(student->enrollmentDetails, "Dept: CSE, Semester: 4, Section: A, Courses: Math, CS");
    send(clientSocket, student, sizeof(*student), 0);
}

void handleSubjectCode(int clientSocket, struct Student *student) {
    strcpy(student->marks, "Subject: Math, Marks: 95");
    send(clientSocket, student, sizeof(*student), 0);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    struct Student student;
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

        int option;
        recv(clientSocket, &option, sizeof(option), 0);

        pid_t childPID = fork();

        if (childPID == -1) {
            perror("fork");
            exit(1);
        }

        if (childPID == 0) {
            // In child process
            close(serverSocket);

            if (option == 1) {
                handleRegistrationNumber(clientSocket, &student);
            } else if (option == 2) {
                handleStudentName(clientSocket, &student);
            } else if (option == 3) {
                handleSubjectCode(clientSocket, &student);
            }

            printf("Child process (PID: %d) handled option %d\n", getpid(), option);
            close(clientSocket);
            exit(0);
        } else {
            // In parent process
            wait(NULL);
            close(clientSocket);
        }
    }

    close(serverSocket);

    return 0;
}
