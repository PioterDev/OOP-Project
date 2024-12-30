#include "DSA/BitArray.hpp"
#include "DSA/ListArray.hpp"


BitArray::BitArray(const u64 initialSize) {
    u64 numberOfBytes = initialSize / 8 + (initialSize % 8 == 0 ? 0 : 1);
    if(numberOfBytes == 0) numberOfBytes = 1; //edge case
    this->bits = (u8*)calloc(numberOfBytes, 1);
    if(this->bits == nullptr) Unlikely throw std::bad_alloc();
    this->numberOfBitsAvailable = numberOfBytes * 8;
}

BitArray::BitArray(const BitArray& other)
: numberOfBits(other.numberOfBits),
  numberOfBitsAvailable(other.numberOfBitsAvailable)
{
    u64 numberOfBytes = other.numberOfBitsAvailable / 8 + (other.numberOfBitsAvailable % 8 == 0 ? 0 : 1);
    if(numberOfBytes == 0) numberOfBytes = 1; //edge case
    this->bits = (u8*)calloc(numberOfBytes, 1);
    if(this->bits == nullptr) throw std::bad_alloc();
}

BitArray::BitArray(BitArray&& other)
: numberOfBits(other.numberOfBits),
  numberOfBitsAvailable(other.numberOfBitsAvailable),
  bits(other.bits)
{
    other.bits = nullptr;
    other.numberOfBits = other.numberOfBitsAvailable = 0;
}

void BitArray::set(const u64 bit) {
    if(bit >= this->numberOfBitsAvailable) {
        if(this->reallocate(bit + 1)) Unlikely throw std::bad_alloc();
    }
    else if(bit >= this->numberOfBits) this->numberOfBits = bit + 1;
    this->bits[bit / 8] |= (1 << (bit % 8));
}

void BitArray::clear(const u64 bit) {
    if(bit >= this->numberOfBitsAvailable) {
        if(reallocate(bit + 1)) Unlikely throw std::bad_alloc();
    }
    else if(bit >= this->numberOfBits) this->numberOfBits = bit + 1;
    this->bits[bit / 8] &= ~(1 << (bit % 8));
}


bool BitArray::at(const u64 bit) {
    if(bit >= this->numberOfBits) return false;
    return (this->bits[bit / 8] & (1 << (bit % 8))) != 0;
}

bool BitArray::reallocate(const u64 minimumNewSize) {
    u64 newSize = roundUpToPowerOf2(minimumNewSize / 8 + (minimumNewSize % 8 == 0 ? 0 : 1));
    u8* newBits = (u8*)calloc(newSize, 1);
    if(newBits == nullptr) return true;
    memcpy(newBits, this->bits, this->numberOfBits / 8 + (this->numberOfBits % 8 == 0 ? 0 : 1));
    free(this->bits);
    this->bits = newBits;
    this->numberOfBits = minimumNewSize;
    this->numberOfBitsAvailable = newSize * 8;
    return false;
}