//
// Created by Mohammad Yazdani on 2019-01-26.
//

#include <net/Event.h>


Event::Event(TCPEventType type) : type(type)
{
    this->start_stamp = std::chrono::high_resolution_clock::now();
}

TCPEventType Event::getType() const
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
        case TCPEventType::RTT:
            return "RTT";
    }
}

uint64_t
Event::getStamp() const
{
    auto start = static_cast<uint64_t>(this->start_stamp.time_since_epoch().count());
    return start;
}

long
Event::timeSince(Event *pastEvent)
{
    auto diff = this->start_stamp - pastEvent->start_stamp;
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count();
    return duration;
}

double Event::toSeconds(long nanoseconds) {
    return ((double)nanoseconds / NANOSECOND);
}
