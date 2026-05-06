// +-------------------------------------------------------------------------
// | prelude.h
// | 
// | Author: Gilbert Bernstein
// +-------------------------------------------------------------------------
// | COPYRIGHT:
// |    Copyright Gilbert Bernstein 2013
// |    See the included COPYRIGHT file for further details.
// |    
// |    This file is part of the Cork library.
// |
// |    Cork is free software: you can redistribute it and/or modify
// |    it under the terms of the GNU Lesser General Public License as
// |    published by the Free Software Foundation, either version 3 of
// |    the License, or (at your option) any later version.
// |
// |    Cork is distributed in the hope that it will be useful,
// |    but WITHOUT ANY WARRANTY; without even the implied warranty of
// |    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// |    GNU Lesser General Public License for more details.
// |
// |    You should have received a copy 
// |    of the GNU Lesser General Public License
// |    along with Cork.  If not, see <http://www.gnu.org/licenses/>.
// +-------------------------------------------------------------------------
#pragma once

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <array>
#include <vector>
#include <optional>
#include <variant>

using uint = unsigned int;
using byte = unsigned char;

// ***********
// * Logging

// error log -- silent; will not stop program
std::ostream &err();

#ifndef ENSURE
#define ENSURE(STATEMENT) { \
    if(!(STATEMENT)) { \
        std::cerr << "ENSURE FAILED at " \
                  << __FILE__ << ", line #" << __LINE__ << ":\n" \
                  << "    " << #STATEMENT << std::endl; \
        err()     << "ENSURE FAILED at " \
                  << __FILE__ << ", line #" << __LINE__ << ":\n" \
                  << "    " << #STATEMENT << std::endl; \
        exit(1); \
    } \
}
#endif // ENSURE

// Use ERROR to print an error message tagged with the given file/line #
#ifndef CORK_ERROR
#define CORK_ERROR(message) { \
    std::cerr << "error at " \
              << __FILE__ << ", line #" << __LINE__ << ": " \
              << (message) << std::endl; \
    err()     << "error at " \
              << __FILE__ << ", line #" << __LINE__ << ": " \
              << (message) << std::endl; \
}
#endif // CORK_ERROR

// Use MARKER for debugging to create a trace of control flow...
#ifndef MARKER
#define MARKER(message) { \
    std::cout << "marker at " \
              << __FILE__ << ", line #" << __LINE__ << ": " \
              << (message) << std::endl; \
}
#endif // MARKER

// ***********
// * Assorted

template<typename T>
constexpr T clamp(T val, T mina, T maxa) {
    return std::max(mina, std::min(maxa, val));
}

template<typename T>
constexpr T wrap(T val, T mina, T maxa) {
    T range = maxa - mina;
    T result = std::fmod(val - mina, range);
    if (result < T{}) result += range;
    return result + mina;
}

constexpr double deg2rad(double deg) {
    return (M_PI/180.0) * deg;
}
constexpr double rad2deg(double rad) {
    return (180.0/M_PI) * rad;
}

// **********
// * Timing

#ifdef _WIN32
#include <winsock.h>
#endif

class Timer {
public:
    Timer(); // automatically start timer on creation
    ~Timer();
public:
    void start();
    // returns lap time in milliseconds
    double lap();
    // returns total ellapsed time in milliseconds
    double stop();
    // re-retrieve values
    // if this operation is meaningless, you will receive 0.0 instead
    double lastLap() const;
    double ellapsed() const;
private:
    timeval init;
    timeval prev;
    double last_lap_duration;
    double ellapsed_time_duration;
    bool isRunning;
};

// ***********
// * Random

#include <random>

inline void initRand() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

inline double drand(double min, double max) {
    static std::default_random_engine gen{std::random_device{}()};
    std::uniform_real_distribution<double> dist{min, max};
    return dist(gen);
}

inline uint randMod(uint range) {
    static std::default_random_engine gen{std::random_device{}()};
    std::uniform_int_distribution<uint> dist{0, range - 1};
    return dist(gen);
}



