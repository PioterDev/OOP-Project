#pragma once

#include "Bindings.h"

#include <stdexcept>

/**
 * @brief A bit array implementation as an alternative to
 * std::vector<bool> (this thing is...not great).
 * 
 * @throws std::bad_alloc if it fails to (re)allocate memory
 */
class BitArray {
    private:
        uint64_t numberOfBits;
        uint64_t numberOfBitsAvailable;
        uint8_t* bits = nullptr;

        ForceInline uint64_t roundUpToPowerOf2(uint64_t in) {
            uint64_t out = in;
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

        bool reallocate(uint64_t minimumNewSize);
    public:
        /**
         * @brief Constructs a BitArray.
         * 
         * @param initialSize number of bits(!!!) initially allocated
         * 
         * @throws std::bad_alloc on allocation failure
         */
        BitArray(uint64_t initialSize);
        
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
        void set(uint64_t bit);

        /**
         * @brief Clears he selected bit.
         * If `bit` is larger than the available amount
         * of bits, the container is automatically reallocated.
         * 
         * @param bit bit to clear
         * 
         * @throws std::bad_alloc on reallocation failure
         */
        void clear(uint64_t bit);

        /**
         * @brief Checks if `bit`'s bit is set. If `bit`
         * is larger than the number of bits used, returns false.
         * 
         * @param bit bit
         * 
         * @return whether `bit` is set
         */
        bool operator[](uint64_t bit) { return at(bit); }

        /**
         * @brief Checks if `bit`'s bit is set. If `bit`
         * is larger than the number of bits, returns false.
         * 
         * @param bit bit
         * 
         * @return whether `bit` is set
         */
        bool at(uint64_t bit);

        /**
         * @brief Get size of the bit array.
         * 
         * @return size of the bit array in bits (!!!)
         */
        uint64_t size() const { return this->numberOfBits; }
        /**
         * @brief Get capacity of the bit array.
         * 
         * @return capacity of the bit array in bits (!!!)
         * 
         * The capacity will always be a power of 2.
         */
        uint64_t capacity() const { return this->numberOfBitsAvailable; }
};