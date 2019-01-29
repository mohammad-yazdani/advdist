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
getRTT(Benchmark * bench)
{
    std::cout << "== RTT TEST ===" << std::endl;
    auto event1 = Event("RTT");
    auto clientSocket = Socket("18.221.69.86", 8080);
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
largeFile(const std::string &abspath, Benchmark * bench)
{
    std::cout << "== LARGE FILE TEST ===" << std::endl;

    void * data = mmap(abspath);
    auto size = getFileSize(abspath);

    auto clientSocket = Socket("18.221.69.86", 3000);

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

    if (ack == '!')
        return;
    else
        perror("Did not get ACK from server.");
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
main(int argc, char * argv[])
{
    std::cout << "CS 798 > P1 > Q1 | Client" << std::endl;

    if (argc < 2) {
        perror("Please provide file path.\n");
        return 1;
    }
    std::string abspath = argv[1];
    (void)abspath;
    experiment_1();
    //largeFile(abspath, bench);
}