#pragma once

#include "Bindings.h"

#include <stdexcept>

#include "deus.hpp"

/**
 * @brief A bit array implementation as an alternative to
 * std::vector<bool> (this thing is...not great).
 * 
 * @throws std::bad_alloc if it fails to (re)allocate memory
 */
class BitArray {
    private:
        u64 numberOfBits;
        u64 numberOfBitsAvailable;
        u8* bits = nullptr;

        ForceInline u64 roundUpToPowerOf2(u64 in) {
            u64 out = in;
            out--;
            out |= out >> 1;
            out |= out >> 2;
            out |= out >> 4;
            out |= out >> 8;
            out |= out >> 16;
            out |= out >> 32;
            out++;
            return out;
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
        BitArray(u64 initialSize);
        
        ~BitArray() {
            free(this->bits);
        }

        /**
         * @brief Sets the selected bit.
         * If `bit` is larger than the available amount
         * of bits, the container is automatically reallocated.
         * 
         * @param bit bit to set
         * 
         * @throws std::bad_alloc on reallocation failure
         */
        void set(u64 bit);

        /**
         * @brief Clears he selected bit.
         * If `bit` is larger than the available amount
         * of bits, the container is automatically reallocated.
         * 
         * @param bit bit to clear
         * 
         * @throws std::bad_alloc on reallocation failure
         */
        void clear(u64 bit);

        /**
         * @brief Checks if `bit`'s bit is set. If `bit`
         * is larger than the number of bits used, returns false.
         * 
         * @param bit bit
         * 
         * @return whether `bit` is set
         */
        bool operator[](u64 bit) { return at(bit); }

        /**
         * @brief Checks if `bit`'s bit is set. If `bit`
         * is larger than the number of bits, returns false.
         * 
         * @param bit bit
         * 
         * @return whether `bit` is set
         */
        bool at(u64 bit);

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