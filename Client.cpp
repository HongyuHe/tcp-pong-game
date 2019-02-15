#include "Client.h"
#include "vusocket.h"
#include <io.h>
#include <iostream>

void Client::createSocketAndLogIn() {
    sock_init();

    // Connect to the server //
    const char *server = "52.58.97.202";
    const char *server_port = "5378";
    struct addrinfo hints;
    struct addrinfo *server_list = NULL;

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(server, server_port, &hints, &server_list);
    if (rc != 0) {
        fprintf(stderr, "error in getaddrinfo: %d %s\n", rc, gai_strerror(rc));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for (struct addrinfo *p = server_list; p != nullptr; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype,
                           p->ai_protocol)) == -1) {
            perror("socket");
            sock_error_code();
            continue;
        }
        if (connect((SOCKET) sock, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            sock_error_code();
            sock_close(sock);
            continue;
        }
        printf("%s\n", ">>> Connect to Sever successfully! Start chatting...\n");
        break; // if we get here, we must have connected successfully
    }

    /**
     * Log in using a unique name
     */
    while (login_status_ != LoginStatus::SUCCESS) {
        if (FirstHandShake()) {
            SecondHandShake();
            switch(login_status_) {
                case LoginStatus::BUSY:
                    cout << ">>> Server is busy now. Please try later!" << endl;
                    createSocketAndLogIn();
                    return;
                case LoginStatus::IN_USE:
                    cout << ">>> This username is already in use. Please pick another one!" << endl;
                    createSocketAndLogIn();
                    return;
                case LoginStatus::FAILE:
                    cout << ">>> Something went wrong. Please try again." << endl;
                    createSocketAndLogIn();
                    return;
                case LoginStatus::SUCCESS:
                    cout << ">>> Login successfully!" << endl;
                    startThreads();
                    return;
                default:
                    break;
            }
        }
    }
}

// First hand-shake message //
bool Client::FirstHandShake() {
    memset(&message_.stream_out, 0x00, sizeof(message_.stream_out));

    char login_msg[] = "HELLO-FROM ";

    cout << ">>> Please enter your user name:";
    fgets(message_.stream_out, MSG_LEN, stdin);
    strcat(login_msg, message_.stream_out);
//    cout << login_msg << endl;

    int len = strlen(login_msg);
    int send_len = send(sock, login_msg, len, 0);
    if (send_len) {
        cout << "##### Send Success! SIZE: " << send_len << endl;
        return true;
    }else {
        cout << "##### Send Error!" << endl;
        return false;
    }
}

void Client::SecondHandShake() {
    memset(&message_.stream_in, 0x00, sizeof(message_.stream_in));

    int recv_len = recv(sock, message_.stream_in, MSG_LEN, 0);

    if (recv_len != -1) {
        cout << "##### Read Success! SIZE: " << recv_len << endl;
        cout << "##### Server: " <<message_.stream_in << strlen(message_.stream_in)<< endl;

        if (!strncmp("IN-USE", message_.stream_in, 6))
            login_status_ = LoginStatus::IN_USE;
        else if (!strncmp("BUSY", message_.stream_in, 4))
            login_status_ = LoginStatus::BUSY;
        else
            login_status_ = LoginStatus::SUCCESS;

    }else {
        cout << "Read Error" << endl;
        fprintf(stderr, "error in read(): %d %s\n", recv_len, gai_strerror(recv_len));

        login_status_ = LoginStatus::FAILE;
    }
}

void Client::closeSocket() {
    sock_close(sock);
}

int Client::readFromStdin() {
    memset(&message_.stream_out, 0x00, sizeof(message_.stream_out));
    printf(">>> ");
    fgets(message_.stream_out, MSG_LEN, stdin);

    if (!strncmp("!quit", message_.stream_out, 5)) {
        closeSocket();
        cout << ">>> Client shut down!" << endl;
    }

    return 1;
}


int Client::readFromSocket() {
    return 1;
}

void Client::tick() {

}











