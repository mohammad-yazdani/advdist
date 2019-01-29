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
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    this->socket_fd = fd;
    this->server_addr = {};
    memset(&this->server_addr, 0, sizeof(this->server_addr));
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = htonl((uint32_t)INADDR_ANY);
    this->server_addr.sin_port = htons(static_cast<uint16_t>(this->port));
    this->setOptions();
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

    auto res = connect(this->socket_fd, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr));
    if (res) exit(res);
}

ssize_t
Socket::writeBuf(void * msg, size_t size)
{
    //std::cout << "WRITE ATTEMPT" << std::endl;
    ssize_t writeSize = send(this->socket_fd, msg, size, 0);
    if (writeSize == -1) {
        perror("WRITE FAILED\n");
    } else {
        this->event_queue.emplace_back(Event("WRITE"));
        //std::cout << "WROTE " << (char *) msg << std::endl;
    }

    return writeSize;
}
ssize_t
Socket::readBuf(char * read_buf, size_t size)
{
    //std::cout << "READ ATTEMPT" << std::endl;
    ssize_t readSize = recv(this->socket_fd, read_buf, size, MSG_WAITALL);
    if (readSize == -1) {
        perror("READ FAILED\n");
    } else {
        this->event_queue.emplace_back(Event("READ"));
        //std::cout << "READ " << read_buf << std::endl;
    }

    return readSize;
}

void
Socket::waitForConn()
{
    std::cout << "WAITING FOR CONNECTION " << this->port << std::endl;
    this->socket_fd = accept(this->socket_fd, (struct sockaddr*)&this->server_addr ,
            (socklen_t*)sizeof(this->server_addr));
    std::cout << "CONNECTION ACCEPTED " << this->port << std::endl;
}

const std::vector<Event> &
Socket::getEvent_queue() const
{
    return this->event_queue;
}

Socket::~Socket()
{
    shutdown(this->socket_fd, SHUT_RDWR);
    close(this->socket_fd);
}

void
Socket::sendACK(char ackMsg)
{
    this->writeBuf(&ackMsg, 1);
}

void
Socket::readACK(char ackMsg)
{
    char ack = 0;
    this->readBuf(&ack, 1);
    assert(ack == ackMsg);
}

void
Socket::setOptions()
{
    struct timeval tv{};
    tv.tv_sec = 30; /* seconds */
    tv.tv_usec = 0;

    if(setsockopt(this->socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Cannot Set SO_SNDTIMEO for socket\n");
    }

    if(setsockopt(this->socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Cannot Set SO_RCVTIMEO for socket\n");
    }

    int opt = 1;
    if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("Cannot Set SO_REUSEADDR | SO_REUSEPORT for socket\n");
    }
}
