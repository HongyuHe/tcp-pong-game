cmake_minimum_required(VERSION 3.10.2)
project(cpp-chat-client)

set(CMAKE_CXX_STANDARD 17)

set(TARGET_NAME cpp-chat-client)
add_executable(${TARGET_NAME} main.cpp vusocket.h Client.cpp Client.h Application.cpp Application.h vusocket.cpp CircularLineBuffer.cpp CircularLineBuffer.h)
if(WIN32)
    target_link_libraries(${TARGET_NAME} Ws2_32)
else()
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${TARGET_NAME} Threads::Threads)
endif()

