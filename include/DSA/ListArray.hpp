#pragma once

#include <cstdlib>
#include <cstdint>

#include "deus.hpp"

using namespace Enums;

/**
 * @brief A more cache and memory-friendly singly linked list implementation.
 * Stores a variable amount of elements in each node.
 */
template<typename T> class ListArray {
    private:
        typedef struct ListArrayNode {
            struct ListArrayNode* next;
            u32 nodeSize;
            u32 usedSize;
            //ah yes...undefined behavior, love it
            T elements[1];
        } ListArrayNode;

        u32 nodeSize = 4'096;
        u32 numberOfNodes = 0;
        ListArrayNode* head = nullptr;
        ListArrayNode* tail = nullptr;

        ListArrayNode* createNode(size_t size) {
            ListArrayNode* node = (ListArrayNode*)malloc(size);
            if(node == nullptr) throw std::bad_alloc();

            node->next = nullptr;
            node->nodeSize = size;
            node->usedSize = 16u;
            this->numberOfNodes++;

            return node;
        }
    public:
        ListArray() : head(createNode(nodeSize)), tail(head) {}

        ListArray(u32 nodeSize)
         : nodeSize(nodeSize), head(createNode(nodeSize)), tail(head) {}

        ~ListArray() {
            ListArrayNode* current = this->head;
            ListArrayNode* next;
            while(current != nullptr) {
                next = current->next;
                for(size_t i = 0; i < (current->usedSize - 32u) / sizeof(T); i++) {
                    current->elements[i].~T();
                }
                free(current);
                current = next;
            }
        }

        T& operator[](size_t index) {
            size_t i = 0, currentIndex = 0;
            ListArrayNode* current = this->head;
            while(currentIndex != index) {
                if(current == nullptr) throw std::out_of_range("");
                if(currentIndex >= (current->usedSize - 16u) / sizeof(T) - 1) {
                    currentIndex = 0;
                    current = current->next;
                    continue;
                }
                i++;
            }
            return current->elements[currentIndex];
        }

        /**
         * @brief Adds the object at the start of the list.
         * It is not guaranteed that it will be first,
         * only that it will be in the first node.
         * 
         * @param obj object to add, constructed in-place
         * @return reference to the object added
         */
        T& prepend(T obj) {
            if(this->head->usedSize + sizeof(T) > this->head->nodeSize) {
                ListArrayNode* newHead = createNode(this->nodeSize);
                newHead->next = this->head;
                this->head = newHead;
            }
            T* x = new(&this->head->elements[(head->usedSize - (u32)16) / sizeof(T)]) T(std::move(obj));
            this->head->usedSize += sizeof(T);

            return *x;
        }
        
        /**
         * @brief Adds the object at the end of the list.
         * 
         * @param obj object to add, constructed in-place
         * @return reference to the object added
         */
        T& append(T obj) {
            if(this->tail->usedSize + sizeof(T) > this->tail->nodeSize) {
                this->tail->next = createNode(this->nodeSize);
                this->tail = this->tail->next;
            }
            //this has to be this weird shenanigan because direct
            //assignment doesn't actually move `obj` into the container for some reason...
            T* x = new (&this->tail->elements[(this->tail->usedSize - (u32)16) / sizeof(T)]) T(std::move(obj));
            this->tail->usedSize += sizeof(T);

            return *x;
        }

        u32 getNumberOfNodes() const { return this->numberOfNodes; }

        template<typename F> void forEach(F func) {
            ListArrayNode* current = this->head;
            while(current != nullptr) {
                for(size_t i = 0; i < (current->usedSize - (u32)16) / sizeof(T); i++) {
                    func(current->elements[i]);
                }
                current = current->next;
            }
        }
};