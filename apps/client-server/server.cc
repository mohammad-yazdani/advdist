//
// Created by Mohammad Yazdani on 2019-01-27.
//

#include <iostream>
#include <net/Socket.h>
#include <string>
#include <thread>

int
main()
{
    std::cout << "CS 798 > P1 > Q1 | Server" << std::endl;

    auto serverSocket = Socket(8080);
    serverSocket.waitForConn();
    serverSocket.writeBuf("!");
    auto res = serverSocket.readBuf();
    std::cout << "Client said: " << res << std::endl;
    for (auto event : serverSocket.getEvent_queue()) {
        std::cout << event.getEpoch() << " ";
        std::cout << event.getStringType() << std::endl;
    }

    return 0;
}