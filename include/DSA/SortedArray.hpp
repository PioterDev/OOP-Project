#pragma once

#include "Bindings.h"

#include <stdexcept>


/**
 * @brief Do not used for now, contains quite a few bugs.
 * 
 * @tparam T 
 */
template<typename T> class SortedArray {
    private:
        T* data = nullptr;
        size_t size = 0;
        size_t capacity = 0;

        void keepSorted() {
            if(this->size < 2) return;

            ssize_t left = (ssize_t)0, right = this->size - 2, mid;
            while(left <= right) {
                mid = left + (right - left) / 2;
                if(this->data[mid] > this->data[this->size - 1]) {
                    right = mid - 1;
                }
                else {
                    left = mid + 1;
                }
            }

            T copy = this->data[this->size - 1];
            memmove(
                (void*)&this->data[mid + 1],
                (void*)&this->data[mid],
                sizeof(T) * (this->size - 1 - mid)
            );
            this->data[mid] = copy;
        }
    public:
        SortedArray() {}

        SortedArray(const size_t initialCapacity, const bool zeroInit) : capacity(initialCapacity) {
            if(zeroInit) this->data = (T*)calloc(initialCapacity, sizeof(T));
            else this->data = (T*)malloc(initialCapacity * sizeof(T));
            if(this->data == nullptr) Unlikely throw std::bad_alloc();
        }

        ~SortedArray() {
            free(data);
        }

        /**
         * @brief Overloaded operator at.
         * 
         * @param index index
         * @return reference to the object stored at that index
         * @throws std::out_of_range if attempted accessing out of bounds
         */
        T& operator[](const size_t index) {
            if(index >= size) Unlikely throw std::out_of_range("Dupa sraka");
            return data[index];
        }

        /**
         * @brief Adds the element to the array.
         * 
         * @param element element
         */
        void push(const T& element) {
            if(this->size == this->capacity) {
                T* newData = (T*)realloc(this->data, this->capacity * 2 * sizeof(T));
                if(newData == nullptr) Unlikely throw std::bad_alloc();
                this->data = newData;
                this->capacity *= (size_t)2;
            }

            this->data[this->size] = element;
            this->size++;
            this->keepSorted();
        }
        /**
         * @brief Adds the element to the array.
         * 
         * @param element element
         */
        void push(const T element) {
            if(this->size == this->capacity) {
                T* newData = (T*)realloc(this->data, this->capacity * 2 * sizeof(T));
                if(newData == nullptr) Unlikely throw std::bad_alloc();
                this->data = newData;
                this->capacity *= (size_t)2;
            }

            this->data[this->size] = element;
            this->size++;
            this->keepSorted();
        }

        /**
         * @brief Removes and returns the last element in the array.
         * 
         * @return copy of the object
         * @throws nullptr if attempted pop on empty array
         */
        T pop() {
            if(this->size == 0) Unlikely throw nullptr;
            return this->data[--this->size];
        }

        template<typename F> void forEach(F func) {
            for(size_t i = 0; i < this->size; i++) {
                func(&this->data[i]);
            }
        }
};