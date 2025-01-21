#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define ERROR_C "CLIENT ERROR: "
#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT 1602
#define BUFFER_SIZE 1024
#define SERVER_CLOSE_CONNECTION 'q'

bool client_close_connection(const char *c);

int main(int argv, const char* argc[]) {
    int client;

    struct sockaddr_in server_address;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client < 0) {
        std::cout << ERROR_C << "establishing socket error\n";
        exit(0);
    }

    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    std::cout << "\n ==> Client socket has been succesfully created!\n";

    int ret = connect(client,
        reinterpret_cast<struct sockaddr*>(&server_address),
        sizeof(server_address));

    if(ret == 0) {
        std::cout << "Connection to server... "
                  << inet_ntoa(server_address.sin_addr)
                  << " with port number: "
                  << DEFAULT_PORT << std::endl;
    }

    char buffer[BUFFER_SIZE];
    std::cout << "==> Waiting for server confirmation...\n";
    recv(client, buffer, BUFFER_SIZE, 0);
    std::cout << "==> Connection established!\n"
              << "Enter '" << SERVER_CLOSE_CONNECTION
              << "' to end the connection.\n";

    while (true) {
        std::cout << "Client: ";
        std::cin.getline(buffer, BUFFER_SIZE);
        send(client, buffer, BUFFER_SIZE, 0);
        if(client_close_connection(buffer)) {
            break;
        }

        std::cout << "Server: ";
        recv(client, buffer, BUFFER_SIZE, 0);
        std::cout << buffer;

        if(client_close_connection(buffer)) {
            break;
        }
        std::cout << std::endl;
    }
    close(client);
    std::cout << "Client closed the connection by entering '"
              << SERVER_CLOSE_CONNECTION << "' symbol" << std::endl;


    return 0;
}

bool client_close_connection(const char *c) {
    for(int i = 0; i < strlen(c); ++i) {
        if(c[i] == SERVER_CLOSE_CONNECTION) {
            return true;
        }
    }
    return false;
}
