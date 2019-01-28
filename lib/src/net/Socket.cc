//
// Created by Mohammad Yazdani on 2019-01-26.
//

#include <iostream>
#include <string.h>
#include <net/Socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cassert>
#include <sys/socket.h>

Socket::Socket(unsigned int port) : port(port)
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    this->server_addr = {};
    memset(&this->server_addr, 0, sizeof(this->server_addr));
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = htonl((uint32_t)INADDR_ANY);
    this->server_addr.sin_port = htons(static_cast<uint16_t>(this->port));

    bind(this->socket_fd, (struct sockaddr*)&this->server_addr , sizeof(this->server_addr));
    listen(this->socket_fd, MAX_BACKLOG);
}

Socket::Socket(std::string ip, unsigned int port) : port(port)
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    this->server_addr = {};
    memset(&this->server_addr, 0, sizeof(this->server_addr));
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(static_cast<uint16_t>(this->port));
    this->server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    connect(this->socket_fd, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr));
}

void
Socket::writeBuf(void * msg, size_t size)
{
    this->event_queue.emplace_back(Event(TCPEventType::WRITE));
    size_t sizeLimit = size;
    char * toWrite = (char *)msg;

    size_t writeBlock = BUFFER_SIZE;

    while (true) {
        std::cout << sizeLimit << std::endl;
        ssize_t writeSize = 0;
        if (sizeLimit < writeBlock) writeSize = write(this->socket_fd, toWrite, sizeLimit);
        else writeSize = write(this->socket_fd, toWrite, writeBlock);
        sizeLimit -= writeSize;

        std::cout << "WROTE " << writeSize << " bytes | TOTAL " << (size - sizeLimit) << " OF " << size << std::endl;
        if (!sizeLimit) break;
        toWrite += writeBlock;
    }
}

int
Socket::readBuf(char * read_buf, size_t size)
{
    this->event_queue.emplace_back(Event(TCPEventType::READ));

    char * buffer_head = read_buf;
    size_t sizeLimit = size;
    while (true) {
        ssize_t readSize = read(this->socket_fd, read_buf, BUFFER_SIZE);

        if (readSize == 0) continue;

        sizeLimit -= readSize;

        std::cout << "READ " << readSize << " bytes | TOTAL " << (size - sizeLimit) << " OF " << size << std::endl;
        if (readSize < 0) {
            std::cout << "READ FAILED" << std::endl;
            exit(1);
        }

        if (!sizeLimit) break;
    }

    return 0;
}

void
Socket::waitForConn()
{
    this->socket_fd = accept(this->socket_fd, (struct sockaddr*) nullptr, nullptr);
}

const std::vector<Event> &
Socket::getEvent_queue() const
{
    return event_queue;
}

Socket::~Socket()
{
    shutdown(this->socket_fd, SHUT_RDWR);
    close(this->socket_fd);
}

long
Socket::askRTT() {
    auto e1 = Event(TCPEventType::RTT);
    char start = '!';
    this->writeBuf(&start, 1);
    char ack = 0;
    this->readBuf(&ack, 1);
    assert(ack == '#');
    auto e2 = Event(TCPEventType::RTT);
    return e2.timeSince(&e1);
}

void
Socket::waitRTT()
{
    char ack = 0;
    this->readBuf(&ack, 1);
    this->writeBuf((void *) "#", 1);
}
