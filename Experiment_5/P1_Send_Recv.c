#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>

// Define constans
#define PORT 8080
#define MAX_FRAME 5
#define TIMEOUT 2

// Sender function
void sender(){
    int sockfd;
    struct sockaddr_in receiver_addr;
    socklen_t addr_len = sizeof(receiver_addr);
    char buffer[1024];
    int frame_number = 0;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;

    // Set timeout for receiving ACKs
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Send frames one by one and wait for ACK
    while(frame_number < MAX_FRAME){
        sprintf(buffer, "Frame %d", frame_number);
        printf("Sending: %s\n", buffer);
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&receiver_addr, addr_len);

        // Wait for ACK
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&receiver_addr, &addr_len);
        if(n > 0){
            buffer[n] = '\0';
            printf("Received: %s\n", buffer);
            frame_number++;
        }
        else{
            printf("Timeout! Resending frame ...\n");
        }
    }
    close(sockfd);
}

// Receiver function
void receiver(){
    int sockfd;
    struct sockaddr_in receiver_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    char buffer[1024];
    int expected_frame = 0;

    // Create and bind socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(PORT);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0){
        perror("Bind failed");
        close(sockfd);
    }
    srand(time(0)); // Seed for random ACK loss simulation

    //Receive frames and send ACKs
    while (expected_frame < MAX_FRAME)
    {
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);
        if(n > 0){
            buffer[n] = '\0';
            printf("Received: %s\n",buffer);

            // Simulate ACK loss randomly
            if((rand() % 10) < 2){
                printf("Simulating ACK loss\n");
                continue;
            }

            // Send ACK for received frame
            sprintf(buffer, "ACK %d", expected_frame);
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &sender_addr, addr_len);
            expected_frame++;
        }
    }
    close(sockfd);
}

// Main function to run sender or receiver based on argument
int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s sender | receiver\n", argv[0]);
        return 1;
    }
    if(strcmp(argv[1], "sender") == 0){
        sender();
    }
    else if(strcmp(argv[1], "receiver") == 0){
        receiver();
    }
    else{
        printf("Invalid argument! Use 'sender' or 'receiver'.\n");
    }
    return 0;
}