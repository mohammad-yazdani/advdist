//
// Created by Mohammad Yazdani on 2019-01-27.
//

#include <iostream>
#include <net/Socket.h>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>

void
getRTT()
{
    auto serverSocket = Socket(8080);
    serverSocket.waitForConn();
    auto rtt = serverSocket.askRTT();
    std::cout << "RTT: " << rtt << " nanoseconds | " << Event::toSeconds(rtt) << " seconds" << std::endl;
    for (auto event : serverSocket.getEvent_queue()) {
        std::cout << event.getStamp() << " ";
        std::cout << event.getStringType() << std::endl;
    }
}

int
main()
{
    std::cout << "CS 798 > P1 > Q1 | Server" << std::endl;
    //getRTT();

    size_t bigSize = 10490883;
    auto serverSocket = Socket(3000);
    serverSocket.waitForConn();
    std::cout << "Connection established to client" << std::endl;

    char * bigBuf = (char *)malloc(sizeof(char) * bigSize);

    auto res = serverSocket.readBuf(bigBuf, bigSize);
    if (res)
        return res;
    std::ofstream downloadedFile;
    downloadedFile.open("test.txt", std::fstream::out);
    downloadedFile << bigBuf;
    downloadedFile.close();

    char ack = '!';
    serverSocket.writeBuf(&ack, 1);
    return 0;
}