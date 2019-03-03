#include "Client.h"
#include "vusocket.h"
#include <io.h>
#include <cstring>
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
//        printf("%s\n", ">>> Connect to Sever successfully! Start chatting...\n");
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

    cout << "\n>>> Please enter your user name:";
    fgets(message_.stream_out, MSG_LEN, stdin);
    strcat(login_msg, message_.stream_out);
//    cout << login_msg << endl;

    int len = strlen(login_msg);
    int send_len = send(sock, login_msg, len, 0);
    if (send_len) {
//        cout << "##### Send Success! SIZE: " << send_len << endl;
        return true;
    }else {
//        cout << "##### Send Error!" << endl;
        return false;
    }
}

void Client::SecondHandShake() {
    memset(&message_.stream_in, 0x00, sizeof(message_.stream_in));

    int recv_len = recv(sock, message_.stream_in, MSG_LEN, 0);

    if (recv_len != -1) {
//        cout << "##### Read Success! SIZE: " << recv_len << endl;
//        cout << "##### Server: " <<message_.stream_in << strlen(message_.stream_in)<< endl;

        if (!strncmp("IN-USE", message_.stream_in, 6))
            login_status_ = LoginStatus::IN_USE;
        else if (!strncmp("BUSY", message_.stream_in, 4))
            login_status_ = LoginStatus::BUSY;
        else if (!strncmp("BAD-RQST-BODY", message_.stream_in, 13))
            login_status_ = LoginStatus::FAILE;
        else
            login_status_ = LoginStatus::SUCCESS;

    }else {
        cout << "Read Error" << endl;
        fprintf(stderr, "error in read(): %d %s\n", recv_len, gai_strerror(recv_len));

        login_status_ = LoginStatus::FAILE;
    }
}

int Client::readFromStdin() {

    const char list_users_c[] = "WHO\n";
    const char send_msg_c[] = "SEND ";

    memset(&message_.stream_out, 0x00, sizeof(message_.stream_out));
    fgets(message_.stream_out, MSG_LEN, stdin);

    if (!strncmp("!quit", message_.stream_out, 5)) {
        cout << ">>>[Client shut down!]" << endl;
        closeSocket();

    } else if (!strncmp("!who", message_.stream_out, 4)) {
        memset(&message_.stream_out, 0x00, sizeof(message_.stream_out));
        strcpy(message_.stream_out, list_users_c);

    } else if (!strncmp("@", message_.stream_out, 1)) {
        strcat((char *)send_msg_c, message_.stream_out+1);
        memset(&message_.stream_out, 0x00, sizeof(message_.stream_out));
        strcpy(message_.stream_out, send_msg_c);

    } else {
        cout << ">>>[Bad Command! Please try again!]" << ">>>" << endl;
    }

    if (strlen(message_.stream_out) != 0) {
        stdinBuffer.writeChars(message_.stream_out, strlen(message_.stream_out));
    }

//    int len = strlen(message_.stream_out);
//    int send_len = send(sock, message_.stream_out, len, 0);
//    if (send_len) {
////        cout << "##### Send Success! SIZE: " << send_len << endl;
//        return 1;
//    }else {
////        cout << "##### Send Error!" << endl;
//        return 0;
//    }
    return 1;
}


int Client::readFromSocket() {
    memset(&message_.stream_in, 0x00, sizeof(message_.stream_in));

    int recv_len = recv(sock, message_.stream_in, MSG_LEN, 0);

    if (recv_len >= 0) {
//        cout << "##### Read Success! SIZE: " << recv_len << endl;
//        cout << "##### Server: " << message_.stream_in << strlen(message_.stream_in) << endl;

        if (strlen(message_.stream_in) != 0) {
//            cout << "$$$" << message_.stream_in << "$$$";
            socketBuffer.writeChars(message_.stream_in, strlen(message_.stream_in));
        }

//        if (!strncmp("WHO-OK", message_.stream_in, 6)) {
//
//            cout << ">>>[Online users]: " << message_.stream_in + 6 << ">>>" << endl;
////            printf(">>>[Online users]: %s<<<", message_.stream_in+6);
//        } else if (!strncmp("DELIVERY", message_.stream_in, 8)) {
//
//            cout << ">>>[Private Message] from @" << message_.stream_in+9 << ">>>" << endl;
////            printf(">>>[Private Message] from @:%s {SIZE:%d}<<<", message_.stream_in, strlen(message_.stream_in));
//        }

        return 1;
    }

//     else if (recv_len == -1) {
//        cout << "Read Error" << endl;
//        fprintf(stderr, "error in read(): %d %s\n", recv_len, gai_strerror(recv_len));
//
//        return 0;
    else {
        return 0;
    }
}

void Client::tick() {
    if (stdinBuffer.hasLine()) {
        char send_msg[2000] = {0};
        string tmp_str = stdinBuffer.readLine();
        if (tmp_str.length() > 0) {
            strcpy(send_msg, tmp_str.data());
            strcat(send_msg, "\n");
        }
//        printf("$$$$ Send $$$$\n %s\n", send_msg);

        int len = strlen(send_msg);
        if (len > 1) {
            int send_len = send(sock, send_msg, len, 0);
            if (send_len > 0) {
//                cout << "##### Send Success! SIZE: " << send_len << endl;
            } else {
//                cout << "##### Send Error!" << endl;
        }

    }

//        cout << "Send: " << send_len << endl;
    }

    if (socketBuffer.hasLine()) {
        char rcv_msg[2000] = {0};
        string tmp_str = socketBuffer.readLine();
        strcpy(rcv_msg, tmp_str.data());

//        printf("\n %s\n", rcv_msg);


        /*Output*/
        if (!strncmp("WHO-OK", rcv_msg, 6)) {

            cout << ">>>[Online users List]: "<< endl;
            for (int i = 6; i <= strlen(rcv_msg); i++) {
                if (*(rcv_msg+i) == ',') {
                    cout << '\n' << "       @";
                } else {
                    cout << *(rcv_msg+i);
                }
            }
            cout << '\n' << ">>> ";
//            printf(">>>[Online users]: %s<<<", message_.stream_in+6);
        } else if (!strncmp("DELIVERY", rcv_msg, 8)) {

            cout << ">>>[Private Message] from @" << rcv_msg+9 << '\n' << ">>>" << endl;
//            printf(">>>[Private Message] from @:%s {SIZE:%d}<<<", message_.stream_in, strlen(message_.stream_in));
        }

    }
}


void Client::closeSocket() {
    sock_close(sock);
}









