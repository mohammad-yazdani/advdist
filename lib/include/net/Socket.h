//
// Created by Mohammad Yazdani on 2019-01-26.
//

#ifndef ADVDIST_SOCKET_H
#define ADVDIST_SOCKET_H

#include <vector>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Event.h"

#define MAX_BACKLOG 10

class Socket {
private:
    std::vector<Event> event_queue;
    unsigned int port;
    int socket_fd;
    sockaddr_in server_addr;

    void setOptions();
public:
    explicit Socket(unsigned int port);
    Socket(std::string ip, unsigned int port);
    virtual ~Socket();

    void waitForConn();
    void sendACK(char ackMsg);
    void readACK(char ackMsg);

    ssize_t writeBuf(void * buf, size_t size);
    ssize_t readBuf(char * read_buf, size_t size);

    const std::vector<Event> &getEvent_queue() const;
};


#endif //ADVDIST_SOCKET_H
