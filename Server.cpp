//
// Created by Ray on 3/4/2019.
//
#include "Server.h"
#include "vusocket.h"
#include <io.h>
#include <cstring>
#include <iostream>

void Server::createSocket() {
    sock_init();

    maxfd = sock;
    sub_max = -1;
    for (int i = 0; i < FD_SETSIZE; i++) {
        clients_sock[i] = NONE_CLINET;
    }
    string name = "hhyy\n";
    users_map_.insert(
            pair<string, int>(
                    name, 10
            )
    );

    //初始化；
    sock = socket (AF_INET, SOCK_STREAM, 0);

    memset (&servaddr, 0x00, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons (SERV_PORT);

//    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));	//客户端退出处理；
    if ((bind (sock, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    if ((listen(sock, CLIENT_MAX)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    FD_ZERO(&all_set);
    FD_SET(sock, &all_set);

//    startThreads();
    cout << "Server start...\n";

}

int Server::readFromSocket() {

//    /*Data_stream*/
//    int clientfd;
//    string name;
//    map<string, int>::iterator iter;
//
//    for (int i = 0; i <= sub_max; i++) {    /* check all clients for data */
//        if ((clientfd = clients_sock[i]) < 0)
//            continue;
//
//        memset(packet_.stream_in, 0x00, MSG_LEN);
//        if (FD_ISSET(clientfd, &read_set)) {
//
//            if (recv((SOCKET)clientfd, packet_.stream_in, MSG_LEN, 0) != 0) {
//                if (strlen(packet_.stream_in) > 1)
//                    printf("### [Client Packet]->%s %d\n", packet_.stream_in, strlen(packet_.stream_in));
//
//                if (!strncmp("HELLO-FROM", packet_.stream_in, 10)) {
//
////                    iter = users_map_.find(packet_.stream_in + 11);
//                    name = packet_.stream_in + 11;
//                    // 检查整个map：
//                    cout << "### Map: \n";
//                    for (auto it:users_map_) {
//                        cout << "@" << it.first << endl;
//                    }
//                    if (0 == users_map_.count(name)) {
//
////                        cout << "Count:" << users_map_.count(name) << endl;
//                        cout << "### User name:" << name << endl;
//
//                        users_map_.insert(
//                                pair<string, int>(
//                                        name, clientfd
//                                )
//                        );
//                        string msg = "HELLO "+name+"\n";
//                        send((SOCKET)clientfd, (const char*)msg.data(), strlen(msg.data()), 0);
//                    } else {
//                        send((SOCKET)clientfd, "IN-USE\n", 7, 0);
//                        cout << "IN-USE\n";
//                    }
//                } else if (!strncmp("SEND", packet_.stream_in, 4)) {
//                    string to_name;
//                    int i;
//                    for (i = 5; i < strlen(packet_.stream_in); i++) {
//                        if (packet_.stream_in[i] == ' ') break;
//                        to_name += packet_.stream_in[i];
//                    }
//                    to_name += '\n';
//
//                    string msg;
//                    string from_name;
//                    string send_msg = "DELIVERY ";
//                    i++;
//                    for (; i < strlen(packet_.stream_in); i++)
//                        msg += packet_.stream_in[i];
//
//                    iter = users_map_.find(to_name);
//                    if (iter != users_map_.end()) {
//                        cout << "### No."<< iter->second << " is online\n";
//
//                        for (auto it:users_map_) {
//                            if(it.second == clientfd)
//                                from_name = it.first;
//                        }
//                        from_name = from_name.substr(0, from_name.length()-1);    // 去掉\n;
//                        send_msg += from_name + " " + msg + "\n";
//                        cout << "### [Private]:" << send_msg;
//                        int to_fd = iter->second;
//                        send((SOCKET)to_fd, send_msg.data(), send_msg.length(), 0);
//                    } else {
//                        send((SOCKET)clientfd, "UNKNOWN\n", 8, 0);
//                        cout << "UNKNOWN\n";
//                    }
//
//                    cout << "### To:" << to_name << "### MSG:" << msg << endl;
//                } else if (!strncmp("WHO", packet_.stream_in, 3)) {
//                    string onlinelist = "WHO-OK ";
//                    for (auto it:users_map_) {
//                        onlinelist += it.first.substr(0, it.first.length()-1);
//                        onlinelist += ",";
//                    }
//                    onlinelist = onlinelist.substr(0, onlinelist.length()-1);
//                    onlinelist += "\n";
//
//                    send((SOCKET)clientfd, onlinelist.data(), onlinelist.length(), 0);
//                    cout << "Users: " << onlinelist << endl;
//                } else {
//                    send((SOCKET)clientfd, "BAD-RQST-BODY\n", 14, 0);
//                }
//
//            } else {
//                /* connection closed by client */
//                close(clientfd);
//                FD_CLR(clientfd, &all_set);
//                cout << "Close: " << clientfd << endl;
//            }
//        }
//    }

    return 0;
}

int Server::readFromStdin() {
    return 0;
}

void Server::tick() {
    read_set = all_set;
    int num_ready = select(maxfd + 1, &read_set, (fd_set *) nullptr, (fd_set *) nullptr, (struct timeval *) nullptr);
    if (num_ready < 0) {
        perror ("select");
        exit (EXIT_FAILURE);
    }
    /*Connection*/
    if (FD_ISSET(sock, &read_set)) {
        int cliaddr_len = sizeof(cliaddr);
        int connfd = accept(sock, (struct sockaddr *) &cliaddr, &cliaddr_len);

        cout << "Accept: " << connfd << endl;

        int i;
        for (i = 0; i < FD_SETSIZE; i++) {
            if (clients_sock[i] < 0) {
                clients_sock[i] = connfd;
                break;
            }
        }
        if (i == FD_SETSIZE) {
            fputs("too many clients\n", stderr);
            exit(1);
        }

        FD_SET(connfd, &all_set);   // Add new fd to sets;

        if (connfd > maxfd) {
            maxfd = (SOCKET) connfd;
        }
        if (i > sub_max) {
            sub_max = i;
        }

        if (0 == --num_ready)  //第一次进入函数进行连接accept后就退出（第一次调用select的时候nready为1）；
            return;
    }
    /*Data_stream*/
    int clientfd;
    string name;
    map<string, int>::iterator iter;

    for (int i = 0; i <= sub_max; i++) {    /* check all clients for data */
        if ((clientfd = clients_sock[i]) < 0)
            continue;

        memset(packet_.stream_in, 0x00, MSG_LEN);
        if (FD_ISSET(clientfd, &read_set)) {

            if (recv((SOCKET)clientfd, packet_.stream_in, MSG_LEN, 0) != 0) {
                if (strlen(packet_.stream_in) > 1)
                    printf("### [Client Packet]->%s %d\n", packet_.stream_in, strlen(packet_.stream_in));

                if (!strncmp("HELLO-FROM", packet_.stream_in, 10)) {

//                    iter = users_map_.find(packet_.stream_in + 11);
                    name = packet_.stream_in + 11;
                    // 检查整个map：
                    cout << "### Map: \n";
                    for (auto it:users_map_) {
                        cout << "@" << it.first << endl;
                    }
                    if (0 == users_map_.count(name)) {

//                        cout << "Count:" << users_map_.count(name) << endl;
                        cout << "### User name:" << name << endl;

                        users_map_.insert(
                                pair<string, int>(
                                        name, clientfd
                                )
                        );
                        string msg = "HELLO "+name+"\n";
                        send((SOCKET)clientfd, (const char*)msg.data(), strlen(msg.data()), 0);
                    } else {
                        send((SOCKET)clientfd, "IN-USE\n", 7, 0);
                        cout << "IN-USE\n";
                    }
                } else if (!strncmp("SEND", packet_.stream_in, 4)) {
                    string to_name;
                    int i;
                    for (i = 5; i < strlen(packet_.stream_in); i++) {
                        if (packet_.stream_in[i] == ' ') break;
                        to_name += packet_.stream_in[i];
                    }
                    to_name += '\n';

                    string msg;
                    string from_name;
                    string send_msg = "DELIVERY ";
                    i++;
                    for (; i < strlen(packet_.stream_in); i++)
                        msg += packet_.stream_in[i];

                    iter = users_map_.find(to_name);
                    if (iter != users_map_.end()) {
                        cout << "### No."<< iter->second << " is online\n";

                        for (auto it:users_map_) {
                            if(it.second == clientfd)
                                from_name = it.first;
                        }
                        from_name = from_name.substr(0, from_name.length()-1);    // 去掉\n;
                        send_msg += from_name + " " + msg + "\n";
                        cout << "### [Private]:" << send_msg;
                        int to_fd = iter->second;
                        send((SOCKET)to_fd, send_msg.data(), send_msg.length(), 0);
                    } else {
                        send((SOCKET)clientfd, "UNKNOWN\n", 8, 0);
                        cout << "UNKNOWN\n";
                    }

                    cout << "### To:" << to_name << "### MSG:" << msg << endl;
                } else if (!strncmp("WHO", packet_.stream_in, 3)) {
                    string onlinelist = "WHO-OK ";
                    for (auto it:users_map_) {
                        onlinelist += it.first.substr(0, it.first.length()-1);
                        onlinelist += ",";
                    }
                    onlinelist = onlinelist.substr(0, onlinelist.length()-1);
                    onlinelist += "\n";

                    send((SOCKET)clientfd, onlinelist.data(), onlinelist.length(), 0);
                    cout << "Users: " << onlinelist << endl;
                } else {
                    send((SOCKET)clientfd, "BAD-RQST-BODY\n", 14, 0);
                }

            } else {
                /* connection closed by client */
                close(clientfd);
                FD_CLR(clientfd, &all_set);
                cout << "Close: " << clientfd << endl;
            }
        }
    }
}

void Server::closeSocket() {
    sock_close(sock);
}
