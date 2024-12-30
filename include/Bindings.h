#ifndef BINDINGS_H
/**
 * @brief This header file is responsible for glueing
 * different underlying operating systems,
 * compilers and C/C++ verions to create a coherent
 * system for platform-independent
 * functions and macros.
 * 
 * If this header is used in C++, functions related to
 * obtaining the information about the current timestamp
 * and resolution from a monotonic clock should be used
 * in place of std::chrono:: equivalents as they
 * return an unsigned integer instead of
 * a weird structure, especially for calculating
 * how much time elapsed from a known timestamp...
 * and to avoid atrocities like this:
 * 
 * std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
 * ...
 * std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
 * std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
 * 
 * vs
 * uint64_t begin = getClockTime();
 * ...
 * uint64_t end = getClockTime();
 * end - begin
 * 
 * ...or just use SDL_GetPerformanceXXX, as they're already doing what is done here.
 * 
 * This header should be included in every file that wishes to use
 * macros like NoDiscard, ForceInline and clock functions, but it'll be
 * better to include it everywhere.
 */
#define BINDINGS_H

#pragma once

#include "Macros.h"

#ifdef __cplusplus
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <type_traits>
extern "C" {
#else
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#endif /* C++ */

#ifndef CompilePlatform
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__NT__)
#define CompilePlatform "Windows"
#ifndef WINDOWS
#define WINDOWS 1
#endif /* Windows */
#include <profileapi.h>
#include <synchapi.h>
#elif defined(__linux__)
#define CompilePlatform "Linux"
#ifndef LINUX
#define LINUX 1
#endif /* Linux */
#include <unistd.h>
// #elif defined(__APPLE__) /* MacOS */
// #include <unistd.h>
// #elif defined(__unix__) /* Other Unix-like platforms */
// #include <unistd.h> <--- may add support later on
#else
#error "Unsupported platform"
#endif /* OS */
#endif /* Compile platform */



/**
 * @brief Get the monotonic clock timestamp based on the underlying OS.
 * 
 * @return Clock timestamp as uint64_t, i.e. 
 */
ForceInline uint64_t getClockTime() {
#if defined(WINDOWS)
    LARGE_INTEGER t;
    assert(QueryPerformanceCounter(&t) != 0);
    return (uint64_t)t.QuadPart;
#elif defined(LINUX)
    struct timespec t;
    uint64_t s, ns;
    assert(clock_gettime(CLOCK_MONOTONIC, &t) == 0);
    s = t.tv_sec;
    ns = t.tv_nsec;
    clock_getres(CLOCK_MONOTONIC, &t);
    return (uint64_t)(s * 1000000000 + ns) / t.tv_nsec;
#endif /* OS */
}

/**
 * @brief Get the monotonic clock resolution based on the underlying OS.
 * 
 * @return Clock resolution as uint64_t, i.e. how many ticks of
 * the clock are there in a second.
 */
ForceInline uint64_t getClockResolution() {
#if defined(WINDOWS)
    LARGE_INTEGER frequency;
    assert(QueryPerformanceFrequency(&frequency) != 0);
    return (uint64_t)frequency.QuadPart;
#elif defined(LINUX)
    struct timespec t;
    assert(clock_getres(CLOCK_MONOTONIC, &t) == 0);
    return (uint64_t)(1000000000 / t.tv_nsec);
#endif /* OS */
}

#ifndef sleep
#if defined(WINDOWS)
#define sleep(ms) Sleep(ms)
#elif defined(LINUX)
#define sleep(ms) do {                  \
    struct timespec ts;                 \
    ts.tv_nsec = (ms % 1000) * 1000000; \
    ts.tv_sec = ms / 1000;              \
    nanosleep(&ts, &ts);                \
} while(0)
#else
#define sleep(ms)
#endif /* OS */
#endif /* sleep */

#ifdef __cplusplus
}
#endif /* C++ */

ForceInline uint64_t roundUpToPowerOf2(uint64_t in) {
    in--;
    in |= in >> 1;
    in |= in >> 2;
    in |= in >> 4;
    in |= in >> 8;
    in |= in >> 16;
    in |= in >> 32;
    in++;
    return in;
}


#endif /* Header */