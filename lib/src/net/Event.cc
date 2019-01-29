#include <utility>

//
// Created by Mohammad Yazdani on 2019-01-26.
//

#include <net/Event.h>

Event::Event(const std::string &description) : description(description)
{
    this->start_stamp = std::chrono::high_resolution_clock::now();
}


long
Event::timeSince(Event *pastEvent)
{
    auto diff = this->start_stamp - pastEvent->start_stamp;
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count();
    return duration;
}

const std::string &Event::getDescription() const
{
    return this->description;
}
