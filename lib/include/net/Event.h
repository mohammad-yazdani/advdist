//
// Created by Mohammad Yazdani on 2019-01-26.
//

#ifndef ADVDIST_EVENT_H
#define ADVDIST_EVENT_H

#include <ctime>
#include <string>

enum class TCPEventType {
    READ,
    WRITE,
    ACK
};

class Event {
private:
    time_t epoch;
    TCPEventType type;
public:
    explicit Event(TCPEventType type);
    time_t getEpoch() const;
    TCPEventType getType() const;
    std::string getStringType() const;
};


#endif //ADVDIST_EVENT_H
