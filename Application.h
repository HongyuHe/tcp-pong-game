//
// Created by Jesse on 2019-01-09.
//

#ifndef CPP_CHAT_CLIENT_APPLICATION_H
#define CPP_CHAT_CLIENT_APPLICATION_H


class Application {
private:
    bool stop = false;

    virtual void tick() = 0;

public:
    virtual ~Application() = default;

    virtual void setup() = 0;

    void run();

    void stopApplication();

    bool isStopped();
};


#endif //CPP_CHAT_CLIENT_APPLICATION_H
