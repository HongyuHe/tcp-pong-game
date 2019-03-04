#include <iostream>
#include "Client.h"
#include "Server.h"

int main(int nargs, char **argv)
{
//    std::cout << "Computer Networks Chat Client Starting..." << std::endl;
    Application* app = new Server();

//    std::cout << "Setting up" << std::endl;
    app->setup();

//    std::cout << "Running application" << std::endl;
    app->run();

    delete app;
    return 0;
}
