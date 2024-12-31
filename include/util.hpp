#pragma once

#include <cstdint>
#include <string>

/**
 * @brief Integer to C string.
 * 
 * @param in integer
 * @param base conversion base from 2 to 36
 * @param buf user-provided output buffer
 * @param bufsize size of provided buffer
 */
void itos(int in, const unsigned int base, char* buf, const size_t bufsize);

/**
 * @brief Variadic to_string.
 *
 * @return converted string
 */
template<class...Args> std::string toString(Args...args);

/**
 * @brief strlen for char16_t.
 * 
 * @param str 
 * @return length of `str`
 */
size_t wstrlen(const char16_t* str);