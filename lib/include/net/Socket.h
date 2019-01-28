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
public:
    explicit Socket(unsigned int port);
    Socket(std::string ip, unsigned int port);
    virtual ~Socket();

    void waitForConn();
    long askRTT();
    void waitRTT();

    void writeBuf(std::string msg, size_t size);
    std::string readBuf(size_t size);

    const std::vector<Event> &getEvent_queue() const;
};


#endif //ADVDIST_SOCKET_H
