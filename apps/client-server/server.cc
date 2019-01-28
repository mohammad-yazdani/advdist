//
// Created by Mohammad Yazdani on 2019-01-27.
//

#include <iostream>
#include <net/Socket.h>
#include <string>
#include <thread>
#include <chrono>

int
main()
{
    std::cout << "CS 798 > P1 > Q1 | Server" << std::endl;

    auto serverSocket = Socket(8080);
    serverSocket.waitForConn();
    auto rtt = serverSocket.askRTT();
    std::cout << "RTT: " << rtt << " nanoseconds | " << Event::toSeconds(rtt) << " seconds" << std::endl;
    for (auto event : serverSocket.getEvent_queue()) {
        std::cout << event.getStamp() << " ";
        std::cout << event.getStringType() << std::endl;
    }

    return 0;
}