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
#include <vector>

void
getRTT(Benchmark * bench, unsigned int port)
{
    auto event1 = Event("RTT");
    auto serverSocket = Socket(port);
    serverSocket.waitForConn();
    serverSocket.sendACK('!');
    serverSocket.readACK('#');
    auto event2 = Event("END");

    bench->addInterval(event2.timeSince(&event1), event1.getDescription());
}

void
largeFile(unsigned int port, Benchmark * bench)
{
    size_t bigSize = 0;
    auto serverSocket = Socket(port);
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
    std::cout << "== EXPERIMENT 1 ===" << std::endl;

    auto serverSocket = Socket(3000);
    serverSocket.waitForConn();

    auto bench = new Benchmark();
    for (unsigned int i = 0; i < 100; i++) {
        serverSocket.sendACK('1');
        getRTT(bench, 9001 + i);
        serverSocket.readACK('2');
        serverSocket.sendACK('3');
    }

    long long total = 0;
    for (const auto &pair : bench->getIntervals()) {
        total += pair.first;
        std::cerr << pair.first << " " << pair.second << std::endl;
    }
    std::cerr << "TOTAL " << total << std::endl;

    delete bench;
}

void
experiment_2()
{
    std::cout << "== EXPERIMENT 2 ===" << std::endl;

    auto serverSocket = Socket(3000);
    serverSocket.waitForConn();

    auto bench = new Benchmark();
    for (unsigned int i = 0; i < 100; i++) {
        auto event1 = Event("RTT");
        serverSocket.sendACK('!');
        serverSocket.readACK('#');
        auto event2 = Event("END");
        bench->addInterval(event2.timeSince(&event1), event1.getDescription());
    }

    long long total = 0;
    for (const auto &pair : bench->getIntervals()) {
        total += pair.first;
        std::cerr << pair.first << " " << pair.second << std::endl;
    }
    std::cerr << "TOTAL " << total << std::endl;

    delete bench;
}

void
experiment_3()
{
    std::cout << "== EXPERIMENT 3 ===" << std::endl;

    auto serverSocket = Socket(3000);
    serverSocket.waitForConn();

    auto bench = new Benchmark();
    for (unsigned int i = 0; i < 100; i++) {
        std::cout << "ITERATION " << i << std::endl;
        serverSocket.sendACK('1');
        largeFile(9001 + i, bench);
        serverSocket.readACK('2');
        serverSocket.sendACK('3');
    }

    long long total = 0;
    for (const auto &pair : bench->getIntervals()) {
        total += pair.first;
        std::cerr << pair.first << " " << pair.second << std::endl;
    }
    std::cerr << "TOTAL " << total << std::endl;

    delete bench;
}

void
experiment_4()
{
    std::cout << "== EXPERIMENT 4 ===" << std::endl;

    auto serverSocket = Socket(3000);
    serverSocket.waitForConn();

    auto bench = new Benchmark();
    for (unsigned int i = 0; i < 100; i++) {
        std::cout << "ITERATION " << i << std::endl;

        size_t bigSize = 0;
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

    long long total = 0;
    for (const auto &pair : bench->getIntervals()) {
        total += pair.first;
        std::cerr << pair.first << " " << pair.second << std::endl;
    }
    std::cerr << "TOTAL " << total << std::endl;

    delete bench;
}

int
main(int argc, char * argv[])
{
    std::cout << "CS 798 > P1" << std::endl;

    if (argc < 2) {
        perror("Please provide experiment number.\n");
        return 1;
    }

    std::string experiment = argv[1];
    if (experiment == "1") {
        experiment_1();
    } else if (experiment == "2") {
        experiment_2();
    } else if (experiment == "3") {
        experiment_3();
    } else if (experiment == "4") {
        experiment_4();
    } else {
        perror("No such experiment.\n");
        return 1;
    }
}