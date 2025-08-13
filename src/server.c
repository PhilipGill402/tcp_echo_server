#include "server.h"

#define MAX_MESSAGE_LENGTH 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main(){
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int client_fd;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0){
        perror("socket");
        return -1;
    } 
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    int ret = bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    
    if (ret < 0){
        perror("bind");
        close(server_fd); 
        return -1;
    }
    
    ret = listen(server_fd, 50);
    if (ret < 0){
        perror("listen");
        close(server_fd);
        return -1;
    }
   
    client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_address_len);
    if (client_fd < 0){
        perror("accept");
        close(server_fd);
        return -1;
    }

    while (1){
        char* buffer = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    
        int bytes_read = recv(client_fd, buffer, MAX_MESSAGE_LENGTH - 1, 0);
        if (bytes_read < 0){
            perror("recv");
            free(buffer);
            buffer = NULL;
            close(server_fd);
            close(client_fd);
            return -1;
        } else if (bytes_read == 0){
            free(buffer);
            buffer = NULL;
            break;
        }
        
        //strips the newline and adds a null terminator
        buffer[bytes_read - 1] = '\0';
        
        if (strcmp(buffer, "exit") == 0){
            free(buffer);
            buffer = NULL;
            break;
        }

        printf("Client: %s\n", buffer);
        
        //reappends the new line
        buffer[bytes_read - 1] = '\n';
        buffer[bytes_read] = '\0';
        
        int bytes_sent = send(client_fd, buffer, MAX_MESSAGE_LENGTH - 1, 0);
        if (bytes_sent < 0){
            perror("send");
            free(buffer);
            buffer = NULL;
            close(server_fd);
            close(client_fd);
            return -1;
        }
        
        //strips the newline and adds a null terminator
        buffer[bytes_read - 1] = '\0';

        printf("Server: %s\n", buffer);

        free(buffer);
        buffer = NULL;
    }
    
    printf("Client disconnected\n");
    close(client_fd);
    close(server_fd);
    return 0;
}
