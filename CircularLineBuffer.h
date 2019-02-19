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
    std::mutex mtx;

    int start = 0;
    int count = 0;
    bool data_flag = false;
    static const int bufferSize = 2000; // 4096
    char buffer[bufferSize] = {0};

public:
    int freeSpace();    // 返回剩余空间

    bool isFull();

    bool isEmpty();

    bool hasLine(); // 判断是否有数据；

    int nextFreeIndex(); // 下一个写入的位置；

    int findNewline(); // 找到下一个message；

    bool writeChars(const char *chars, size_t nchars);  // 将数据写入buffer；

    std::string readLine(); // 读出一条message；
};


#endif //CPP_CHAT_CLIENT_CIRCULARBUFFER_H
