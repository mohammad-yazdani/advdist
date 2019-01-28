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

Socket::Socket(unsigned int port) : port(port)
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    this->server_addr = {};
    memset(&this->server_addr, 0, sizeof(this->server_addr));
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = htonl((uint32_t)INADDR_ANY);
    this->server_addr.sin_port = htons((uint16_t)this->port);

    bind(this->socket_fd, (struct sockaddr*)&this->server_addr , sizeof(this->server_addr));
    listen(this->socket_fd, MAX_BACKLOG);
}

Socket::Socket(std::string ip, unsigned int port) : port(port)
{
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    this->server_addr = {};
    memset(&this->server_addr, 0, sizeof(this->server_addr));
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(this->port);
    this->server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    connect(this->socket_fd, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr));
}

void
Socket::writeBuf(std::string msg, size_t size)
{
    this->event_queue.emplace_back(Event(TCPEventType::WRITE));
    ssize_t actualSize = write(this->socket_fd, msg.c_str(), size);
    assert(size == actualSize);
}

std::string
Socket::readBuf(size_t size)
{
    this->event_queue.emplace_back(Event(TCPEventType::READ));
    char buf[1];
    ssize_t actualSize = read(this->socket_fd, buf, size);
    assert(size == actualSize);
    return std::string(buf);
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
    close(this->socket_fd);
}

long
Socket::askRTT() {
    auto e1 = Event(TCPEventType::RTT);
    this->writeBuf("!", 1);
    auto res = this->readBuf(1);
    assert(res == "#");
    auto e2 = Event(TCPEventType::RTT);
    return e2.timeSince(&e1);
}

void
Socket::waitRTT()
{
    auto res = this->readBuf(1);
    this->writeBuf("#", 1);
}
