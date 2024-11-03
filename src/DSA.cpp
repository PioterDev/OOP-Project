#include "BitArray.hpp"
#include "ListArray.hpp"



BitArray::BitArray(uint64_t initialSize) {
    uint64_t numberOfBytes = initialSize / 8 + (initialSize % 8 == 0 ? 0 : 1);
    if(numberOfBytes == 0) numberOfBytes = 1; //edge case
    this->bits = (uint8_t*)calloc(numberOfBytes, 1);
    if(this->bits == nullptr) throw std::bad_alloc();
    this->numberOfBits = 0;
    this->numberOfBitsAvailable = numberOfBytes * 8;
}

void BitArray::set(uint64_t bit) {
    if(bit >= this->numberOfBitsAvailable) {
        if(this->reallocate(bit + 1)) throw std::bad_alloc();
    }
    else if(bit >= this->numberOfBits) this->numberOfBits = bit + 1;
    this->bits[bit / 8] |= (1 << (bit % 8));
}

void BitArray::clear(uint64_t bit) {
    if(bit >= this->numberOfBitsAvailable) {
        if(reallocate(bit + 1)) throw std::bad_alloc();
    }
    else if(bit >= this->numberOfBits) this->numberOfBits = bit + 1;
    this->bits[bit / 8] &= ~(1 << (bit % 8));
}


bool BitArray::at(uint64_t bit) {
    if(bit >= this->numberOfBits) return false;
    return (this->bits[bit / 8] & (1 << (bit % 8))) != 0;
}

bool BitArray::reallocate(uint64_t minimumNewSize) {
    uint64_t newSize = this->roundUpToPowerOf2(minimumNewSize / 8 + (minimumNewSize % 8 == 0 ? 0 : 1));
    uint8_t* newBits = (uint8_t*)calloc(newSize, 1);
    if(newBits == nullptr) return true;
    memcpy(newBits, this->bits, this->numberOfBits / 8 + (this->numberOfBits % 8 == 0 ? 0 : 1));
    free(this->bits);
    this->bits = newBits;
    this->numberOfBits = minimumNewSize;
    this->numberOfBitsAvailable = newSize * 8;
    return false;
}