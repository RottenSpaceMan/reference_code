#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_FRAME 1

int main(){
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int frame = 0;

    // Create socket
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address to binary
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0){
        printf("Invalid address\n");
        return -1;
    }

    while(1){
        printf("Sender: Sending frame %d\n", frame);
        sendto(sock, &frame, sizeof(frame), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        printf("Sender: Waiting for ACK\n");

        socklen_t addr_len = sizeof(serv_addr);
        recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&serv_addr, &addr_len);
        if(strcmp(buffer, "ACK") == 0){
            printf("Sender: Receiver ACK. Sending next frame...\n");
            frame++;
        }
        else{
            printf("Sender: No ACK received, resending frame %d\n", frame);
        }
        sleep(1);
    }
    close(sock);
    return 0;
}