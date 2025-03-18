// Client Code(client.c)
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_file(int socket, const char *filename){
  char buffer[BUFFER_SIZE];
  FILE *file = fopen(filename, "rb");
  if(!file){
    perror("File open failed");
    return;
  }
  int bytes_read;
  while((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0){
    send(socket, buffer, bytes_read, 0);
  }
  fclose(file);
  printf("File sent successfully\n");
}

int main(){
  int client_socket;
  struct sockaddr_in server_addr;

  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(client_socket == -1){
    perror("Socket creation failed");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    perror("Connection failed");
    exit(1);
  }

  send_file(client_socket, "file_to_send.txt");
  close(client_socket);
  return 0;
}
