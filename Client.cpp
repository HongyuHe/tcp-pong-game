#include "Client.h"
#include "vusocket.h"
#include <io.h>
#include <iostream>

SOCKET sockfd_g;

void Client::createSocketAndLogIn() {
    // Connect to the server
    int rc = 1;
    const char *server = "52.58.97.202";
    const char *server_port = "5378";
    struct addrinfo hints;
    struct addrinfo *server_list = NULL;

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    rc = getaddrinfo(server, server_port, &hints, &server_list);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(struct addrinfo* p = server_list; p != nullptr; p = p->ai_next) {
        if ((sockfd_g = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect((SOCKET)sockfd_g, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(sockfd_g);
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

}

void Client::closeSocket() {
    close(sockfd_g);
}