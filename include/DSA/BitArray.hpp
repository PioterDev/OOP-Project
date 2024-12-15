#pragma once

#include "Bindings.h"

#include <stdexcept>

#include "deus.hpp"

/**
 * @brief A bit array implementation as an alternative to
 * std::vector<bool> (this thing is...not great).
 * It is guaranteed that bits are stored contigously here.
 * 
 * @throws std::bad_alloc if it fails to (re)allocate memory
 */
class BitArray {
    private:
        u64 numberOfBits = 0;
        u64 numberOfBitsAvailable = 0;
        u8* bits = nullptr;

        ForceInline u64 roundUpToPowerOf2(u64 in) {
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

        bool reallocate(u64 minimumNewSize);
    public:
        /**
         * @brief Constructs a BitArray.
         * 
         * @param initialSize number of bits(!!!) initially allocated
         * 
         * @throws std::bad_alloc on allocation failure
         */
        BitArray(const u64 initialSize);
        
        ~BitArray() {
            free(this->bits);
        }

        BitArray(const BitArray& other);
        BitArray(BitArray&& other);

        /**
         * @brief Sets the selected bit.
         * If `bit` is larger than the available amount
         * of bits, the container is automatically reallocated.
         * 
         * @param bit bit to set
         * 
         * @throws std::bad_alloc on reallocation failure
         */
        void set(const u64 bit);

        /**
         * @brief Clears he selected bit.
         * If `bit` is larger than the available amount
         * of bits, the container is automatically reallocated.
         * 
         * @param bit bit to clear
         * 
         * @throws std::bad_alloc on reallocation failure
         */
        void clear(const u64 bit);

        /**
         * @brief Checks if `bit`'s bit is set. If `bit`
         * is larger than the number of bits used,
         * it does NOT throw an exception, but always returns false.
         * 
         * @param bit bit
         * 
         * @return whether `bit` is set
         */
        bool operator[](const u64 bit) { return this->at(bit); }

        /**
         * @brief Checks if `bit`'s bit is set. If `bit`
         * is larger than the number of bits, returns false.
         * 
         * @param bit bit
         * 
         * @return whether `bit` is set
         */
        bool at(const u64 bit);

        /**
         * @brief Get size of the bit array.
         * 
         * @return size of the bit array in bits (!!!)
         */
        u64 size() const { return this->numberOfBits; }
        /**
         * @brief Get capacity of the bit array.
         * 
         * @return capacity of the bit array in bits (!!!)
         * 
         * The capacity will always be a power of 2.
         */
        u64 capacity() const { return this->numberOfBitsAvailable; }
};