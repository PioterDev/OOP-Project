#ifndef BINDINGS_H
/**
 * @brief This header file is responsible for gluing
 * different underlying operating systems,
 * compilers and C/C++ to create a coherent
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

#ifdef __cplusplus
#include <cassert>
#include <cmath>
#include <cstdint>
#include <ctime>

#ifndef NoDiscard
#if __cplusplus >= 201703L
#define NoDiscard [[nodiscard]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define NoDiscard __atrribute__((warn_unused_result))
#else
#define NoDiscard
#endif /* Compiler */
#endif /* Check C++ standard */
#endif /* Defining NoDiscard */
extern "C" {
#else
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#ifndef NoDiscard
#if __STDC_VERSION__ >= 202311L
#define NoDiscard [[nodiscard]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define NoDiscard __attribute__((warn_unused_result))
#else
#define NoDiscard
#endif /* Compiler */
#endif /* Check C standard */
#endif /* Defining NoDiscard */
#endif /* C++ */

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__NT__)
#ifndef WINDOWS
#define WINDOWS 1
#endif /* Windows */
#include <windows.h>
#elif defined(__linux__)
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

#ifndef ForceInline
#ifdef _MSC_VER
#define ForceInline __forceinline /* MSVC */
#elif ((defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__))
#define ForceInline static inline __attribute__((always_inline))
#else
#define ForceInline static inline
#endif /* Compiler */
#endif /* Defining ForceInline */

#ifndef Unused
#ifdef __cplusplus
#if __cplusplus >= 201103L
#define Unused [[maybe_unused]]
#else 
#if defined(__GNUC__) || defined(__clang__)
#define Unused __attribute__((unused))
#else
#define Unused
#endif /* Compiler */
#endif /* C++11 */
#else
#if __STDC_VERSION__ >= 202311L
#define Unused [[maybe_unused]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define Unused __attribute__((unused))
#else
#define Unused
#endif /* Compiler */
#endif /* C23 */
#endif /* C++ */
#endif /* Defining Unused */

/**
 * @brief Get the monotonic clock resolution based on the underlying OS.
 * 
 * @return Clock resolution as uint64_t
 */
ForceInline uint64_t getClockResolution() {
#if defined(WINDOWS)
    LARGE_INTEGER frequency;
    assert(QueryPerformanceFrequency(&frequency) != 0);
    return (uint64_t)frequency.QuadPart;
#elif defined(LINUX)
    struct timespec t;
    assert(clock_getres(CLOCK_MONOTONIC, &t) == 0);
    return (uint64_t)t.tv_nsec;
#endif /* OS */
}

/**
 * @brief Get the monotonic clock timestamp based on the underlying OS.
 * 
 * @return Clock timestamp as uint64_t
 */
ForceInline uint64_t getClockTime() {
#if defined(WINDOWS)
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (uint64_t)t.QuadPart;
#elif defined(LINUX)
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_nsec;
#endif /* OS */
}

#ifndef sleep
#ifdef WINDOWS
#define sleep(ms) Sleep(ms)
#elif defined(LINUX)
#define sleep(ms) usleep(1000 * ms)
#endif /* OS */
#endif /* sleep(ms) */

#ifdef __cplusplus
}
#endif /* C++ */


#endif /* Header */