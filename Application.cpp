//
// Created by Jesse on 2019-01-09.
//

#include "Application.h"

void Application::run() {
    while (!this->stop) {
        this->tick();
    }
}

void Application::stopApplication() {
    this->stop = true;
}

bool Application::isStopped() {
    return this->stop;
}
