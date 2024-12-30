#pragma once

#include <array>
#include <cstddef>

namespace Meta {
    template<unsigned int N> struct Factorial {
        static constexpr size_t value = N * Factorial<N - 1>::value;
    };
    
    template<> struct Factorial<0> {
        static constexpr size_t value = 1;
    };


    template<unsigned int N> struct Fibonacci {
        static constexpr size_t value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
    };

    template<> struct Fibonacci<0> {
        static constexpr size_t value = 0;
    };

    template<> struct Fibonacci<1> {
        static constexpr size_t value = 1;
    };



    template<char...args> struct Metastring {
        const char data[sizeof...(args)] = {args...};
    };

    template<ssize_t N, bool sign, char...args> struct numberString {
        typedef struct numberString<N / 10, sign, N % 10 + '0', args...>::type type;
    };

    template<char...args> struct numberString<0, true, args...> {
        typedef Metastring<'-', args...> type;
    };

    template<char...args> struct numberString<0, false, args...> {
        typedef Metastring<args...> type;
    };

    template<ssize_t N> class IntegerToString {
        typedef typename numberString<(N > 0 ? N : -N), (N < 0), '\0'>::type type;
        static constexpr type value {};
        public:
            static constexpr const char* get() { return value.data; }
    };



    template<typename T, T...args> struct MetaIntegerArray {
        const std::array<T, sizeof...(args)> data = {args...};
    };

    template<ssize_t start, ssize_t end, ssize_t step, ssize_t...args> struct _range {
        typedef struct _range<start + step, end, ((step > 0) ? ((start + step >= end) ? 0 : step) : ((start + step <= end) ? 0 : step)), args..., start>::type type;
    };

    template<ssize_t start, ssize_t end, ssize_t...args> struct _range<start, end, 0, args...> {
        typedef struct MetaIntegerArray<ssize_t, args...> type;
    };

    template<ssize_t Start, ssize_t End, ssize_t Step = 1> class Range {
        typedef struct _range<Start, End, Step>::type type;
        static constexpr type value {};
        public:
            static constexpr auto get() { return value.data; }
    };

    

    template<bool add, size_t X, size_t Factor, size_t...args> struct factors {
        typedef struct factors<X % Factor == 0, X, Factor, args...>::type type;
    };

    template<size_t X, size_t Factor, size_t...args> struct factors<true, X, Factor, args...> {
        typedef struct factors<
            false, X / Factor, 1, Factor, args...
        >::type type;
    };

    template<size_t X, size_t Factor, size_t...args> struct factors<false, X, Factor, args...> {
        typedef struct factors<
            X % (Factor + 1) == 0, X, Factor + 1, args...
        >::type type;
    };

    template<size_t Factor, size_t...args> struct factors<true, 1, Factor, args...> {
        typedef struct MetaIntegerArray<size_t, args...> type;
    };

    template<size_t Factor, size_t...args> struct factors<false, 1, Factor, args...> {
        typedef struct MetaIntegerArray<size_t, args...> type;
    };


    template<size_t X> class Factors {
        typedef struct factors<false, X, 1>::type type;
        static constexpr type value {};
        public:
            static constexpr auto get() { return value.data; }
    };


    template<size_t x> struct Collatz { static constexpr bool value = Collatz<x%2 == 0 ? x/2 : (3*x+1)/2>::value; };
    template<> struct Collatz<1> { static constexpr const bool value = true; };

    template<size_t x, size_t y> struct GCD { static constexpr size_t value = GCD<y, x % y>::value; };
    template<size_t x> struct GCD<x, 0> { static constexpr size_t value = x; };

    template<size_t n> struct Hanoi { static constexpr size_t value = 2 * Hanoi<n - 1>::value + 1; };
    template<> struct Hanoi<1> { static constexpr size_t value = 1; };
};

