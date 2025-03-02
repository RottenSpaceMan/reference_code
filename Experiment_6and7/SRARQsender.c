#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>

#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10
#define LOSS_PROBABILITY 0.2
#define PORT 8080
#define TIMEOUT 2

int sockfd;
struct sockaddr_in receiver_addr;
socklen_t addr_len;

bool send_frame(int seq_num){
    if((float)rand()/RAND_MAX < LOSS_PROBABILITY){
        printf("Frame %d lost during transmission.\n", seq_num);
        return false;
    }
    printf("Frame %d sent.\n", seq_num);

    char buffer[10];
    sprintf(buffer, "%d", seq_num);
    sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&receiver_addr, addr_len);
    return true;
}

bool receive_ack(int seq_num){
    char ack_buffer[10];
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    int ret = select(sockfd+1, &readfds, NULL, NULL, &tv);
    if(ret == 0){
        printf("Timeout for ACK %d, retransmitting...\n", seq_num);
        return false;
    } else if(ret < 0){
        perror("select() failed");
        return false;
    }

    recvfrom(sockfd, ack_buffer, sizeof(ack_buffer), 0, (struct sockaddr *)&receiver_addr, &addr_len);
    int ack_num = atoi(ack_buffer);
    if(ack_num == seq_num){
        printf("ACK %d received.\n", ack_num);
        return true;
    }
    return false;
}

void simulate(){
    int base = 0, next_seq_num = 0;
    while(base < TOTAL_FRAMES){
        // Send frames within window size
        while(next_seq_num < base + WINDOW_SIZE && next_seq_num < TOTAL_FRAMES){
            if(send_frame(next_seq_num)){
                next_seq_num++;
            }
        }
        sleep(1); // Simulate waiting time between frame transmission

        // Handle acknowledgments
        for(int i = base; i < next_seq_num; i++){
            if(receive_ack(i)){
                base++;
            } else {
                send_frame(i);
            }
        }
    }
}

int main(){
    srand(time(0));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;
    addr_len = sizeof(receiver_addr);

    simulate();

    close(sockfd);
    return 0;
}