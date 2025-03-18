#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465
#define SENDER "asender@gmail.com"
#define PASSWORD "asender!123!"
#define RECEIVER "areceiver@gmail.com"

void send_smtp_command(SSL *ssl,const char *cmd) {
  SSL_write(ssl, cmd, strlen(cmd));
  char buffer[1024] = {0};
  SSL_read(ssl, buffer, sizeof(buffer) - 1);
  printf("%s\n", buffer);
}

int main(){
  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[1024];
  SSL_CTX *ctx;
  SSL *ssl;

  // Initialize openssl
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
  ctx = SSL_CTX_new(SSLv23_client_method());

  if(!ctx){
    printf("SSL context create failed");
    return 1;
  }

  // Create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0){
    printf("Socket creation failed");
    return 1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SMTP_PORT);
  server_addr.sin_addr.s_addr = inet_addr(SMTP_SERVER);
  inet_pton(AF_INET, "74.125.24.108", &server_addr.sin_addr); // Replace with actual Gmail SMTP IP
  
  // Connect to SMTP server
  if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
    printf("Connection to SMTP server failed");
    return 1;
  }

  // Create SSL connection 
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, sockfd);
  if(SSL_connect(ssl) != 1){
    ERR_print_errors_fp(stderr);
    return 1;
  }

  // Receive greeting message
  SSL_read(ssl, buffer, sizeof(buffer) - 1);
  printf("%s\n", buffer);

  // Send EHLO command
  send_smtp_command(ssl, "EHLO localhost\r\n");

  // Authenticate
  send_smtp_command(ssl, "AUTH LOGIN\r\n");
  send_smtp_command(ssl, "YXNlbmRlckBnbWFpbC5jb20=\r\n"); // Base64 of asender@gmail.com
  send_smtp_command(ssl, "YXNlbmRlciExMjMh\r\n"); // Base64 of asender!123!

  // Send MAIL FROM
  send_smtp_command(ssl, "MAIL FROM:<" SENDER ">\r\n");

  // Send RCPT TO
  send_smtp_command(ssl, "RCPT TO:<" RECEIVER ">\r\n");

  // Send DATA
  send_smtp_command(ssl, "DATA\r\n");
  send_smtp_command(ssl, "Subject: Test Email\r\n\r\nThis is a test email from C program.\r\n.\r\n");

  // Send QUIT
  send_smtp_command(ssl, "QUIT\r\n");

  // Cleanup
  SSL_shutdown(ssl);
  SSL_free(ssl);
  close(sockfd);
  SSL_CTX_free(ctx);
  return 0;
}
