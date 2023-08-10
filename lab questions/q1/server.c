#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == -1) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(3388); // Server port
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // Server IP (localhost)

    if (connect(clientSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        return 1;
    }

    printf("Connected to server.\n");

    int arr[20];
    int l;
    char operation;

    while (1) {
        printf("Length of array: ");
        scanf("%d", &l);

        if (l <= 0 || l > 100) {
            printf("impossible.\n");
            continue;
        }

        printf("Enter array elements: ");
        for (int i = 0; i < l; i++) {
            scanf("%d", &arr[i]);
        }

        
        printf("1.ascending order.\n");
        printf("2.descending order.\n");
        printf("3.odd-even split.\n");
        printf("4.exit.\n ");
        printf("enter choice");
        // getchar(); // consume newline character from previous scanf
        scanf("%d", &operation);

        if (operation == 4) {
            break; // Exit the loop and close the client
        } else {
            send(clientSock, &l, sizeof(l), 0);
            send(clientSock, arr, l * sizeof(int), 0);
            send(clientSock, &operation, sizeof(operation), 0);

            recv(clientSock, arr, l * sizeof(int), 0);

            printf("Result: ");
            for (int i = 0; i < l; i++) {
                printf("%d ", arr[i]);
            }
            printf("\n");
        }
    }

    close(clientSock);
    return 0;
}