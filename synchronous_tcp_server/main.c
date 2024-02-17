#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>

#define SERVER_PORT 32223
#define CLIENT_QUEUE_SIZE 5
#define CONNECTION_BUFFER_SIZE 4 << 10

int main()
{
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_address.sin_port = htons(SERVER_PORT); 

    int server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_socket < 0) {
        fprintf(stderr, "failed to create server socket\n");
        return EXIT_FAILURE;
    }

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "failed to bind server socket\n");
        return EXIT_FAILURE;
    }

    if (listen(server_socket, CLIENT_QUEUE_SIZE) < 0) { 
        fprintf(stderr, "failed to listen server socket\n");
        return EXIT_FAILURE;
    }

    while (1) {
        struct sockaddr_in client_address;
        int address_size = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t*)&address_size);
        if (client_socket < 0) {
            fprintf(stderr, "failed to accept client socket\n");
            continue;           
        }

        char buffer[CONNECTION_BUFFER_SIZE]; 

        while (1) {     
            int read_bytes = read(client_socket, buffer, sizeof(buffer));
            if (read_bytes < 0) {
                fprintf(stderr, "failed to read from client socket\n");
                break;
            }

            const char* response = "Hello from TCP server\n";
            int written_bytes = write(client_socket, response, strlen(response));
            if (written_bytes < 0) {
                fprintf(stderr, "failed to read from client socket\n");
                break;
            }            
        } 
    }

    return EXIT_SUCCESS;
}