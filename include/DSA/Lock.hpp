#pragma once

#include <atomic>

class SpinLock {
    private:
        std::atomic_flag f = ATOMIC_FLAG_INIT;
    public:
        SpinLock() = default;
        ~SpinLock() = default;
        SpinLock(const SpinLock&) = delete;
        SpinLock(SpinLock&&) = delete;
        void operator=(const SpinLock&) = delete;
        void operator=(SpinLock&& ) = delete;

        void lock() { while(f.test_and_set(std::memory_order_acquire)); }
        void unlock() { f.clear(std::memory_order_release); }
};