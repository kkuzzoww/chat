#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define ERROR_S "SERVER ERROR: "
#define DEFAULT_PORT 1602
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION 'q'

bool client_close_connection(const char *c);

void handle_clients(int client_socket_1, int client_socket_2);

int main(int argc, char const *argv[]) {
    int server_socket, client_socket_1, client_socket_2;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0) {
        std::cout << ERROR_S << "establishing socket error\n";
        exit(1);
    }

    std::cout << "SERVER: socket for server has been created\n";

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << ERROR_S << "binding error\n";
        close(server_socket);
        exit(1);
    }

    int size = sizeof(server_address);
    std::cout << "SERVER: " << "Listening clients...\n";
    listen(server_socket, 2); // konec tut

    // socklen_t addr_size = sizeof(server_address);
    // server = accept(client, (struct sockaddr*)&server_address, &addr_size);
    // if(server < 0) {
    //     std::cout << ERROR_S << "can't accept the client...\n";
    // }

    client_socket_1 = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
    if(client_socket_1 < 0) {
        std::cerr << ERROR_S << "accepting client 1 error\n";
        close(server_socket);
        exit(1);
    }
    std::cout << "SERVER: Client 1 Status - Connected.\n";

    client_socket_2 = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
    if(client_socket_2 < 0) {
        std::cerr << ERROR_S << "accepting client 2 error\n";
        close(client_socket_1);
        close(server_socket);
        exit(-1);
    }
    std::cout << "SERVER: Client 2 Status - Connected.\n";

    handle_clients(client_socket_1, client_socket_2);

    close(client_socket_1);
    close(client_socket_2);
    close(server_socket);

    return 0;
}

void handle_clients(int client_socket_1, int client_socket_2) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_recieved = recv(client_socket_1, buffer, BUFFER_SIZE, 0);
        if(bytes_recieved <= 0) {
            std::cout << "Client 1 disconnected\n";
            break;
        }

        std::cout << "Message from Client 1: " << buffer << std::endl;

        send(client_socket_2, buffer, bytes_recieved, 0);

        memset(buffer, 0, BUFFER_SIZE);
        bytes_recieved = recv(client_socket_2, buffer, BUFFER_SIZE, 0);
        if(bytes_recieved <= 0) {
            std::cout << "Client 2 disconnected\n";
            break;
        }

        std::cout << "Message from Client 2: " << buffer << std::endl;

        send(client_socket_1, buffer, bytes_recieved, 0);
    }
}

bool client_close_connection(const char *c) {
    for(int i = 0; i < strlen(c); ++i) {
        if(c[i] == CLIENT_CLOSE_CONNECTION) {
            return true;
        }
    }
    return false;
}
