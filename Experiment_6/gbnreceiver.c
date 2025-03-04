#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_PACKET_SIZE 1024
#define PORT 8080
#define TOTAL_PACKETS 10 // Adjust if you know the total number of packets, or just use a large number

typedef struct {
  int seq_num;
  char data[MAX_PACKET_SIZE];
} Packet;

int main(){
  int sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  Packet received_packet;
  int expected_seq_num = 0;
  int received[TOTAL_PACKETS] = {0}; // Track received packets
  
  // Create socket
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    perror("socket");
    exit(1);
  }

  // Set up server address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind socket to address
  if(bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
    perror("bind");
    close(sock);
    exit(1);
  }

  printf("Server is listening on port %d...\n", PORT);
  
  while(1){
    // Receive packet
    if (recvfrom(sock, &received_packet, sizeof(Packet), 0, (struct sockaddr*)&client_addr, &client_addr_len) == -1){
      perror("recvfrom");
      close(sock);
      exit(1);
    }
    printf("Received packet %d: %s\n", received_packet.seq_num, received_packet.data);

    // If this is the expected packet, acknowledge it 
    if(received_packet.seq_num == expected_seq_num){
      int ack = received_packet.seq_num;
      if(sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1){
        perror("sendto");
        close(sock);
        exit(1);
      }
      printf("Sent ACK for packet %d\n", ack);
      received[expected_seq_num] = 1; // Mark as received
      expected_seq_num++;

      //Check for and ACK any subsequent packets
      while(expected_seq_num < TOTAL_PACKETS && received[expected_seq_num]){
        ack = expected_seq_num;
        if (sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1){
          perror("sendto");
          close(sock);
          exit(1);
        }
        printf("Sent ack for packet %d\n",ack);
        expected_seq_num++;
      }
    } else if(received_packet.seq_num < expected_seq_num){// Duplicate packet
      int ack = received_packet.seq_num;
      if(sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1){
        perror("sendto");
        close(sock);
        exit(1);
      }
      printf("Sent duplicate ACK for packet %d\n",ack);
    } else {// Out of order packet
      int ack = expected_seq_num - 1;
      if (ack >= 0){
        if(sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1){
          perror("sendto");
          close(sock);
          exit(1);
        }
        printf("Sent ACK for packet %d (Expecting %d)\n", ack, expected_seq_num); // ACK for the last in-order packet
      }
    }
  }
  close(sock);
  return 0;
}
