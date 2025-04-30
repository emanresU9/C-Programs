#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6666
#define MAX_SIZE 80

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connection
    listen(server_fd, 3);

    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    //Infinite Loop
    while (1) {
        recv(new_socket, buffer, sizeof(buffer), 0);
        printf("Client: %s\n", buffer);

        printf("Enter message for the client: ");
        fgets(buffer, MAX_SIZE, stdin);
        send(new_socket, buffer, strlen(buffer), 0);
    }

    // Close socket
    close(new_socket);
    close(server_fd);

    return 0;
}
