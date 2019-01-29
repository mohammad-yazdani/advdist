//
// Created by Mohammad Yazdani on 2019-01-28.
//

#ifndef ADVDIST_BENCHMARK_H
#define ADVDIST_BENCHMARK_H

#include <vector>
#include <string>

class Benchmark {
    std::vector<std::pair<long, std::string>> intervals;
public:
    Benchmark();
    void addInterval(long time, std::string detail);
    const std::vector<std::pair<long, std::string>> &getIntervals() const;
};


#endif //ADVDIST_BENCHMARK_H
