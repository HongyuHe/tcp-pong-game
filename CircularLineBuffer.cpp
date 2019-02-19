#include "CircularLineBuffer.h"
#include "vusocket.h"
#include <io.h>
#include <iostream>

using namespace std;

bool CircularLineBuffer::writeChars(const char *chars, size_t nchars) {

    if (nchars > freeSpace()) {
//        int cur = 0;
//
//        while (cur == nchars-1) {
//            int i = nextFreeIndex();
//            while (isFull()) {
//                buffer[i] = chars[cur];
//
//                if (mtx.try_lock()) {  // only increase if currently not locked:
//                    count++;
//                    mtx.unlock();
//                }
//                i = ++i % bufferSize;
//                cur++;
//            }
//        }
//        return true;
        return false;

    } else {
        int next_free = nextFreeIndex();

        if (next_free + nchars > bufferSize) {
            // Handle overflow:
            size_t first_sec = next_free + nchars - bufferSize;
            size_t second_sec = nchars - first_sec;

            strncpy(buffer+next_free, chars, first_sec);
            strncpy(buffer, chars+first_sec, second_sec); // Stuffing remain data;
        } else {
            strncpy(buffer+next_free, chars, nchars);
        }

        if (mtx.try_lock()) {  // only increase if currently not locked:
            count += nchars;
            mtx.unlock();
        }
//        printf("[ Write Buffer ]: %s {len:%d}{start:%d}{count = %d}\n", buffer+start, strlen(buffer+start), start, count);
        return true;
    }
}

string CircularLineBuffer::readLine() {
    string output = "";

    for (int i = start, counter = 1; ; i++, counter++) {
        if (*(buffer+i) != '\n' && counter != count) {
            output += *(buffer+i);
        } else {
            if (mtx.try_lock()) {
                start = i + 1;
                count -= counter;
                mtx.unlock();
            }
            break;
        }
    }
//    cout << "[ Read Buffer ]: " << output;
//    printf("{start = %d} {count = %d}\n", start, count);
    return output;
}

bool CircularLineBuffer::isEmpty() {
    if (count < 0) {
        cout << "!!! <0 " << endl;
        count = 0;
    }
    return count == 0;
}

bool CircularLineBuffer::isFull() {
    return bufferSize == count;
}

int CircularLineBuffer::freeSpace() {
    return bufferSize - count;
}

int CircularLineBuffer::nextFreeIndex() {
    return (start + count) % bufferSize;
}

int CircularLineBuffer::findNewline() {
    int tmp = 0;

    for (int i = start, counter = 0; counter < bufferSize; i++, counter++) {
        if (*(buffer + i % bufferSize) == '\n') {
            tmp++;
            if (tmp == 2)
                return i + 1;
        }
    }
    data_flag = false;
    return -1;  // Error
}

bool CircularLineBuffer::hasLine() {
    return true;
}