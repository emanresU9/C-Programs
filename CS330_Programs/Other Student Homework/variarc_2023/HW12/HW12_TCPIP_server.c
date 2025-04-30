/**
 * name: Christopher Variara
 * course: CS330
 * date: December 1, 2023
 * homework label: HW12_TCPIP_server.cpp
 * file location: ~variarc/www/HW12/HW12_TCPIP_server.cpp
 * assignment: Uses TCP/IP to implement a client/server
 *             network connection. This is server side.
 * compiler: gcc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main()
{
  int serverSocket, clientSocket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];

  // Create the server socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1)
  {
    perror("Error creating socket");
    exit(EXIT_FAILURE);
  }

  // Set socket option to reuse the address
  int enable = 1;
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(EXIT_FAILURE);
  }

  // Initialize server address structure
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind the socket
  if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("Error binding");
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(serverSocket, 5) == -1)
  {
    perror("Error listening");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", PORT);

  // Accept connection from client
  clientSocket = accept(serverSocket, (struct sockaddr *)&client_addr, &client_addr_len);
  if (clientSocket == -1)
  {
    perror("Error accepting connection");
    exit(EXIT_FAILURE);
  }

  printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

  // Communication loop
  while (1)
  {
    // Receive message from client
    recv(clientSocket, buffer, BUFFER_SIZE, 0);
    printf("Client: %s\n", buffer);

    // Send message to client
    printf("Enter message for client: ");
    fflush(stdout);
    fgets(buffer, BUFFER_SIZE, stdin);

    buffer[strcspn(buffer, "\n")] = '\0';

    send(clientSocket, buffer, strlen(buffer), 0);
  }

  // Close the client and server sockets
  close(clientSocket);
  close(serverSocket);

  return 0;
}
