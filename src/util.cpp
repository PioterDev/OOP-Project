#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdint>
#include <string>
#include <sstream>

using std::string, std::ostringstream;

static constexpr char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

const char* emptyCString = "";

void itos(int in, const unsigned int base, char* buf, const size_t bufsize) {
    if(base > sizeof(digits) - 1 || base < 2) return;

    char tmp[64] = {0};
    size_t i = 0, sign = 0;
    if(in == 0 && bufsize >= 2ull) {
        tmp[0] = '0';
        strncpy(buf, tmp, 2);
        return;
    }
    if(in < 0) {
        tmp[0] = '-';
        i++; sign++; 
    }
    int len = -1, incpy = in;
    while(incpy != 0) {
        len++;
        incpy /= base;
    }
    in = abs(in);
    while(in != 0) {
        if(i == bufsize - 1) return;
        char digit = in % base;
        tmp[sign + len] = digits[(int)digit];
        i++; len--;
        in /= base;
    }
    strncpy(buf, tmp, i);
}

template<class...Args> string toString(Args...args) {
    ostringstream s;
    (s << ... << args);
    return s.str();
}

size_t wstrlen(const char16_t* str) {
    const char16_t* s = str;
    while(*s) ++s;
    return (size_t)(s - str) >> 1;
}