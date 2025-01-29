#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/_select.h>
#include <sys/_types/_fd_def.h>
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
    int client_socket;

    struct sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket < 0) {
        std::cerr << ERROR_C << "establishing socket error\n";
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    std::cout << "\n ==> Client socket has been succesfully created!\n";

    // int ret = connect(client_socket,
    //     reinterpret_cast<struct sockaddr*>(&server_address),
    //     sizeof(server_address));

    // if(ret == 0) {
    //     std::cout << "Connection to server... "
    //               << inet_ntoa(server_address.sin_addr)
    //               << " with port number: "
    //               << DEFAULT_PORT << std::endl;
    // }

    if(connect(client_socket, (struct sockaddr*)&server_address,
        sizeof(server_address)) < 0) {
            std::cerr << ERROR_C << "Connection failed...\n";
            close(client_socket);
            exit(1);
        }

    std::cout << "Connected to the server.\n";

    char buffer[BUFFER_SIZE];
    fd_set fd_set_client;

    while (true) {
        FD_ZERO(&fd_set_client);
        FD_SET(STDIN_FILENO, &fd_set_client);
        FD_SET(client_socket, &fd_set_client);

        int activity = select(client_socket + 1, &fd_set_client, NULL, NULL, NULL);
        if(activity < 0) {
            std::cerr << ERROR_C << "Select error\n";
            break;
        }

        if(FD_ISSET(client_socket, &fd_set_client)) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_recieved = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if(bytes_recieved <= 0) {
                std::cout << "Server disconnected\n";
                break;
            }
            std::cout << "Message from another client: " << buffer << std::endl;
        }

        if(FD_ISSET(STDIN_FILENO, &fd_set_client)) {
            std::cin.getline(buffer, BUFFER_SIZE);
            send(client_socket, buffer, strlen(buffer), 0);
        }
    }
    close(client_socket);
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
