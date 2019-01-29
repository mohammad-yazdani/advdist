//
// Created by Mohammad Yazdani on 2019-01-27.
//

#include <iostream>
#include <net/Socket.h>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <time/Benchmark.h>

void
getRTT(Benchmark * bench)
{
    std::cout << "== RTT TEST ===" << std::endl;
    auto event1 = Event("RTT");
    auto serverSocket = Socket(8080);
    serverSocket.waitForConn();
    serverSocket.sendACK('!');
    serverSocket.readACK('#');
    auto event2 = Event("END");

    bench->addInterval(event2.timeSince(&event1), event1.getDescription());
}

void
largeFile(Benchmark * bench)
{
    std::cout << "== LARGE FILE TEST ===" << std::endl;

    size_t bigSize = 0;
    auto serverSocket = Socket(3000);
    serverSocket.waitForConn();

    char * sizeBuf[4] = {nullptr};

    auto event1 = Event("READ SIZE OF LARGE FILE");
    serverSocket.readBuf(reinterpret_cast<char *>(sizeBuf), 8);

    bigSize = *((size_t *)sizeBuf);
    if (bigSize <= 0) {
        perror("Size of incoming file could not be read.");
        exit(1);
    }

    auto event2 = Event("CONFIRM FILE SIZE");
    serverSocket.sendACK('!');

    char * bigBuf = (char *)malloc(sizeof(char) * bigSize);

    auto event3 = Event("READ LARGE FILE");
    auto read = serverSocket.readBuf(bigBuf, bigSize);
    if (read <= 0) exit(1);

    bigSize -= read;
    while (bigSize) {
        read = serverSocket.readBuf(bigBuf, bigSize);
        bigSize -= read;
    }

    char ack = '!';
    auto event4 = Event("SEND ACK FOR LARGE FILE");
    serverSocket.writeBuf(&ack, 1);

    auto event5 = Event("END");

    bench->addInterval(event2.timeSince(&event1), event1.getDescription());
    bench->addInterval(event3.timeSince(&event2), event2.getDescription());
    bench->addInterval(event4.timeSince(&event3), event3.getDescription());
    bench->addInterval(event5.timeSince(&event4), event4.getDescription());
}

void
experiment_1()
{
    auto bench = new Benchmark();
    for (int i = 0; i < 100; i++) {
        std::cout << "ITERATION " << i << std::endl;
        getRTT(bench);
    }
    delete bench;
}

int
main()
{
    std::cout << "CS 798 > P1" << std::endl;

    auto bench = new Benchmark();

    experiment_1();

    delete bench;
}