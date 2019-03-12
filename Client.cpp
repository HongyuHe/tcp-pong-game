#include "Client.h"
#include "vusocket.h"
#include <io.h>
#include <cstring>
#include <iostream>

int counter = 0;

void Client::createSocketAndLogIn() {
    sock_init();

    // Connect to the server //
    const char *server = "52.58.97.202";
    const char *server_port = "2357";
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

    }
    startThreads();

//    while (login_status_ != LoginStatus::SUCCESS) {
//        if (FirstHandShake()) {
//            SecondHandShake();
//            switch(login_status_) {
//                case LoginStatus::BUSY:
//                    cout << ">>> Server is busy now. Please try later!" << endl;
//                    createSocketAndLogIn();
//                    return;
//                case LoginStatus::IN_USE:
//                    cout << ">>> This username is already in use. Please pick another one!" << endl;
//                    createSocketAndLogIn();
//                    return;
//                case LoginStatus::FAILE:
//                    cout << ">>> Something went wrong. Please try again." << endl;
//                    createSocketAndLogIn();
//                    return;
//                case LoginStatus::SUCCESS:
//                    cout << ">>> Login successfully!" << endl;
//                    startThreads();
//                    return;
//                default:
//                    break; // if we get here, we must have connected successfully
//                }
//            }
//        }
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
    char report_output[] = "REPORT botid=60d47fdf01e71d29 os=windows <END>";
    char version_update_output[] = "UPDATE version=1.33.7 <END>";
    char command_output[] = "COMMAND <END>";
    char done_output[] = "DONE <END>";
//    char tmp_str;

    switch (send_step_) {
        case 0:
            int send_len = send(sock, report_output, strlen(report_output), 0);
            if (send_len > 0) {
                cout << "##### Send_0 Success! SIZE: " << send_len << endl;
                recv_step_++;

            } else {
                cout << "##### Send_0 Error!" << endl;
            }
            break;
        case 1:
            send_len = send(sock, version_update_output, strlen(version_update_output), 0);
            if (send_len > 0) {
                cout << "##### Send_1 Success! SIZE: " << send_len << endl;
                recv_step_++;

            } else {
                cout << "##### Send_1 Error!" << endl;
            }
            break;
        case 2:
            send_len = send(sock, command_output, strlen(command_output), 0);
            if (send_len > 0) {
                cout << "##### Send_2 Success! SIZE: " << send_len << endl;
                recv_step_++;

            } else {
                cout << "##### Send_2 Error!" << endl;
            }
            break;
        case 3:
            send_len = send(sock, done_output, strlen(done_output), 0);
            if (send_len > 0) {
                cout << "##### Send_3 Success! SIZE: " << send_len << endl;
                recv_step_++;

            } else {
                cout << "##### Send_3 Error!" << endl;
            }
            break;
        default:;
    }

    memset(&message_.stream_in, 0x00, sizeof(message_.stream_in));
    int recv_len = recv(sock, message_.stream_in, MSG_LEN, 0);
    switch (recv_step_) {
        case 0:
            if (!strncmp("HELLO", message_.stream_in, 5)) {
                cout <<". ";
                send_step_++;
            }
            break;

    }

}


int Client::readFromSocket() {
//    memset(&message_.stream_in, 0x00, sizeof(message_.stream_in));
//
//    int recv_len = recv(sock, message_.stream_in, MSG_LEN, 0);
//
//    if (recv_len >= 0) {
//        //        cout << "##### Read Success! SIZE: " << recv_len << endl;
//        //        cout << "##### Server: " << message_.stream_in << strlen(message_.stream_in) << endl;
//
//        if (strlen(message_.stream_in) != 0) {
//            //            cout << "$$$" << message_.stream_in << "$$$";
//            socketBuffer.writeChars(message_.stream_in, strlen(message_.stream_in));
//        }
//
//        //        if (!strncmp("WHO-OK", message_.stream_in, 6)) {
//        //
//        //            cout << ">>>[Online users]: " << message_.stream_in + 6 << ">>>" << endl;
//        ////            printf(">>>[Online users]: %s<<<", message_.stream_in+6);
//        //        } else if (!strncmp("DELIVERY", message_.stream_in, 8)) {
//        //
//        //            cout << ">>>[Private Message] from @" << message_.stream_in+9 << ">>>" << endl;
//        ////            printf(">>>[Private Message] from @:%s {SIZE:%d}<<<", message_.stream_in, strlen(message_.stream_in));
//        //        }
//
//        return 1;
//    }
//
//    //     else if (recv_len == -1) {
//    //        cout << "Read Error" << endl;
//    //        fprintf(stderr, "error in read(): %d %s\n", recv_len, gai_strerror(recv_len));
//    //
//    //        return 0;
//    else {
//        return 0;
//    }
}



void Client::tick() {
    
//    string version_update_output = "UPDATE version=1.33.7 <END>";
//    string command_output = "COMMAND <END>";
//    string done_output = "DONE <END>";
//    string tmp_str;
//    int step = 0;
//
//    switch (step) {
//        case 0:
//            int send_len = send(sock, version_update_output, strlen(version_update_output), 0);
//            if (send_len > 0) {
//                 cout << "##### Send_1 Success! SIZE: " << send_len << endl;
//            } else {
//                 cout << "##### Send_1 Error!" << endl;
//            }
//
//    }

//    if (true) {
//
//        char send_msg[2000] = {0};
//
//        if (counter == 0) {
//            tmp_str = "REPORT botid=60d47fdf01e71d29 os=windows <END>";
//            counter++;
//        } else {
//            if (tmp_str.length() > 0) {
//                strcpy(send_msg, tmp_str.data());
//                strcat(send_msg, );
//            }
//            // printf("$$$$ Send $$$$\n %s\n", send_msg);
//
//            int len = strlen(send_msg);
//            if (len > 1) {
//                int send_len = send(sock, send_msg, len, 0);
//                if (send_len > 0) {
//                    // cout << "##### Send Success! SIZE: " << send_len << endl;
//                } else {
//                    // cout << "##### Send Error!" << endl;
//                }
//            }
//        }
//    }
//
//// !!! MODIFY .hasLine TO DETECT <END>
//    if (socketBuffer.hasLine()) {
//
//        char rcv_msg[2000] = {0};
//        tmp_str = socketBuffer.readLine();
//        strcpy(rcv_msg, tmp_str.data());
//
////        printf("\n %s\n", rcv_msg);
//
//        if (!strncmp("HELLO", rcv_msg, 5)) {
//            memset(&tmp_str, 0x00, tmp_str.length());
//            strcpy(tmp_str.data(), version_update_output.data());
//            cout << ". ";
//        }
//        else if (!strncmp("UPDATE", rcv_msg, 6)) {
//            memset(&tmp_str, 0x00, tmp_str.length());
//            strcpy(tmp_str.data(), command_output.data());
//            cout << ". ";
//        }
//        else if (!strncmp("COMMAND", rcv_msg, 7)) {
//            memset(&tmp_str, 0x00, tmp_str.length());
//            strcpy(tmp_str.data(), done_output.data());
//            cout << ". ";
//        }
//        else if (!strncmp("BYE", rcv_msg, 3)) {
//            closeSocket();
//            exit(0);
//        }
//    }
}



void Client::closeSocket() {
    sock_close(sock);
}









