/**
 * name: Christopher Variara
 * course: CS330
 * date: December 1, 2023
 * homework label: HW12_TCPIP_client.cpp
 * file location: ~variarc/www/HW12/HW12_TCPIP_client.cpp
 * assignment: Uses TCP/IP to implement a client/server
 *             network connection. This is client side.
 * compiler: gcc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

int main()
{
  int clientSocket;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];

  // Create the client socket
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1)
  {
    perror("Error creating socket");
    exit(EXIT_FAILURE);
  }

  // Initialize server address structure
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(PORT);

  // Connect to the server
  if (connect(clientSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("Error connecting to server");
    exit(EXIT_FAILURE);
  }

  printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

  // Communication loop
  while (1)
  {
    // Send message to server
    printf("Enter message for server: ");
    fflush(stdout);
    fgets(buffer, BUFFER_SIZE, stdin);

    buffer[strcspn(buffer, "\n")] = '\0';

    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive message from server
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    printf("Server: %s\n", buffer);
  }

  // Close the client socket
  close(clientSocket);

  return 0;
}
