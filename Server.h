//
// Created by Ray on 3/4/2019.
//

#ifndef CPP_CHAT_CLIENT_SERVER_H
#define CPP_CHAT_CLIENT_SERVER_H

#ifdef _WIN32
#else
#include <pthread.h>
#endif

#include <thread>
#include <string.h>
#include <iostream>
#include "Application.h"
#include "vusocket.h"
#include "CircularLineBuffer.h"

#define MSG_LEN 500
#define SERV_PORT 8000
#define CLIENT_MAX 50
#define NONE_CLINET -1000

struct Packet {
    char stream_in[MSG_LEN];
    char stream_out[MSG_LEN];
};

class Server: public Application {
private:
//    pthread_t ntid;
//    pthread_mutex_t clock_lock;
    struct sockaddr_in cliaddr;
    struct sockaddr_in servaddr;
    struct Packet packet_;
    fd_set all_set;
    fd_set read_set;
    SOCKET maxfd;
    int clients_sock[FD_SETSIZE];
    int sub_max;

    SOCKET sock;
    std::thread socketThread, stdinThread;
    CircularLineBuffer socketBuffer, stdinBuffer;

    void pthread_loop(int server_sockfd); // one loop one thread

    void tick() override;
    int readFromStdin();
    int readFromSocket();

    void createSocket();
    void closeSocket();

    inline void threadReadFromStdin() {
        while (!isStopped()) {
            auto res = readFromStdin();
            if (res < 0) {
                stopApplication();
            }
        }
    }

    inline void threadReadFromSocket() {
        while (!isStopped()) {
            auto res = readFromSocket();
            if (res < 0) {
                stopApplication();
            }
        }
    }

    inline void startThreads() {
        socketThread = std::thread(&Server::threadReadFromSocket, this);
        stdinThread = std::thread(&Server::threadReadFromStdin, this);
    }

    inline void stopThreads() {
        this->stopApplication();
        socketThread.join();
        stdinThread.join();
    }

public:
    inline ~Server() override {
        closeSocket();
        stopThreads();
    }

    inline void setup() override {
        createSocket();
    }
};


#endif //CPP_CHAT_CLIENT_SERVER_H
