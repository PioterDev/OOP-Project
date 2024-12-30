#pragma once

#include "Bindings.h"

#include <functional>

#include "DSA/Vector.hpp"



template<size_t N = 1> class EventNotifier {
    private:
        Vector<std::function<void()>, N> observers;
    public:
        explicit EventNotifier() = default;
        ~EventNotifier() = default;

        EventNotifier(const EventNotifier&) = delete;
        EventNotifier(EventNotifier&& other) : observers(std::move(other.observers)) {}

        EventNotifier& operator=(const EventNotifier& other) = delete;
        EventNotifier& operator=(EventNotifier&& other) {
            this->observers = std::move(other.observers);

            return *this;
        }

        bool subscribe(std::function<void()>&& callback) {
            try {
                this->observers.append(std::move(callback));
            }
            catch(const std::bad_alloc& e) {
                return false;
            }
            return true;
        }

        void notifyAll() {
            for(auto it = this->observers.begin(); it != this->observers.end(); ++it) {
                (*it)();
            }
        }
};