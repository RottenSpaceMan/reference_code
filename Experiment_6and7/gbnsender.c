// sender.c 
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#define MAX_PACKET_SIZE 1024
#define WINDOW_SIZE 4
#define TOTAL_PACKETS 10 
#define PORT 8080

typedef struct{
  int seq_num;
  char data[MAX_PACKET_SIZE];
} Packet ;

int main(){
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == -1){
    perror("socket");
    exit(1);
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY; // Or specify server IP if needed
  socklen_t addr_len = sizeof(server_addr);
  Packet packets[TOTAL_PACKETS];

  // Initialize packets 
  for(int i = 0; i < TOTAL_PACKETS; i++){
    packets[i].seq_num = i;
    sprintf(packets[i].data, "Packet %d data", i);
  }
  int base = 0;
  int next_seq_num = 0;
  int ack;
  srand(time(NULL)); // Seed the random number generator
  
  while(base < TOTAL_PACKETS){
    while(next_seq_num < base + WINDOW_SIZE && next_seq_num < TOTAL_PACKETS){
      printf("Sending packet %d\n", next_seq_num);

      // Simulate packetloss (e.g., 20% success rate)
      if(rand() % 5 != 0){ // 80% success rate
        if(sendto(sock, &packets[next_seq_num], sizeof(Packet), 0, (struct sockaddr *)&server_addr, addr_len) == -1){
          perror("sendto");
          exit(1);
        }
      } else {
        printf("Simulated packet loss for packet %d\n", next_seq_num);
      }
      next_seq_num++;
    }
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    int retval = select(sock + 1, &fds, NULL, NULL, &timeout);

    if(retval == -1){
      perror("select");
      exit(1);
    } else if(retval == 0){
      printf("Timeout! Resending from packet %d\n", base);
      next_seq_num = base;
    } else {
      if(recvfrom(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&server_addr, &addr_len) == -1){
        perror("recvfrom");
        exit(1);
      }
      printf("Received ACK for packet %d\n", ack);
      if(ack >= base){
        base = ack + 1;
      } else {
        printf("Duplicase ACK received. Ignoring\n");
      }
    }
  }
  printf("Transmission complete.\n");
  close(sock);
  return 0;
}

