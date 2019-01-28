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

    auto clientSocket = Socket("18.221.69.86", 8080);
    clientSocket.waitRTT();

    return 0;
}