#include "Application.h"
//#include "Client.h"
#include <iostream>
#include <stdio.h>

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
