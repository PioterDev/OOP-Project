#pragma once

/**
 * @brief This header contains useful macros for various,
 * often compiler-dependent functionality like force-inlining,
 * hints for branch prediction, struct packing and other
 * things.
 */

#ifdef __cplusplus /* C++ */
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

#ifndef NoReturn
#if __cplusplus >= 201103L
#define NoReturn [[noreturn]]
#else
#if defined(__GNUC__)
#define NoReturn __attribute__((noreturn))
#else
#error "NoReturn macro cannot be defined"
#endif /* Compilers */
#endif /* C++11 */
#endif /* Defining NoReturn */

#else /* C */

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

#ifndef NoReturn
#if __STDC_VERSION >= 202311L
#define NoReturn [[noreturn]]
#else
#if defined(__GNUC__)
#define NoReturn __attribute__((noreturn))
#else
#error "NoReturn macro cannot be defined"
#endif /* Compilers */
#endif /* C23 */
#endif /* Defining NoReturn */


#endif /* C/C++ */

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

#ifndef ForceInline
#if ((defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__))
#define ForceInline inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ForceInline __forceinline /* MSVC */
#else
#define ForceInline inline
#endif /* Compiler */
#endif /* Defining ForceInline */

#ifndef BeforeMain

#endif

#ifdef __cplusplus
#define BeforeMain(f) \
static void f(); \
struct __##f##__ { __##f##__() { f(); } }; static __##f##__ __##f##_; \
static void f()
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU",read)
#define INITIALIZER_(f,p) \
static void f(); \
__declspec(allocate(".CRT$XCU")) void (*f##_)() = f; \
__pragma(comment(linker,"/include:" p #f "_")) \
static void f()
#ifdef _WIN64
#define BeforeMain(f) INITIALIZER_(f,"")
#else
#define BeforeMain(f) INITIALIZER_(f,"_")
#endif
#else
#define BeforeMain(f) static void f(void) __attribute__((constructor))
#endif