//
// Created by Jesse on 2019-01-09.
//

#ifndef CPP_CHAT_CLIENT_CLIENT_H
#define CPP_CHAT_CLIENT_CLIENT_H

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

enum class LoginStatus {
    BUSY,
    IN_USE,
    SUCCESS,
    FAILE
};

struct Message {
    char stream_in[MSG_LEN];
    char stream_out[MSG_LEN];
};

class Client : public Application {
private:
    int severaddlen_ = 0;
    const struct sockaddr *serveraddr;
    struct Message message_;
    LoginStatus login_status_;
    char pre_buffer_[MSG_LEN] = {0};

    bool FirstHandShake();
    void SecondHandShake();

    SOCKET sock;
    std::thread socketThread, stdinThread;
    CircularLineBuffer socketBuffer, stdinBuffer;

    void tick() override;
    int readFromStdin();
    int readFromSocket();

    void createSocketAndLogIn();
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
        socketThread = std::thread(&Client::threadReadFromSocket, this);
        stdinThread = std::thread(&Client::threadReadFromStdin, this);
    }

    inline void stopThreads() {
        this->stopApplication();
        socketThread.join();
        stdinThread.join();
    }

public:
    inline ~Client() override {
        closeSocket();
        stopThreads();
    }

    inline void setup() override {
        login_status_ = LoginStatus::FAILE;
        createSocketAndLogIn();
    }
};


#endif //CPP_CHAT_CLIENT_CLIENT_H
