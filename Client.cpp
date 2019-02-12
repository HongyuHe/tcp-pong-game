#include "Client.h"
#include "vusocket.h"
#include <io.h>
#include <iostream>

void Client::createSocketAndLogIn() {
    sock_init();

    // Connect to the server
    const char *server = "52.58.97.202";
    const char *server_port = "5378";
    struct addrinfo hints;
    struct addrinfo *server_list = NULL;

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(server, server_port, &hints, &server_list);
    if (rc != 0) {
        fprintf(stderr, "error in getaddrinfo: %d %s\n", rc, gai_strerror(rc));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(struct addrinfo* p = server_list; p != nullptr; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("socket");
            sock_error_code();
            continue;
        }
        if (connect((SOCKET)sock, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            sock_error_code();
            sock_close(sock);
            continue;
        }
        break; // if we get here, we must have connected successfully
    }
}

void Client::closeSocket() {
    sock_close(sock);
}

int Client::readFromStdin() {
    fgets(message_.stream_in, MSG_LEN, stdin);
//        fflush(stdin);
//    cout << message_.stream_in << endl;
    return 1;
}

void Client::tick() {
    char login_msg[] = "HELLO-FROM ";

    printf("%s\n", "Connect to Sever successfully! Start chatting...\n @@@>");

    cout << "Please enter your user name:";
    readFromStdin();
    strcat(login_msg, message_.stream_in);
    cout << login_msg << endl;
    int len = strlen(login_msg);

    int send_len = send(sock, login_msg, len, 0);
    if (send_len)
        cout << "Success!- " << send_len << endl;
    else
        cout << "Erro" << endl;

}