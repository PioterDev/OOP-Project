#include "DSA/BitArray.hpp"
#include "DSA/ListArray.hpp"


BitArray::BitArray(u64 initialSize) {
    u64 numberOfBytes = initialSize / 8 + (initialSize % 8 == 0 ? 0 : 1);
    if(numberOfBytes == 0) numberOfBytes = 1; //edge case
    this->bits = (u8*)calloc(numberOfBytes, 1);
    if(this->bits == nullptr) Unlikely throw std::bad_alloc();
    this->numberOfBits = 0;
    this->numberOfBitsAvailable = numberOfBytes * 8;
}

void BitArray::set(u64 bit) {
    if(bit >= this->numberOfBitsAvailable) {
        if(this->reallocate(bit + 1)) Unlikely throw std::bad_alloc();
    }
    else if(bit >= this->numberOfBits) this->numberOfBits = bit + 1;
    this->bits[bit / 8] |= (1 << (bit % 8));
}

void BitArray::clear(u64 bit) {
    if(bit >= this->numberOfBitsAvailable) {
        if(reallocate(bit + 1)) Unlikely throw std::bad_alloc();
    }
    else if(bit >= this->numberOfBits) this->numberOfBits = bit + 1;
    this->bits[bit / 8] &= ~(1 << (bit % 8));
}


bool BitArray::at(u64 bit) {
    if(bit >= this->numberOfBits) return false;
    return (this->bits[bit / 8] & (1 << (bit % 8))) != 0;
}

bool BitArray::reallocate(u64 minimumNewSize) {
    u64 newSize = this->roundUpToPowerOf2(minimumNewSize / 8 + (minimumNewSize % 8 == 0 ? 0 : 1));
    u8* newBits = (u8*)calloc(newSize, 1);
    if(newBits == nullptr) return true;
    memcpy(newBits, this->bits, this->numberOfBits / 8 + (this->numberOfBits % 8 == 0 ? 0 : 1));
    free(this->bits);
    this->bits = newBits;
    this->numberOfBits = minimumNewSize;
    this->numberOfBitsAvailable = newSize * 8;
    return false;
}