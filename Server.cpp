//
// Created by Ray on 3/4/2019.
//
#include "Server.h"
#include "vusocket.h"
#include <io.h>
#include <cstring>
#include <iostream>

void Server::createSocket() {
//    int opt = 1;
    sock_init();

    sock = socket (AF_INET, SOCK_STREAM, 0);
    maxfd = sock;
    sub_max = -1;

    for (int i = 0; i < FD_SETSIZE; i++) {
        clients_sock[i] = NONE_CLINET;
    }

    //初始化；
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
    cout << "Server start...\n";

    while(true) {
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

            FD_SET(connfd, &all_set);

            if (connfd > maxfd) {
                maxfd = (SOCKET) connfd;
            }
            if (i > sub_max) {
                sub_max = i;
            }

            if (0 == --num_ready)  //第一次进入函数进行连接accept后就退出（第一次调用select的时候nready为1）；
            {
                continue;
            }
        }

        /*Data_stream*/
        int sockfd;
        for (int i = 0; i <= sub_max; i++) {    /* check all clients for data */
            if ((sockfd = clients_sock[i]) < 0)
                continue;

            memset(packet_.stream_in, 0, MSG_LEN);
            if (FD_ISSET(sockfd, &read_set)) {
                if (recv(sockfd, packet_.stream_in, MSG_LEN, 0) == 0) {
                    /* connection closed by client */
//                Online_list(NULL, sockfd, fp_registry, NULL, DELETE);        //从在线用户列表删去；
                    close(sockfd);
                    FD_CLR(sockfd, &all_set);
                    cout << "Close: " << sockfd << endl;
//                client[i] = -1;
//                stream.cmd[i] = 0;        //还原对应命令号；

                } else {
                    if (strlen(packet_.stream_in) != 0) {
                        cout << "Client:" << packet_.stream_in << endl;
                    }
                }
            }
        }
    }
//    struct sockaddr_in svr_addr;
//    int ret;
//    socklen_t addrlen = sizeof(struct sockaddr_in);
//
//    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//        perror("socket");
//        exit(EXIT_FAILURE);
//    }
//
//    //sendto()函数需要指定目的端口/地址
//    svr_addr.sin_family = AF_INET;
//    svr_addr.sin_port = htons(5382);
//    svr_addr.sin_addr.s_addr = inet_addr("52.58.97.202");
//
////    int tmp = 10;
////    while (tmp > 0) {
//    int send_num = sendto(sock, buf, 16, 0, (struct sockaddr* )&svr_addr, addrlen);
//    cout << "Send: " << send_num << endl;
////        tmp--;
////    }
//
//    ret = recvfrom(sock, packet_.stream_in, 16, 0, (struct sockaddr* )&svr_addr, &addrlen);
//    if (ret == -1)
//        fprintf(stderr, "error in Recv: %d %s\n", ret, gai_strerror(ret));
//    cout << "Ret:" << ret << endl;
//    cout << "Recv:" << packet_.stream_in << endl;
}

void Server::tick() {
//    read_set = all_set;
//    int num_ready = select(maxfd + 1, &read_set, (fd_set *) nullptr, (fd_set *) nullptr, (struct timeval *) nullptr);
//    /*Connection*/
//    if (FD_ISSET(sock, &read_set)) {
//        int cliaddr_len = sizeof(cliaddr);
//        int connfd = accept(sock, (struct sockaddr *) &cliaddr, &cliaddr_len);
//
//        cout << "Accept: " << connfd << endl;
////        printf("———> 成功与地址为：%s、端口为：%d的客户端建立连接￥～￥\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));
//        int i;
//        for (i = 0; i < FD_SETSIZE; i++) {
//            if (clients_sock[i] < 0) {
//                clients_sock[i] = connfd;
//                break;
//            }
//        }
//        if (i == FD_SETSIZE) {
//            fputs("too many clients\n", stderr);
//            exit(1);
//        }
//
//        FD_SET(connfd, &all_set);
//
//        if (connfd > maxfd) {
//            maxfd = (SOCKET) connfd;
//        }
//        if (i > sub_max) {
//            sub_max = i;
//        }
//
//        if (0 == --num_ready)  //第一次进入函数进行连接accept后就退出（第一次调用select的时候nready为1）；
//        {
//            return;
//        }
//    }
//
//    /*Data_stream*/
//    int sockfd;
//    for (int i = 0; i <= sub_max; i++) {    /* check all clients for data */
//        if ((sockfd = clients_sock[i]) < 0)
//            continue;
//
//        memset(packet_.stream_in, 0, MSG_LEN);
//        if (FD_ISSET(sockfd, &read_set)) {
//            if (read(sockfd, packet_.stream_in, MSG_LEN) == 0) {
//                /* connection closed by client */
////                Online_list(NULL, sockfd, fp_registry, NULL, DELETE);        //从在线用户列表删去；
//                close(sockfd);
//                FD_CLR(sockfd, &all_set);
////                client[i] = -1;
////                stream.cmd[i] = 0;        //还原对应命令号；
//
//            } else {
//                cout << "Client:" << packet_.stream_in << endl;
//            }
//        }
//    }
}

void Server::closeSocket() {
    sock_close(sock);
}
