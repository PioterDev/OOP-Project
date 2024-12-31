#pragma once

#include "Bindings.h"

#include <array>
#include <cstdlib>
#include <stdexcept>

/**
 * @brief An alternative implementation of a dynamic array, provided
 * by me, PioterDev - the creator of this codebase.
 * 
 * For now it doesn't support
 * arbitrary iteration (you can't do `iterator += 2` for example),
 * custom allocators (uses `malloc` underneath - yes yes this is C++, but 
 * calling raw `operator new` caused some issues, hence `malloc`).
 * It also doesn't provide strong exception safety unlike `std::vector`,
 * for now at least.
 * 
 * @tparam T type
 * @tparam staticCapacity how much space should be reserved right inside
 * this structure itself instead of the heap, useful when a dynamic array
 * is needed, but you don't necessarily want to use the heap. Defaults to 0.
 * @tparam moveOnReallocation whether to move objects during reallocation
 * or to copy them, defaults to true. This is a protective measure (somewhat)
 * for exception safety, but the vector itself is not fully immune to them.
 * It also enforces the usage of `noexcept`.
 */
template<typename T, size_t staticCapacity = 0, bool moveOnReallocation = true>
class Vector {
    static_assert(
        moveOnReallocation ? (std::is_nothrow_move_constructible_v<T> && moveOnReallocation) : true,
        "Move-on-reallocation specified, but type provided "
        "does not have a noexcept move constructor."
    );
    protected:
        size_t __size = 0;
        size_t __capacity = staticCapacity;
        T* __data = nullptr;
        std::array<T, staticCapacity> staticData;

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

        void __expand(float factor) {
            assert(factor >= 1.0f);
            size_t newCapacity = (size_t)(factor * (float)this->__capacity);
            T* newData = nullptr;
            if constexpr(!std::is_trivially_move_constructible_v<T>) {
                newData = static_cast<T*>(malloc(sizeof(T) * (newCapacity - staticCapacity)));
                if(newData == nullptr) throw std::bad_alloc();
                for(size_t i = 0; i < this->__size - staticCapacity; i++) {
                    if constexpr(moveOnReallocation) new (&newData[i]) T(std::move(this->__data[i]));
                    else new (&newData[i]) T(this->__data[i]);
                    this->__data[i].~T();
                }
                free(this->__data);
            }
            else {
                newData = static_cast<T*>(realloc(this->__data, sizeof(T) * (newCapacity - staticCapacity)));
                if(newData == nullptr) throw std::bad_alloc();
            }
            this->__data = newData;
            this->__capacity = newCapacity;
        }

        void __allocIfNeeded() {
            if constexpr(staticCapacity == 0) {
                if(this->__capacity == 0) return this->__allocIfDynamicCapacity0();
                if(this->__size == this->__capacity) return this->__expand(2.0f);
            }
            else {
                if(this->__size == this->__capacity) {
                    if(this->__data == nullptr) return this->__allocIfDynamicCapacity0();
                    else return this->__expand(2.0f);
                }
            }
        }

        void __shiftBy1Forward(const size_t index) {
            if constexpr(std::is_trivially_move_constructible_v<T>) {
                if(index < staticCapacity) {
                    memmove(&this->__data[1], &this->__data[0], sizeof(T) * (this->__size - staticCapacity));
                    memcpy(&this->__data[0], &this->staticData.back(), sizeof(T));
                    if(((ssize_t)(staticCapacity) - (ssize_t)1 - (ssize_t)index) > 0) {
                        memmove(
                            &this->staticData[index + 1],
                            &this->staticData[index],
                            sizeof(T) * (staticCapacity - 1 - index)
                        );
                    }
                }
                else {
                    memmove(
                        &this->__data[index + 1 - staticCapacity],
                        &this->__data[index - staticCapacity],
                        sizeof(T) * (this->__size - index)
                    );
                }
            }
            else {
                for(ssize_t i = (ssize_t)this->__size; i > (ssize_t)index; i--) {
                    new (&__at(i)) T(std::move(__at(i - 1)));
                }
            }
        }

        void __shiftBy1Backward(const size_t index) {
            if constexpr(std::is_trivially_move_constructible_v<T>) {
                if(index < staticCapacity) {
                    if(((ssize_t)(staticCapacity) - (ssize_t)1 - (ssize_t)index) > 0) {
                        memmove(
                            &this->staticData[index],
                            &this->staticData[index + 1],
                            sizeof(T) * (staticCapacity - 1 - index)
                        );
                    }
                    memcpy(&this->__data[0], &this->staticData.back(), sizeof(T));
                    memmove(&this->__data[0], &this->__data[1], sizeof(T) * (this->__size - staticCapacity));
                }
                else {
                    memmove(
                        &this->__data[index - staticCapacity],
                        &this->__data[index + 1 - staticCapacity],
                        sizeof(T) * (this->__size - index)
                    );
                }
            }
            else {
                for(ssize_t i = (ssize_t)index; i < this->__size - 1; i++) {
                    if constexpr(moveOnReallocation) new (&__at(i)) T(std::move(__at(i + 1)));
                    else new (&__at(i)) T(__at(i + 1));
                }
            }
        }

        T& __at(const size_t index) noexcept {
            if constexpr(staticCapacity == 0) return this->__data[index];
            else return index < staticCapacity ? this->staticData[index] : this->__data[index - staticCapacity];
        }

        const T& __at(const size_t index) const noexcept {
            if constexpr(staticCapacity == 0) return this->__data[index];
            else return index < staticCapacity ? this->staticData[index] : this->__data[index - staticCapacity];
        }

    public: //iterators
        class Iterator {
            friend class Vector;
            private:
                T* staticPtr = nullptr;
                const T* staticPtrEnd = nullptr;
                T* heapPtr = nullptr;
                
                explicit Iterator(T* staticPtr, const T* staticPtrEnd, T* heapPtr)
                 : staticPtr(staticPtr), staticPtrEnd(staticPtrEnd), heapPtr(heapPtr) {}
            public:
                ~Iterator() = default;
                
                T& operator*() const {
                    if constexpr(staticCapacity == 0) return *heapPtr;
                    else return *(staticPtr >= staticPtrEnd ? heapPtr : staticPtr);
                }
                T* operator->() const {
                    if constexpr(staticCapacity == 0) return heapPtr;
                    else return staticPtr >= staticPtrEnd ? heapPtr : staticPtr;
                }

                Iterator& operator++() noexcept {
                    if constexpr(staticCapacity == 0) ++heapPtr;
                    else staticPtr >= staticPtrEnd ? ++heapPtr : ++staticPtr;
                    return *this;
                }
                Iterator operator++(int) noexcept { Iterator tmp = *this; ++(*this); return tmp; }
                
                bool operator==(const Iterator& other) const noexcept {
                    if constexpr(staticCapacity == 0) return heapPtr == other.heapPtr;
                    else return staticPtr == other.staticPtr || heapPtr == other.heapPtr;
                }
                bool operator!=(const Iterator& other) const noexcept {
                    if constexpr(staticCapacity == 0) return heapPtr != other.heapPtr;
                    else return staticPtr != other.staticPtr && heapPtr != other.heapPtr;
                }

        };
        class ReverseIterator {
            friend class Vector;
            private:
                T* staticPtr = nullptr;
                T* heapPtr = nullptr;
                const T* heapPtrEnd = nullptr;
                
                explicit ReverseIterator(T* staticPtr, T* heapPtr, const T* heapPtrEnd)
                 : staticPtr(staticPtr), heapPtr(heapPtr), heapPtrEnd(heapPtrEnd) {}
            public:
                ~ReverseIterator() = default;
                
                T& operator*() const {
                    if constexpr(staticCapacity == 0) return *heapPtr;
                    else return *(heapPtr <= heapPtrEnd ? staticPtr : heapPtr);
                }
                T* operator->() const {
                    if constexpr(staticCapacity == 0) return heapPtr;
                    else return heapPtr <= heapPtrEnd ? staticPtr : heapPtr;
                }

                ReverseIterator& operator--() noexcept {
                    if constexpr(staticCapacity == 0) --heapPtr;
                    else heapPtr <= heapPtrEnd ? --staticPtr : --heapPtr;
                    return *this;
                }
                ReverseIterator operator--(int) noexcept { ReverseIterator tmp = *this; --(*this); return tmp; }
                
                bool operator==(const ReverseIterator& other) const noexcept {
                    if constexpr(staticCapacity == 0) return heapPtr == other.heapPtr;
                    else return staticPtr == other.staticPtr || heapPtr == other.heapPtr;
                }
                bool operator!=(const ReverseIterator& other) const noexcept {
                    if constexpr(staticCapacity == 0) return heapPtr != other.heapPtr;
                    else return staticPtr != other.staticPtr && heapPtr != other.heapPtr;
                }

        };
        class ReadOnlyIterator {
            friend class Vector;
            private:
                const T* staticPtr = nullptr;
                const T* staticPtrEnd = nullptr;
                const T* heapPtr = nullptr;
                
                explicit ReadOnlyIterator(const T* staticPtr, const T* staticPtrEnd, const T* heapPtr)
                 : staticPtr(staticPtr), staticPtrEnd(staticPtrEnd), heapPtr(heapPtr) {}
            public:
                ~ReadOnlyIterator() = default;
                
                const T& operator*() const {
                    if constexpr(staticCapacity == 0) return *heapPtr;
                    else return *(staticPtr >= staticPtrEnd ? heapPtr : staticPtr);
                }
                const T* operator->() const {
                    if constexpr(staticCapacity == 0) return heapPtr;
                    else return staticPtr >= staticPtrEnd ? heapPtr : staticPtr;
                }

                ReadOnlyIterator& operator++() noexcept {
                    if constexpr(staticCapacity == 0) ++heapPtr;
                    else staticPtr >= staticPtrEnd ? ++heapPtr : ++staticPtr;
                    return *this;
                }
                ReadOnlyIterator operator++(int) noexcept { ReadOnlyIterator tmp = *this; ++(*this); return tmp; }
                
                bool operator==(const ReadOnlyIterator& other) const noexcept {
                    if constexpr(staticCapacity == 0) return heapPtr == other.heapPtr;
                    else return staticPtr == other.staticPtr && heapPtr == other.heapPtr;
                }
                bool operator!=(const ReadOnlyIterator& other) const noexcept {
                    if constexpr(staticCapacity == 0) return heapPtr != other.heapPtr;
                    else return staticPtr != other.staticPtr && heapPtr != other.heapPtr;
                }

        };
        class ReadOnlyReverseIterator {
            friend class Vector;
            private:
                const T* staticPtr = nullptr;
                const T* heapPtr = nullptr;
                const T* heapPtrEnd = nullptr;
                
                explicit ReadOnlyReverseIterator(const T* staticPtr, const T* heapPtr, const T* heapPtrEnd)
                 : staticPtr(staticPtr), heapPtr(heapPtr), heapPtrEnd(heapPtrEnd) {}
            public:
                ~ReadOnlyReverseIterator() = default;
                
                const T& operator*() const {
                    if constexpr(staticCapacity == 0) return *heapPtr;
                    else return *(heapPtr <= heapPtrEnd ? staticPtr : heapPtr);
                }
                const T* operator->() const {
                    if constexpr(staticCapacity == 0) return heapPtr;
                    else return heapPtr <= heapPtrEnd ? staticPtr : heapPtr;
                }

                ReadOnlyReverseIterator& operator--() noexcept {
                    if constexpr(staticCapacity == 0) --heapPtr;
                    else heapPtr <= heapPtrEnd ? --staticPtr : --heapPtr;
                    return *this;
                }
                ReadOnlyReverseIterator operator--(int) noexcept { ReadOnlyReverseIterator tmp = *this; --(*this); return tmp; }
                
                bool operator==(const ReadOnlyReverseIterator& other) const noexcept {
                    if constexpr(staticCapacity == 0) return heapPtr == other.heapPtr;
                    else return staticPtr == other.staticPtr || heapPtr == other.heapPtr;
                }
                bool operator!=(const ReadOnlyReverseIterator& other) const noexcept {
                    if constexpr(staticCapacity == 0) return heapPtr != other.heapPtr;
                    else return staticPtr != other.staticPtr && heapPtr != other.heapPtr;
                }

        };
        //yes, I really have whitespace
        /**
         * @brief Get a read/write forward iterator (not fully implemented).
         */
        Iterator begin() const noexcept {
            if constexpr(staticCapacity == 0) return Iterator{ __data, __data, __data };
            else return Iterator{ (T*)staticData.data(), &(T&)staticData.data()[__size > staticCapacity ? staticCapacity : __size], __data };
        }
        /**
         * @brief Get the read/write forward iterator marking the end of the container.
         */
        Iterator end() const noexcept {
            if constexpr(staticCapacity == 0) return Iterator{ __data, __data, &__data[__size] };
            else return Iterator{
                &(T&)staticData.data()[__size > staticCapacity ? staticCapacity : __size],
                &(T&)staticData.data()[__size > staticCapacity ? staticCapacity : __size],
                &__data[__size - staticCapacity] 
            };
        }
        /**
         * @brief Get a read/write backward iterator (not fully implemented).
         */
        ReverseIterator rbegin() const noexcept {
            if constexpr(staticCapacity == 0) return ReverseIterator{ __data, &__data[__size - 1], __data - 1 };
            else return ReverseIterator{
                &(T&)staticData.data()[(__size > staticCapacity ? staticCapacity : __size) - 1],
                __size > staticCapacity ? &__data[__size - 1 - staticCapacity] : nullptr,
                __size > staticCapacity ? __data - 1 : nullptr
            };
        }
        /**
         * @brief Get the read/write backward iterator marking the end of the container.
         */
        ReverseIterator rend() const noexcept {
            if constexpr(staticCapacity == 0) return ReverseIterator{ __data - 1, __data - 1, __data - 1};
            else return ReverseIterator{ (T*)staticData.data() - 1, __data - 1, __data - 1 };
        }
        /**
         * @brief Get a read-only forward iterator (not fully implemented).
         */
        ReadOnlyIterator cbegin() const noexcept {
            if constexpr(staticCapacity == 0) return ReadOnlyIterator{ __data, __data, __data };
            else return ReadOnlyIterator{ staticData.data(), &staticData.data()[__size > staticCapacity ? staticCapacity : __size], __data };
        }
        /**
         * @brief Get the read-only forward iterator marking the end of the container.
         */
        ReadOnlyIterator cend() const noexcept {
            if constexpr(staticCapacity == 0) return ReadOnlyIterator{__data, __data, &__data[__size] };
            else return ReadOnlyIterator{
                &staticData.data()[__size > staticCapacity ? staticCapacity : __size],
                &staticData.data()[__size > staticCapacity ? staticCapacity : __size],
                &__data[__size - staticCapacity]
            };
        }
        /**
         * @brief Get a read-only backward iterator (not fully implemented).
         */
        ReadOnlyReverseIterator crbegin() const noexcept {
            if constexpr(staticCapacity == 0) return ReadOnlyReverseIterator{ __data, &__data[__size - 1], __data - 1 };
            else return ReadOnlyReverseIterator{
                &(T&)staticData.data()[(__size > staticCapacity ? staticCapacity : __size) - 1],
                __size > staticCapacity ? &__data[__size - 1 - staticCapacity] : nullptr,
                __size > staticCapacity ? __data - 1 : nullptr
            };
        }
        /**
         * @brief Get the read-only backward iterator marking the end of the container.
         */
        ReadOnlyReverseIterator crend() const noexcept {
            if constexpr(staticCapacity == 0) return ReadOnlyReverseIterator{__data - 1, __data - 1, __data - 1 };
            else return ReadOnlyReverseIterator{ staticData.data() - 1, __data - 1, __data - 1 };
        }
    public: //Constructors, operators
        Vector() = default;
        /**
         * @brief Construct a new Vector with dynamic capacity (on the heap)
         * `capacity`.
         * 
         * @param capacity initial dynamic capacity
         * 
         * If you specified `staticCapacity` in the template parameter
         * and don't want extra pre-allocation, use the default constructor.
         */
        explicit Vector(const size_t capacity)
         : __capacity(capacity + staticCapacity), __data(static_cast<T*>(malloc(sizeof(T) * capacity))), staticData() {
            if(!this->__data) throw std::bad_alloc();
        }

        ~Vector() noexcept {
            if constexpr(!std::is_trivially_destructible_v<T>) {
                for(ssize_t i = 0; i < (ssize_t)(this->__size) - (ssize_t)(staticCapacity); ++i) {
                    this->__data[i].~T();
                }
            }
            free(this->__data);
        }

        Vector(const Vector& other) : __size(other.__size), __capacity(other.__capacity), staticData(other.staticData) {
            if(this->__capacity > staticCapacity) {
                this->__data = static_cast<T*>(malloc(sizeof(T) * (this->__capacity - staticCapacity)));
                if(this->__data == nullptr) throw std::bad_alloc();
                if(this->__size > staticCapacity) {
                    if constexpr(std::is_trivially_copy_constructible_v<T>) {
                        memcpy(this->__data, other.__data, sizeof(T) * (this->__size - staticCapacity));
                    }
                    else {
                        for(size_t i = 0; i < this->__size - staticCapacity; i++) {
                            new (&this->__data[i]) T(other.__data[i]);
                        }
                    }
                }
            }
            else this->__data = nullptr;
        }
        Vector(Vector&& other) noexcept 
         : __size(other.__size), __capacity(other.__capacity),
           __data(other.__data), staticData(std::move(other.staticData)) {
            other.__size = 0;
            other.__capacity = staticCapacity;
            other.__data = nullptr;
        }

        Vector& operator=(const Vector& other) {
            if(this == &other) return *this;
            this->__size = other.__size;
            this->__capacity = other.__capacity;
            this->staticData = other.staticData;

            if(this->__capacity > staticCapacity) {
                this->__data = static_cast<T*>(malloc(sizeof(T) * (this->__capacity - staticCapacity)));
                if(this->__data == nullptr) throw std::bad_alloc();
                if(this->__size > staticCapacity) {
                    if constexpr(std::is_trivially_copyable_v<T>) {
                        memcpy(this->__data, other.__data, sizeof(T) * (this->__size - staticCapacity));
                    }
                    else {
                        for(size_t i = 0; i < this.__size - staticCapacity; i++) {
                            new (&this->__data[i]) T(other.__data[i]);
                        }
                    }
                }
            }
            else this->__data = nullptr;

            return *this;
        }
        Vector& operator=(Vector&& other) noexcept {
            if(this == &other) return *this;
            this->__size = other.__size;
            this->__capacity = other.__capacity;
            this->__data = other.__data;
            this->staticData = std::move(other.staticData);

            other.__size = 0;
            other.__capacity = staticCapacity;
            other.__data = nullptr;

            return *this;
        }

        const T& operator[](const size_t index) const noexcept { return __at(index); }
        T& operator[](const size_t index) noexcept { return __at(index); }
    public: //Simple functions
        /**
         * @brief Get a reference to the first element.
         * Alias for calling `at(0)`.
         */
        T& first() const { return this->at(0); }
        /**
         * @brief Get a reference to the first element.
         * Alias for calling `at(0)`.
         */
        T& front() const { return this->at(0); }
        /**
         * @brief Get a reference to the last element.
         * Alias for calling `at(size() - 1)`.
         */
        T& last() const { return this->at(this->__size - 1); }
        /**
         * @brief Get a reference to the last element.
         * Alias for calling `at(size() - 1)`.
         */
        T& back() const { return this->at(this->__size - 1); }
        /**
         * @brief Check whether the container is empty.
         * Alias for `size()` == 0.
         */
        bool empty() const noexcept { return this->__size == 0; }
    public: //Getter-like functions
        /**
         * @brief Get a reference to the element at index `index`.
         * 
         * @param index index
         * @return reference to element at index `index`
         * @throws std::out_of_range on attempted access out of bounds
         */
        T& at(const size_t index) {
            if(index > this->__size) Unlikely throw std::out_of_range("Attempted access out of bounds");
            if constexpr(staticCapacity == 0) return this->__data[index];
            else {
                if(index < staticCapacity) return this->staticData[index];
                else return this->__data[index - staticCapacity];
            }
        }

        const T& at(const size_t index) const {
            if(index > this->__size) Unlikely throw std::out_of_range("Attempted access out of bounds");
            if constexpr(staticCapacity == 0) return this->__data[index];
            else {
                if(index < staticCapacity) return this->staticData[index];
                else return this->__data[index - staticCapacity];
            }
        }

        /**
         * @brief Get the internal pointer to data stored.
         * 
         * Always returns the heap pointer if `staticCapacity == 0`.
         * 
         * If `ifStatic` and `staticCapacity > 0`, returns a pointer
         * to a static array inside. Accessing data at
         * `staticCapacity` and beyond is undefined behavior.
         * 
         * @return pointer to internal container, do not access out of bounds,
         * free manually or remove elements from it. The only safeish
         * operation is to modify/replace objects inside.
         * 
         * Be mindful that if `staticCapacity > 0` and `size < staticCapacity`
         * the pointer returned if `ifStatic` can be a dangling pointer.
         */
        T* data(const bool ifStatic = false) noexcept {
            if constexpr(staticCapacity == 0) return this->__data;
            else {
                if(ifStatic) return this->staticData.data();
                else return this->__data;
            }
        };

        const T* data(const bool ifStatic = false) const noexcept {
            if constexpr(staticCapacity == 0) return this->__data;
            else {
                if(ifStatic) return this->staticData.data();
                else return this->__data;
            }
        };
        /**
         * @brief Get the size of the vector.
         */
        size_t size() const noexcept { return this->__size; }
        /**
         * @brief Get the capacity of the vector.
         */
        size_t capacity() const noexcept { return this->__capacity; }
    public: //Element manipulation functions
        /**
         * @brief Destroys all elements currently stored
         * in the vector.
         * 
         * It does not, however, deallocate memory.
         */
        void clear() {
            if constexpr(!std::is_trivially_destructible_v<T>) {
                for(size_t i = 0; i < this->__size; ++i) {
                    __at(i).~T();
                }
            }
            this->__size = 0;
        }

        /**
         * @brief Reserves space for `n` new elements.
         * 
         * If `capacity() > size() + n`, nothing happens.
         * 
         * If not, the container is reallocated and the new capacity
         * is clamped to an implementation-defined multiple of
         * the previous capacity
         * (in this case 2).
         * 
         * @param n 
         */
        void reserve(const size_t n) {
            if(this->__capacity > this->__size + n) return;
            else this->__expand(2.0f);
        }

        /**
         * @brief Resizes the vector to fit `newSize` elements.
         * 
         * @param newSize new size
         * 
         * If `newSize` < current size, elements are removed until current size
         * equals `newSize`. The capacity does not change during the process.
         * 
         * If `newSize` > current size and `newSize` > current capacity, the container
         * is expanded by a multiple of the current capacity (not necessarily 2).
         * 
         * If `newSize` > current size and `newSize` <= current capacity,
         * no reallocation happens.
         * 
         * In both cases new elements are inserted using their
         * default constructors.
         * 
         * If `newSize` = current size, nothing happens.
         */
        void resize(size_t newSize) {
            if(newSize == this->__size) return;
            else if(newSize > this->__size) {
                if(newSize > this->__capacity) {
                    size_t factor = 1;
                    while(this->__capacity * factor < newSize) { factor++; }
                    this->__expand((float)factor);
                }
                static_assert(std::is_default_constructible_v<T>, "Type doesn't have a default constructor");
                while(this->__size < newSize) { new (&__at(this->__size++)) T{}; }
            }

            else {
                do { __at(--this->__size).~T(); }
                while(this->__size > newSize);
            }
        }

        /**
         * @brief Add a copy of the element to the end
         * of this vector.
         * 
         * @param element element to copy into the vector
         * @return reference to the added element
         * @throws std::bad_alloc on reallocation failure (rare)
         */
        T& append(const T& element) {
            this->__allocIfNeeded();
            return *new (&__at(this->__size++)) T(element);
        }
        /**
         * @brief Move the element into the end
         * of this vector.
         * 
         * @param element element to move into the vector
         * @return reference to the added element
         * @throws std::bad_alloc on reallocation failure (rare)
         */
        T& append(T&& element) {
            this->__allocIfNeeded();
            return *new (&__at(this->__size++)) T(std::move(element));
        }
        /**
         * @brief Create a new element in-place at the end
         * of this vector.
         * 
         * @tparam Args constructor parameters
         * @param args constructor parameters
         * @return reference to the added element
         * @throws std::bad_alloc on reallocation failure (rare)
         */
        template<class...Args> T& emplaceBack(Args&&...args) {
            this->__allocIfNeeded();
            return *new (&__at(this->__size++)) T(args...);
        }
        /**
         * @brief Insert a copy of the element at the given index
         * into this vector.
         * 
         * @param element element to copy
         * @param index index
         * @return reference to the added element
         * @throws std::out_of_range on attempted access out of bounds,
         * std::bad_alloc on reallocation failure (rare)
         */
        T& insert(const T& element, const size_t index) {
            if(index > this->__size) throw std::out_of_range("Attemped insertion into a Vector out of bounds");
            this->__allocIfNeeded();
            this->__shiftBy1Forward(index);
            this->__size++;
            return *new (&__at(index)) T(element);
        }
        /**
         * @brief Move the element into the given index
         * to this vector.
         * 
         * @param element element to move
         * @param index index
         * @return reference to the added element
         * @throws std::out_of_range on attempted access out of bounds,
         * std::bad_alloc on reallocation failure (rare)
         */
        T& insert(T&& element, const size_t index) {
            if(index > this->__size) Unlikely throw std::out_of_range("Attemped insertion into a Vector out of bounds");
            this->__allocIfNeeded();
            this->__shiftBy1Forward(index);
            this->__size++;
            return *new (&__at(index)) T(std::move(element));
        }
        /**
         * @brief Construct a new element at the given index in-place
         * in this vector.
         * 
         * @tparam Args constructor parameters
         * @param index index
         * @param args constructor parameters
         * @return reference to the added element
         * @throws std::out_of_range on attempted access out of bounds,
         * std::bad_alloc on reallocation failure (rare)
         */
        template<class...Args> T& emplace(const size_t index, Args&&...args) {
            if(index > this->__size) throw std::out_of_range("Attemped insertion into a Vector out of bounds");
            this->__allocIfNeeded();
            this->__shiftBy1Forward(index);
            this->__size++;
            return *new (&__at(index)) T(args...);
        }

        /**
         * @brief Return the last element stored in this vector.
         * @return element that was stored
         * @throws std::underflow_error if the vector is empty
         */
        NoDiscard T pop() {
            if(this->empty()) throw std::underflow_error("Cannot pop from empty vector");
            return T(std::move(__at(--this->__size)));
        }
        /**
         * @brief Replace the element at given index with a copy of a given element.
         * 
         * @param newElement element to copy
         * @param index index
         * @return element previously stored at given index
         * @throws std::out_of_range if `index` is out of bounds
         */
        NoDiscard T replace(const T& newElement, const size_t index) {
            if(index >= this->__size) throw std::out_of_range("Attempted replacement out of bounds");
            T oldElement(std::move(__at[index]));
            new (&__at(index)) T(std::move(newElement));
            return oldElement;
        }
        /**
         * @brief Remove the element at given index.
         * 
         * @param index index
         * @return element previously stored at given index
         * @throws std::out_of_range if `index` is out of bounds
         */
        NoDiscard T remove(const size_t index) {
            if(index >= this->__size) throw std::out_of_range("Attempted removal out of bounds");
            else if(index == this->__size - 1) {
                return T(std::move(this->__data[--this->__size]));
            }
            else {
                T t(std::move(this->__data[index]));
                this->__shiftBy1Backward(index);
                return t;
            }
        }
        /**
         * @brief Erase the element at given index. (for now no iterators)
         * 
         * @param index index
         * @throws std::out_of_range if `index` is out of bounds
         */
        void erase(size_t index) { this->remove(index).~T(); }

        /**
         * @brief Perform a lambda expression on every element in
         * this vector.
         * 
         * For some reason slower than an iterator.
         * 
         * @tparam F a lambda expression receiving a T&
         * @param func 
         */
        template<typename F> void forEach(F func) {
            if constexpr(staticCapacity == 0) { for(size_t i = 0; i < this->__size; ++i) { func(this->__data[i]); } }
            else {
                size_t i = 0;
                if(this->__size >= staticCapacity) {
                    for(; i < staticCapacity; ++i) { func(this->staticData[i]); }
                    for(i = 0; i < this->__size - staticCapacity; ++i) { func(this->__data[i]); }
                }
                else { for(; i < this->__size; ++i) { func(this->staticData[i]); } }
            }
        }
};