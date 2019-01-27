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
    std::cout << "CS 798 > P1 > Q1 | Client" << std::endl;

    auto clientSocket = Socket("127.0.0.1", 8080);
    auto res = clientSocket.readBuf();
    std::cout << "Server said: " << res << std::endl;
    clientSocket.writeBuf("#");

    return 0;
}