#pragma once

#include "Bindings.h"

#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>

/**
 * @brief A dynamic array that automatically stays sorted
 * on every insertion/deletion.
 * 
 * For now it doesn't have either iterators or
 * a modifiable allocator (uses malloc/free) - that will eventually change.
 * 
 * You should use SortedVector instead of this as it's better coded
 * than whatever the hell is here.
 * 
 * @tparam T 
 */
template<typename T, size_t staticCapacity = 0> class SortedArray {
    private:
        size_t __size = 0;
        size_t __capacity = staticCapacity;
        std::array<T, staticCapacity> staticData;
        T* __data = nullptr;

        void __allocIfDynamicCapacity0() {
            if constexpr(staticCapacity == 0) {
                this->__data = static_cast<T*>(malloc(sizeof(T)));
                if(this->__data == nullptr) throw std::bad_alloc();
                this->__capacity = 1;
            }
            else {
                this->__data = static_cast<T*>(malloc(sizeof(T) * this->__capacity));
                if(this->__data == nullptr) throw std::bad_alloc();
                this->__capacity *= 2;
            }
        }

        void __realloc() {
            T* newData = nullptr;
            if constexpr(!std::is_trivially_move_constructible_v<T>) {
                newData = static_cast<T*>(malloc(sizeof(T) * (this->__capacity * 2 - staticCapacity)));
                if(newData == nullptr) throw std::bad_alloc();
                for(size_t i = 0; i < this->__size - staticCapacity; i++) {
                    new (&newData[i]) T(std::move(this->__data[i]));
                    this->__data[i].~T();
                }
                free(this->__data);
            }
            else {
                newData = static_cast<T*>(realloc(this->__data, sizeof(T) * this->__capacity * 2 - staticCapacity));
                if(newData == nullptr) throw std::bad_alloc();
            }
            this->__data = newData;
            this->__capacity = this->__capacity * 2;
        }

        void __allocIfNeeded() {
            if constexpr(staticCapacity == 0) {
                if(this->__capacity == 0) return this->__allocIfDynamicCapacity0();
                if(this->__size == this->__capacity) return this->__realloc();
            }
            else {
                if(this->__size == this->__capacity) {
                    if(this->__data == nullptr) return this->__allocIfDynamicCapacity0();
                    else return this->__realloc();
                }
            }
        }

        T& __keepSorted() {
            if(this->__size == 1) return at(0);
            if(at(this->__size - 1) >= at(this->__size - 2))
                return at(this->__size - 1);
            
            ssize_t left = 0, right = (ssize_t)this->__size - 2, mid;
            while(left < right) {
                mid = (left + right) / 2;
                if(at(mid) < at(this->__size - 1)) left = mid + 1;
                else right = mid - 1;
            }
            mid = (left + right) / 2;
            //for fixing an issue where mid is an index of the element behind the desired one
            if(at(mid) < at(this->__size - 1)) mid++; 

            //move the most recently added element out
            T copy(std::move(at(this->__size - 1)));
            //shift elements forward, starting from the second-to-last one up to mid
            for(ssize_t i = this->__size - 1; i > mid; i--) { at(i) = std::move(at(i - 1)); }
            //move the most recently added element back into appropiate place
            new (&at(mid)) T(std::move(copy));

            return at(mid);
        }
    public:
        explicit SortedArray() = default;
        explicit SortedArray(const size_t initialCapacity) 
         : __capacity(initialCapacity + staticCapacity), __data(static_cast<T*>(malloc(sizeof(T) * initialCapacity))) {
            if(!this->__data) throw std::bad_alloc(); //new[]/delete[] don't work, hence malloc
        }

        ~SortedArray() {
            if constexpr(!std::is_trivially_destructible_v<T>) {
                for(size_t i = staticCapacity; i < this->__size; i++) {
                    at(i).~T();
                }
            }
            free(this->__data);
        }

        SortedArray(const SortedArray&) = delete;
        SortedArray(SortedArray&& other)
         : __size(other.__size), __capacity(other.__capacity),
           staticData(std::move(other.staticData)), __data(other.__data) {
            other.__size = 0;
            other.__capacity = staticCapacity;
            other.__data = nullptr;
        }

        SortedArray& operator=(const SortedArray&) = delete;
        SortedArray& operator=(SortedArray&& other) {
            this->__size = other.__size;
            this->__capacity = other.__capacity;
            this->staticData = std::move(other.staticData);
            this->__data = other.__data;

            other.__size = 0;
            other.__capacity = staticCapacity;
            other.__data = nullptr;

            return *this;
        }

        /**
         * @brief Overloaded operator at.
         * 
         * @param index index
         * @return reference to the object stored at that index
         * @throws std::out_of_range if attempted accessing out of bounds
         */
        T& operator[](const size_t index) {
            if constexpr(staticCapacity == 0) return this->__data[index];
            else return index < staticCapacity ? this->staticData[index] : this->__data[index - staticCapacity];
        }

        T& at(const size_t index) {
            if(index >= this->__size) Unlikely throw std::out_of_range("Attempted access out of bounds");
            if constexpr(staticCapacity == 0) {
                return this->__data[index];
            }
            else {
                if(index < staticCapacity)
                    return this->staticData[index];
                else return this->__data[index - staticCapacity];
            }
        }

        /**
         * @brief Get the internal pointer to data stored.
         * 
         * Always returns the heap pointer if `staticCapacity == 0`.
         */
        T* data(bool ifStatic = false) const noexcept {
            if constexpr(staticCapacity == 0) {
                return this->__data;
            }
            else {
                if(ifStatic) return this->staticData.data();
                else return this->__data;
            }
        }
        /**
         * @brief Get the size of the array.
         */
        size_t size() const noexcept { return this->__size; }
        /**
         * @brief Get the capacity of the array.
         */
        size_t capacity() const noexcept { return this->__capacity; }

        /**
         * @brief Adds the copy of the element to the array.
         * 
         * There is no `insert` method since it doesn't make sense to insert
         * into a specified position in a supposedly always sorted array.
         * 
         * @param element element
         * @return reference to the added element
         * @throws std::bad_alloc if reallocation fails
         */
        T& push(const T& element) {
            this->__allocIfNeeded();
            new (&at(this->__size++)) T(element);
            return this->__keepSorted();
        }
        /**
         * @brief Moves the element into the array.
         * 
         * There is no `insert` method since it doesn't make sense to insert
         * into a specified position in a supposedly always sorted array.
         * @param element element
         * @return reference to the added element
         * @throws std::bad_alloc if reallocation fails
         */
        T& push(T&& element) {
            this->__allocIfNeeded();
            new (&at(this->__size++)) T(std::move(element));
            return this->__keepSorted();
        }

        /**
         * @brief Construct a new element in the array.
         * 
         * There is no `insert` method since it doesn't make sense to insert
         * into a specified position in a supposedly always sorted array.
         * @tparam Args constructor arguments
         * @return reference to the added element
         * @throws std::bad_alloc if reallocation fails
         */
        template<class...Args> T& emplace(Args...args) {
            this->__allocIfNeeded();
            new (&at(this->__size++)) T(args...);
            return this->__keepSorted();
        }

        /**
         * @brief Removes and returns the last element in the array.
         * 
         * @return last element that was stored
         * @throws std::underflow_error if attempted pop on empty array
         */
        T pop() {
            if(this->__size == 0) Unlikely throw std::underflow_error("Cannot pop empty array");
            return T(std::move(at(--this->__size)));
        }

        /**
         * @brief Removes and returns the element at given index.
         * 
         * @param index index
         * @return element that was stored at index `index`
         * @throws std::out_of_range if `index >= size`
         */
        T remove(size_t index) {
            if(index >= this->__size) throw std::out_of_range("Attempted removal out of range");

            T t(std::move(at(index)));
            for(size_t i = index; i < this->__size - 1; i++) {
                at(i) = std::move(at(i + 1));
            }
            this->__size--;

            return t;
        }

        /**
         * @brief Erase the element at given index.
         * 
         * @param index index
         * @throws std::out_of_range if `index >= size`
         */
        void erase(size_t index) {
            T t = this->remove(index);
            t.~T();
        }

        /**
         * @brief Perform a function on every element.
         * 
         * Iterators are not yet implemented, so use this instead.
         * 
         * @tparam F 
         * @param func 
         */
        template<typename F> void forEach(F func) {
            for(size_t i = 0; i < this->__size; i++) {
                func(at(i));
            }
        }

        /**
         * @brief Get element that matches the comparison
         * function passed. 
         * 
         * @tparam F comparison function which:
         * 
         * - takes a T&
         * 
         * - returns 0 for equality
         * 
         * - returns a positive `int` if T& passed is ordered after searched element
         * 
         * - returns a negative `int` if T& passed is ordered before searched element
         * @return pointer to the element or nullptr if not found.
         * 
         * Note that if the array contains duplicate elements,
         * the one returned is arbitrary.
         */
        template<typename F> T* find(F compare) {
            if(this->__size == 0) return nullptr;
            else if(this->__size == 1) return &at(0);
            else {
                ssize_t left = 0, right = (ssize_t)this->__size - 1, mid;
                while(left < right) {
                    mid = (left + right) / 2;
                    if(compare(at(mid)) < 0) left = mid + 1;
                    else right = mid - 1;
                }
                mid = (left + right) / 2;
                if(compare(at(mid)) != 0) return nullptr;
                else return &at(mid);
            }
        }

};