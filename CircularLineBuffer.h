//
// Created by Jesse on 2019-01-10.
//

#ifndef CPP_CHAT_CLIENT_CIRCULARBUFFER_H
#define CPP_CHAT_CLIENT_CIRCULARBUFFER_H


#include <cstdlib>
#include <string>
#include <mutex>

/**
 * Assignment 3
 *
 * See the lab manual for the assignment description.
 */
class CircularLineBuffer {
private:
    static const int bufferSize = 100; // 4096
    char buffer[bufferSize] = {0};
    std::mutex mtx;
    int start = 0, count = 0;
public:
    int freeSpace();

    bool isFull();

    bool isEmpty();

    int nextFreeIndex();

    int findNewline();

    bool hasLine();

    bool writeChars(const char *chars, size_t nchars);

    std::string readLine();
};


#endif //CPP_CHAT_CLIENT_CIRCULARBUFFER_H
