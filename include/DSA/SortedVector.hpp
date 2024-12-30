#pragma once

#include "DSA/Vector.hpp"

template<typename T, size_t staticCapacity = 0, bool moveOnReallocation = true>
class SortedVector : public Vector<T, staticCapacity, moveOnReallocation> {
    private:
        using Vector<T, staticCapacity, moveOnReallocation>::append;
        using Vector<T, staticCapacity, moveOnReallocation>::emplaceBack;
        using Vector<T, staticCapacity, moveOnReallocation>::insert;
        using Vector<T, staticCapacity, moveOnReallocation>::emplace;
        using Vector<T, staticCapacity, moveOnReallocation>::replace;
        using Vector<T, staticCapacity, moveOnReallocation>::__at;

        ForceInline T& __insertionSortStep(ssize_t j) {
            T key(std::move(this->__at(j)));
            ssize_t i = j - 1;
            while(i >= 0 && this->__at(i) > key) {
                new (&this->__at(i + 1)) T(std::move(this->__at(i)));
                --i;
            }
            return *new (&this->__at(i + 1)) T(std::move(key));
        }

        void insertionSort(ssize_t left, ssize_t right) {
            for(size_t j = left; j < right; ++j) { this->__insertionSortStep(j); }
        }

        ssize_t __partition(ssize_t left, ssize_t right) {
            ssize_t mid = (left + right) / 2, pivotIndex;

            if((__at(left) < __at(right)) ^ (__at(left) < __at(mid))) pivotIndex = left;
            else if((__at(left) > __at(right)) ^ (__at(mid) > __at(right))) pivotIndex = right;
            else pivotIndex = mid;

            std::swap(__at(pivotIndex), __at(right));

            ssize_t i = left;
            for(ssize_t j = left; j < right; j++) {
                if(__at(j) < __at(right)) std::swap(__at(i), __at(j));
                ++i;
            }

            std::swap(__at(i), __at(right));

            return i;
        }

        void quickSort(ssize_t left, ssize_t right) {
            if(left < right) {
                ssize_t i = __partition(left, right);
                quickSort(left, i - 1);
                quickSort(i + 1, right);
            }
        }

        void __heapify(const size_t n, size_t i) {
            size_t largest, left, right;
            while(true) {
                largest = i;
                left = 2 * i;
                right = 2 * i + 1;

                if(left < n && __at(i) < __at(left)) largest = left;
                if(right < n && __at(largest) < __at(right)) largest = right;

                if(largest != i) {
                    std::swap(__at(i), __at(largest));
                    i = largest;
                }
                else break;
            }
        }

        void heapSort(const size_t n) {
            for(size_t j = this->__size / 2; j >= 1; --j) {
                __heapify(this->__size, j);
            }
            __heapify(0);
            for(size_t j = this->__size - 1; j > 0; j--) {
                std::swap(__at(0), __at(j));
                __heapify(j, 0);
            }
        }

        void introsort(ssize_t left, ssize_t right, short depth) {
            if(right - left <= 16) insertionSort(left, right);
            else if(depth == 0) heapSort(right - left + 1);
            else {
                ssize_t i = __partition(left, right);
                introsort(left, i - 1, depth - 1);
                introsort(i + 1, right, depth - 1);
            }
        }
    public:
        typedef Vector<T, staticCapacity, moveOnReallocation> _Vector;
        SortedVector() = default;
        explicit SortedVector(const size_t capacity) : _Vector(capacity) {}
        // explicit SortedVector(Vector&& other) :
        ~SortedVector() = default;
        SortedVector(const SortedVector& other) : _Vector(other) {}
        SortedVector(SortedVector&& other) noexcept : _Vector(std::move(other)) {}
        SortedVector& operator=(const SortedVector& other) {
            if(this == &other) return *this;
            _Vector::operator=(other);

            return *this;
        }
        SortedVector& operator=(SortedVector&& other) {
            if(this == &other) return *this;
            _Vector::operator=(std::move(other));

            return *this;
        }

        SortedVector(const _Vector& other) : _Vector(other) { introsort(0, this->__size - 1, 16); }
        SortedVector(_Vector&& other) noexcept : _Vector(std::move(other)) { introsort(0, this->__size - 1, 16); }
        SortedVector& operator=(const _Vector& other) {
            if(this == &other) return *this;
            _Vector::operator=(other);
            introsort(0, this->__size - 1, 16);

            return *this;
        }
        SortedVector& operator=(_Vector&& other) {
            if(this == &other) return *this;
            _Vector::operator=(std::move(other));
            introsort(0, this->__size - 1, 16);

            return *this;
        }

        /**
         * @brief Add a copy of the element into this
         * sorted vector.
         * 
         * @param element element to copy 
         * @return reference to the added element
         */
        T& add(const T& element) {
            this->append(element);
            return __insertionSortStep(this->__size - 1);
        }
        /**
         * @brief Move the element into this
         * sorted vector.
         * 
         * @param element element to move
         * @return reference to the added element
         */
        T& add(T&& element) {
            this->append(std::move(element));
            return __insertionSortStep(this->__size - 1);
        }

        /**
         * @brief Construct a new element in-place in this
         * sorted vector.
         * 
         * @tparam Args constructor arguments
         * @param args constructor arguments
         * @return reference to the added element
         */
        template<class...Args> T& addInPlace(Args&&...args) {
            this->emplaceBack(std::move(args)...);
            return __insertionSortStep(this->__size - 1);
        }

        /**
         * @brief Get a pointer to the element that
         * matches the comparison function passed. 
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
        template<typename F> T* find(F compare) noexcept {
            if(this->__size == 0) return nullptr;
            else if(this->__size == 1) return !compare(__at(0)) ? &__at(0) : nullptr;
            else {
                ssize_t left = 0, right = (ssize_t)this->__size - 1, mid;
                while(left < right) {
                    mid = (left + right) / 2;
                    if(compare(__at(mid)) < 0) left = mid + 1;
                    else right = mid - 1;
                }
                mid = (left + right) / 2;
                if(compare(__at(mid)) != 0) return nullptr;
                else return &__at(mid);
            }
        }

        /**
         * @brief Get a pointer to the element that
         * is the same as the one passed (in terms of comparison operators).
         * 
         * @param other an element to compare against
         * @return pointer to the element or nullptr if not found.
         * 
         * Note that if the array contains duplicate elements,
         * the one returned is arbitrary.
         */
        T* find(const T& other) const noexcept {
            if(this->__size == 0) return nullptr;
            else if(this->__size == 1) return other == __at(0) ? &(T&)__at(0) : nullptr;
            else {
                ssize_t left = 0, right = (ssize_t)this->__size - 1, mid;
                while(left < right) {
                    mid = (left + right) / 2;
                    if(__at(mid) < other) left = mid + 1;
                    else right = mid - 1;
                }
                mid = (left + right) / 2;
                if(__at(mid) != other) return nullptr;
                else return &(T&)__at(mid);
            }
        }

};