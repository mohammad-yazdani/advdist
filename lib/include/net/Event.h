//
// Created by Mohammad Yazdani on 2019-01-26.
//

#ifndef ADVDIST_EVENT_H
#define ADVDIST_EVENT_H

#include <chrono>
#include <string>

#define NANOSECOND 1000000000

enum class TCPEventType {
    READ,
    WRITE,
    RTT
};

class Event {
private:
    std::chrono::high_resolution_clock::time_point start_stamp;
    TCPEventType type;
public:
    explicit Event(TCPEventType type);
    TCPEventType getType() const;

    std::string getStringType() const;
    uint64_t getStamp() const;
    long timeSince(Event *pastEvent);

    static double toSeconds(long nanoseconds);
};


#endif //ADVDIST_EVENT_H
