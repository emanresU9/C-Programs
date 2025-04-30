#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6666
#define MAX_SIZE 80

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[MAX_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    //Infinite Loop
    while (1) {
        printf("Enter message for the server: ");
        fgets(buffer, MAX_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);

        recv(sock, buffer, sizeof(buffer), 0);
        printf("Server: %s\n", buffer);
    }

    //Close socket
    close(sock);

    return 0;
}
