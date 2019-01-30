//
// Created by Mohammad Yazdani on 2019-01-28.
//

#include <time/Benchmark.h>

#include "time/Benchmark.h"

Benchmark::Benchmark()
{
    this->intervals = {};
}

void
Benchmark::addInterval(long time, std::string detail)
{
    this->intervals.emplace_back(std::pair<long, std::string>(time, detail));
}

const
std::vector<std::pair<long, std::string>> &Benchmark::getIntervals() const
{
    return this->intervals;
}
