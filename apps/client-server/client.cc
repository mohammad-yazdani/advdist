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

void
getRTT()
{
    auto clientSocket = new Socket("18.221.69.86", 8080);
    clientSocket->waitRTT();
    delete clientSocket;
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

int
main()
{
    std::cout << "CS 798 > P1 > Q1 | Client" << std::endl;
    std::cout << "Buffer size: " << BUFFER_SIZE << std::endl;

    std::string abspath = "/Users/raynor106/advdist/apps/client-server/data/10mb.txt";
    void * data = mmap(abspath);
    auto size = getFileSize(abspath);

    auto clientSocket = new Socket("18.221.69.86", 3000);
    clientSocket->writeBuf(data, size);
    char ack = 0;
    clientSocket->readBuf(&ack, 1);
    delete clientSocket;

    if (ack == '!')
        return 0;
    else
        return 1;
}