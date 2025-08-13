#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_LENGTH 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main(){
    struct sockaddr_in server_address;

    int server_fd= socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0){
        perror("socket");
        return -1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    int ret = connect(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    if (ret < 0){
        perror("connect");
        return -1;
    }
        
    printf("Connected to %s:%d\n", SERVER_IP, SERVER_PORT);
    while (1){
        char* buffer = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
        int n = 0;
        printf("Enter your message >> "); 
        fgets(buffer, MAX_MESSAGE_LENGTH, stdin); 
        
        if (strcmp(buffer, "exit\n") == 0){
            free(buffer);
            buffer = NULL;
            break;
        }

        int bytes_sent = send(server_fd, buffer, strlen(buffer), 0);
        if (bytes_sent < 0){
            free(buffer);
            buffer = NULL;
            close(server_fd);
            return 0;
        }
        printf("Client: %s", buffer); 

        free(buffer);
        buffer = NULL; 
        buffer = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);

        int bytes_read = recv(server_fd, buffer, MAX_MESSAGE_LENGTH, 0);

        printf("Server: %s", buffer);

        free(buffer);
        buffer = NULL;

    }

    close(server_fd);
    return 0;
}
