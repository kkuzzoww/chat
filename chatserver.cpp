#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define ERROR_S "SERVER ERROR: "
#define DEFAULT_PORT 1602
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION 'q'

bool client_close_connection(const char *c);

int main(int argc, char const *argv[]) {
    int client;
    int server;

    struct sockaddr_in server_address;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client < 0) {
        std::cout << ERROR_S << "establishing socket error\n";
        exit(0);
    }

    std::cout << "SERVER: socket for server has been created\n";

    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    int ret = bind(client, (struct sockaddr*)&server_address,
        sizeof(server_address));

    if(ret < 0) {
        std::cout
            << ERROR_S
            << "binding connection. Socket has already been established\n";
        return -1;
    }

    int size = sizeof(server_address);
    std::cout << "SERVER: " << "Listening clients...\n";
    listen(client, 1);

    socklen_t addr_size = sizeof(server_address);
    server = accept(client, (struct sockaddr*)&server_address, &addr_size);
    if(server < 0) {
        std::cout << ERROR_S << "can't accept the client...\n";
    }

    char buffer[BUFFER_SIZE];
    bool is_exit = false;
    while(server > 0) {
        strcpy(buffer, "==> Server connected!");
        send(server, buffer, BUFFER_SIZE, 0);
        std::cout <<  "==> Connected to the client #1!" << std::endl
            << "Enter " << CLIENT_CLOSE_CONNECTION <<  " to end the connection\n\n";


        std::cout << "Client #1: ";
        recv(server, buffer, BUFFER_SIZE, 0);
        std::cout << buffer << std::endl;

        if (client_close_connection(buffer)) {
            is_exit = true;
        }

        while(is_exit) {
            std::cout << "Server: ";
            std::cin.getline(buffer, BUFFER_SIZE);
            send(server, buffer, BUFFER_SIZE, 0);
            if(client_close_connection(buffer)) {
                break;
            }
            std::cout << "Client: ";
            recv(server, buffer, BUFFER_SIZE, 0);
            std::cout << buffer << std::endl;
            if(client_close_connection(buffer)) {

                break;
            }
        }
        std::cout << "Client closed the connection by entering '"
                << CLIENT_CLOSE_CONNECTION << "' symbol" << std::endl;

        is_exit = false;
        exit(1);

    }
    return 0;

}

bool client_close_connection(const char *c) {
    for(int i = 0; i < strlen(c); ++i) {
        if(c[i] == CLIENT_CLOSE_CONNECTION) {
            return true;
        }
    }
    return false;
}
