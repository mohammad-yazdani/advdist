//
// Created by Mohammad Yazdani on 2019-01-27.
//

#include <iostream>
#include <net/Socket.h>
#include <string>
#include <thread>
#include <fstream>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time/Benchmark.h>

void
getRTT(Benchmark * bench, unsigned int port)
{
    auto event1 = Event("RTT");
    auto clientSocket = Socket("18.221.69.86", port);
    clientSocket.readACK('!');
    clientSocket.sendACK('#');
    auto event2 = Event("END");

    bench->addInterval(event2.timeSince(&event1), event1.getDescription());
}

size_t
getFileSize(const std::string &abspath)
{
    struct stat st{};
    stat(abspath.c_str(), &st);
    return static_cast<size_t>(st.st_size);
}

void *
mmap(const std::string &abspath)
{
    int fd = open(abspath.c_str(), O_RDONLY, 0);

    auto size = static_cast<size_t>(getFileSize(abspath));
    void * mapped = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    return mapped;
}

void
largeFile(unsigned int port, const std::string &abspath, Benchmark * bench)
{
    void * data = mmap(abspath);
    auto size = getFileSize(abspath);

    auto clientSocket = Socket("18.221.69.86", port);

    // Send file size
    auto event1 = Event("SEND FILE SIZE");
    clientSocket.writeBuf(&size, 8);
    // Get ACK from server
    auto event2 = Event("READ FILE SIZE CONFIRM");
    clientSocket.readACK('!');
    auto event3 = Event("SEND BIG FILE");
    clientSocket.writeBuf(data, size);
    char ack = 0;
    auto event4 = Event("READ BIG FILE CONFIRM");
    clientSocket.readBuf(&ack, 1);

    auto event5 = Event("END");

    bench->addInterval(event2.timeSince(&event1), event1.getDescription());
    bench->addInterval(event3.timeSince(&event2), event2.getDescription());
    bench->addInterval(event4.timeSince(&event3), event3.getDescription());
    bench->addInterval(event5.timeSince(&event4), event4.getDescription());
    bench->addInterval(event5.timeSince(&event5), event5.getDescription());

    if (ack == '!')
        return;
    else
        perror("Did not get ACK from server.");
}

void
experiment_1()
{
    std::cout << "== EXPERIMENT 1 ===" << std::endl;

    auto clientSocket = Socket("18.221.69.86", 3000);

    auto bench = new Benchmark();
    for (unsigned int i = 0; i < 100; i++) {
        clientSocket.readACK('1');
        getRTT(bench, 9001 + i);
        clientSocket.sendACK('2');
        clientSocket.readACK('3');
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

    auto clientSocket = Socket("18.221.69.86", 3000);

    auto bench = new Benchmark();
    for (unsigned int i = 0; i < 100; i++) {
        auto event1 = Event("RTT");
        clientSocket.sendACK('#');
        clientSocket.readACK('!');
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
experiment_3(const std::string &abspath)
{
    std::cout << "== EXPERIMENT 3 ===" << std::endl;

    auto clientSocket = Socket("18.221.69.86", 3000);

    auto bench = new Benchmark();
    for (unsigned int i = 0; i < 100; i++) {
        std::cout << "ITERATION " << i << std::endl;
        clientSocket.readACK('1');
        largeFile(9001 + i, abspath, bench);
        clientSocket.sendACK('2');
        clientSocket.readACK('3');
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
experiment_4(const std::string &abspath)
{
    std::cout << "== EXPERIMENT 4 ===" << std::endl;

    auto clientSocket = Socket("18.221.69.86", 3000);
    auto bench = new Benchmark();

    for (unsigned int i = 0; i < 100; i++) {
        std::cout << "ITERATION " << i << std::endl;

        void * data = mmap(abspath);
        auto size = getFileSize(abspath);

        // Send file size
        auto event1 = Event("SEND FILE SIZE");
        clientSocket.writeBuf(&size, 8);
        // Get ACK from server
        auto event2 = Event("READ FILE SIZE CONFIRM");
        clientSocket.readACK('!');
        auto event3 = Event("SEND BIG FILE");
        clientSocket.writeBuf(data, size);
        char ack = 0;
        auto event4 = Event("READ BIG FILE CONFIRM");
        clientSocket.readBuf(&ack, 1);

        auto event5 = Event("END");

        bench->addInterval(event2.timeSince(&event1), event1.getDescription());
        bench->addInterval(event3.timeSince(&event2), event2.getDescription());
        bench->addInterval(event4.timeSince(&event3), event3.getDescription());
        bench->addInterval(event5.timeSince(&event4), event4.getDescription());
        bench->addInterval(event5.timeSince(&event5), event5.getDescription());

        if (ack != '!') perror("Did not get ACK from server.");
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
    } else if (experiment == "3" || experiment == "4") {
        if (argc < 3) {
            perror("Please provide file path.\n");
            return 1;
        }
        std::string abspath = argv[2];
        if (experiment == "3") experiment_3(abspath);
        else experiment_4(abspath);
    } else {
        perror("No such experiment.\n");
        return 1;
    }
}