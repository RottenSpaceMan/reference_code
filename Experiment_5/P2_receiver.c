#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main(){
    int server_fd, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int frame;
    char ack[] = "ACK";

    // Create socket
    if((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0){
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Receiver: Waiting for sender connection...\n");
    while(1){
        valread = recvfrom(server_fd, &frame, sizeof(frame), 0, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if(valread > 0){
            printf("Receiver: Frame %d received. Sending ACK...\n", frame);
            sendto(server_fd, ack, strlen(ack), 0, (struct sockaddr *)&address, addrlen);
        }
    }
    close(server_fd);
    return 0;
}