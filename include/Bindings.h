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
#endif /* C++17 */
#endif /* Defining NoDiscard */

#ifndef NoDiscardReason
#if __cplusplus >= 202002L
#define NoDiscardReason(reason) [[nodiscard(reason)]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define NoDiscardReason(reason) __atrribute__((warn_unused_result))
#else
#define NoDiscardReason(reason)
#endif /* Compiler */
#endif /* C++20 */
#endif /* Defining NoDiscardReason */

// #ifndef Unused
// #if __cplusplus >= 201703L
// #define Unused [[maybe_unused]]
// #else
// #if defined(__GNUC__) || defined(__clang__)
// #define Unused __attribute__((unused))
// #else
// #define Unused
// #endif /* Compiler */
// #endif /* C++11 */
// #endif /* Defining Unused */

#ifndef Likely
#if __cplusplus >= 202002L
#define Likely [[likely]]
#else
#define Likely
#endif /* C++20 */
#endif /* Defining Likely */

#ifndef Unlikely
#if __cplusplus >= 202002L
#define Unlikely [[unlikely]]
#else
#define Unlikely
#endif /* C++20 */
#endif /* Defining Unlikely */

#ifndef Deprecated
#if __cplusplus >= 201402L
#define Deprecated [[deprecated]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define Deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#define Deprecated __declspec(deprecated)
#endif /* Compilers */
#endif /* C++14 */
#endif /* Defining Deprecated */

#ifndef DeprecatedMsg
#if __cplusplus >= 201402L
#define DeprecatedMsg(msg) [[deprecated(msg)]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define DeprecatedMsg __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define Deprecated __declspec(deprecated(msg))
#endif /* Compilers */
#endif /* C++14 */
#endif /* Defining DeprecatedMsg */

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
#endif /* C23 */
#endif /* Defining NoDiscard */

#ifndef NoDiscardReason
#if __STDC_VERSION__ >= 202311L
#define NoDiscardReason(reason) [[nodiscard(reason)]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define NoDiscardReason(reason) __attribute__((warn_unused_result))
#else
#define NoDiscardReason(reason)
#endif /* Compiler */
#endif /* C23 */
#endif /* Defining NoDiscardReason */

#ifndef Unused
#if __STDC_VERSION__ >= 202311L
#define Unused [[maybe_unused]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define Unused __attribute__((unused))
#else
#define Unused
#endif /* Compiler */
#endif /* C23 */
#endif /* Defining Unused */

#ifndef Likely
#define Likely
#endif

#ifndef Unlikely
#define Unlikely
#endif

#ifndef Deprecated
#if __STDC_VERSION__ >= 202311L
#define Deprecated [[deprecated]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define Deprecated __attribute__((deprecated))
#else
#define Deprecated
#endif /* Compilers */
#endif /* C23 */
#endif /* Defining Deprecated */

#ifndef DeprecatedMsg
#if __STDC_VERSION__ >= 202311L
#define DeprecatedMsg(msg) [[deprecated(msg)]]
#else
#if defined(__GNUC__) || defined(__clang__)
#define DeprecatedMsg(msg) __attribute__((deprecated(msg)))
#else
#define DeprecatedMsg(msg)
#endif /* Compilers */
#endif /* C23 */
#endif /* Defining Deprecated */

#endif /* C++ */

#ifndef Packed
#if defined(__GNUC__) || defined(__clang__)
#define Packed __attribute__((packed))
#else
#define Packed
#endif /* Compiler */
#endif /* Defining Packed */

#ifndef PackedAligned
#if defined(__GNUC__) || defined(__clang__)
#define PackedAligned(n) __attribute__((packed, aligned(n)))
#else
#define PackedAligned(n)
#endif /* Compiler */
#endif /* Defining Packed */

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

#ifndef ForceInline
#if ((defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__))
#define ForceInline inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ForceInline __forceinline /* MSVC */
#else
#define ForceInline inline
#endif /* Compiler */
#endif /* Defining ForceInline */



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
#ifdef WINDOWS
#define sleep(ms) Sleep(ms)
#elif defined(LINUX)
#define sleep(ms) usleep(1000 * ms)
#else
#define sleep(ms)
#endif /* OS */
#endif /* sleep */

#ifdef __cplusplus
}
#endif /* C++ */


#endif /* Header */