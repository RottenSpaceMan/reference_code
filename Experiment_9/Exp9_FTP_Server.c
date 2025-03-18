// Server Code(server.c)
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void receive_file(int client_socket){
  char buffer[BUFFER_SIZE];
  FILE *file = fopen("received_file.txt", "wb");
  if(!file){
    perror("File open failed");
    return;
  }
  int bytes_received;
  while((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0){
    fwrite(buffer, 1, bytes_received, file);
  }
  fclose(file);
  printf("File received successfully\n");
}

int main(){
  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket == -1){
    perror("Socket creation failed");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  
  if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    perror("Bind failed");
    exit(1);
  }

  if(listen(server_socket, 5) < 0){
    perror("Listen failed");
    exit(1);
  }
  printf("Server listening on port %d\n", PORT);

  addr_size = sizeof(client_addr);
  client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
  if(client_socket < 0){
    perror("Accept failed");
    exit(1);
  }
    receive_file(client_socket);
    close(client_socket);
    close(server_socket);
    return 0;
}
