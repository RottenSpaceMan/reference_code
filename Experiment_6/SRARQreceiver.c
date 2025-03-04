#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 10

int sockfd;
struct sockaddr_in sender_addr, receiver_addr;
socklen_t addr_len;

void receive_frame(){
    char buffer[BUFFER_SIZE];
    while(1){
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
        if(len < 0){
            perror("recvfrom() failed");
            continue;
        }
        int seq_num = atoi(buffer);
        printf("Frame %d received.\n", seq_num);

        // Send acknowledgment back
        sendto(sockfd, buffer, len, 0, (struct sockaddr *)&sender_addr, addr_len);
        printf("Receiver: ACK %d sent.\n", seq_num);
    }
}
int main(){
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = INADDR_ANY;
    receiver_addr.sin_port = htons(PORT);
    addr_len = sizeof(receiver_addr);

    if(bind(sockfd, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0){
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    printf("Receiver is running...\n");
    receive_frame();

    close(sockfd);
    return 0;
}