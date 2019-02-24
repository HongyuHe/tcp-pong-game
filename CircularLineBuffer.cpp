#include "CircularLineBuffer.h"
#include "vusocket.h"
#include <io.h>
#include <iostream>

using namespace std;

bool CircularLineBuffer::writeChars(const char *chars, size_t nchars) {
//    mtx.lock();

    if (isFull() || nchars > freeSpace()) {

        return false;

    } else {
        int next_free = nextFreeIndex();

        if (next_free + nchars > bufferSize) {
//            cout << "______Outside_____\n";
            // Handle overflow:
            auto first_sec = (size_t)(bufferSize - next_free);
            size_t second_sec = nchars - first_sec;

            strncpy(buffer+next_free, chars, first_sec);
            strncpy(buffer, chars+first_sec, second_sec); // Stuffing remain data;
        } else {
//            cout << "______Inside______\n";
            strncpy(buffer+next_free, chars, nchars);
        }

        if (mtx.try_lock()) {  // only increase if currently not locked:
            count += nchars;
            mtx.unlock();
        }
//        printf("[ Write Buffer ]: %s {len:%d}{start:%d}{count = %d}\n", buffer+start, strlen(buffer+start), start, count);
//        mtx.unlock();
        return true;
    }
}

string CircularLineBuffer::readLine() {
//    mtx.lock();
    string output = "";

    if (isEmpty() || !hasLine())
        return output;

    for (int i = start, counter = 1; counter <= count; i++, counter++) {
        i %= bufferSize;
        if (*(buffer+i) != '\n') {
            output += *(buffer + i);
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
//    mtx.unlock();
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

    if (isEmpty()) {
        dirty_flag_ = false;
        return -1;
    }

    for (int i = start, counter = 0; counter < count; i++, counter++) {
        i %= bufferSize;
        if (*(buffer + i) == '\n') {
            dirty_flag_ = true;
            return i;
        }
    }
    dirty_flag_ = false;
    return -1;  // Error
}

bool CircularLineBuffer::hasLine() {

    return findNewline() >= 0;
//    return dirty_flag_;
}