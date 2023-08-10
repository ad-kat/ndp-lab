#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void performOperation(int* arr, int l, int operation);

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == -1) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in serverAddr, clientAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(3388); // Server port
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address

    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        return 1;
    }

    if (listen(serverSock, 5) == -1) {
        perror("Error listening");
        return 1;
    }

    printf("Server is listening for connections...\n");

    int clientSock;
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (1) {
        clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSock == -1) {
            perror("Error accepting connection");
            continue; // Continue to the next iteration and keep listening for new connections
        }

        printf("Connected to client.\n"); //inet_ntoa(clientAddr.sin_addr)--> to read ip address

        int arr[20];
        int l;
        char operation;

        recv(clientSock, &l, sizeof(l), 0);
        recv(clientSock, arr, l * sizeof(int), 0);
        recv(clientSock, &operation, sizeof(operation), 0);

        if (operation == 4) {
            printf("server now closed.\n");
            close(clientSock);
            break; // Exit the loop and close the server
        }
        
        else {
            if (operation == 1 || operation == 2 || operation == 3) {
                performOperation(arr, l, operation);
                send(clientSock, arr, l * sizeof(int), 0);
            }
            
            else
            {
                printf("Invalid operation code: %d\n", operation);
                const char* invalidMsg = "Invalid operation code.";
                send(clientSock, invalidMsg, strlen(invalidMsg), 0);
            }
        }

        close(clientSock);
    }

    close(serverSock);
    return 0;
}


void performOperation(int* arr, int l, int operation) {
    switch (operation) {
        case 1: // Sort in ascending order
            for (int i = 0; i < l - 1; i++) {
                for (int j = 0; j < l - i - 1; j++) {
                    if (arr[j] > arr[j + 1]) {
                        int temp = arr[j];
                        arr[j] = arr[j + 1];
                        arr[j + 1] = temp;
                    }
                }
            }
            break;

        case 2: // Sort in descending order
            for (int i = 0; i < l - 1; i++) {
                for (int j = 0; j < l - i - 1; j++) {
                    if (arr[j] < arr[j + 1]) {
                        int temp = arr[j];
                        arr[j] = arr[j + 1];
                        arr[j + 1] = temp;
                    }
                }
            }
            break;

        case 3: // Split into odd and even
            {
                int odd_count = 0;
                int even_count = 0;
                int odd_arr[100];
                int even_arr[100];

                for (int i = 0; i < l; i++) {
                    if (arr[i] % 2 == 0) {
                        even_arr[even_count++] = arr[i];
                    } else {
                        odd_arr[odd_count++] = arr[i];
                    }
                }

                // Copy the odd numbers first, then the even numbers
                memcpy(arr, odd_arr, odd_count * sizeof(int));
                memcpy(&arr[odd_count], even_arr, even_count * sizeof(int));
            }
            break;

        default:
            printf("Invalid operation code: %d\n", operation);
            break;
    }
}