//
// Created by Mohammad Yazdani on 2019-01-26.
//

#include <net/Event.h>

Event::Event(TCPEventType type) : type(type)
{
    this->epoch = std::time(nullptr);
}

time_t
Event::getEpoch() const
{
    return this->epoch;
}

TCPEventType
Event::getType() const
{
    return this->type;
}

std::string
Event::getStringType() const
{
    TCPEventType tcpEventType = this->getType();
    switch (tcpEventType) {
        case TCPEventType::READ :
            return "READ";
        case TCPEventType::WRITE:
            return "WRITE";
        case TCPEventType::ACK:
            return "ACK";
    }
}
