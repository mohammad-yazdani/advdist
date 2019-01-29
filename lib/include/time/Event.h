//
// Created by Mohammad Yazdani on 2019-01-26.
//

#ifndef ADVDIST_EVENT_H
#define ADVDIST_EVENT_H

#include <chrono>
#include <string>

class Event {
private:
    std::chrono::high_resolution_clock::time_point start_stamp;
    std::string description;
public:
    Event(const std::string &description);
    long timeSince(Event *pastEvent);

    const std::string &getDescription() const;
};


#endif //ADVDIST_EVENT_H
