#include <iostream>
#include <random>
#include <chrono>

#ifndef RANDOM_GENERATOR
#define RANDOM_GENERATOR

inline double getRandom()
{
    // // random setup
    // const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // std::mt19937 generator(seed);

    // // distribution
    // const int min_range = 0;
    // const int max_range = 1;
    // std::uniform_real_distribution<double> distrib(min_range,max_range);

    // double value = distrib(generator);
    // return value;
    
    static std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_real_distribution<double> distrib(0.0, 1.0);
    return distrib(generator);
}

#endif