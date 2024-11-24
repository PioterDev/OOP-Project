#pragma once

#include <cstdlib>
#include <cstdint>

#include "deus.hpp"

using namespace Enums;

/**
 * @brief A more cache and memory-friendly singly linked list implementation.
 * Stores a variable amount of elements in each node.
 * 
 * @tparam T 
 */
template<typename T> class ListArray {
    private:
        typedef struct ListArrayNode {
            struct ListArrayNode* next;
            u32 nodeSize;
            u32 usedSize;
            //ah yes...undefined behavior, love it
            T objects[1];
        } ListArrayNode;

        ListArrayNode* head;
        ListArrayNode* tail;
        u32 numberOfNodes = 0;
        u32 nodeSize = 4'096;

        ListArrayNode* createNode(size_t size) {
            ListArrayNode* node = (ListArrayNode*)malloc(size);
            if(node == nullptr) throw std::bad_alloc();

            node->next = nullptr;
            node->nodeSize = size;
            node->usedSize = 16ull;
            this->numberOfNodes++;

            return node;
        }
    public:
        ListArray() {
            this->head = createNode(nodeSize);
            this->tail = this->head;
        }

        ListArray(u32 nodeSize) {
            this->nodeSize = nodeSize;
            this->head = createNode(nodeSize);
            this->tail = this->head;
        }

        ~ListArray() {
            ListArrayNode* current = this->head;
            ListArrayNode* next;
            while(current != nullptr) {
                next = current->next;
                free(current);
                current = next;
            }
        }

        void prepend(T obj) {
            if(this->head->usedSize + sizeof(T) > this->head->nodeSize) {
                ListArrayNode* newHead = createNode(this->nodeSize);
                newHead->next = this->head;
                this->head = newHead;
            }
            this->head->objects[(head->usedSize - 16ull) / sizeof(T)] = obj;
            this->head->usedSize += sizeof(T);
        }
        
        void append(T obj) {
            if(this->tail->usedSize + sizeof(T) > this->tail->nodeSize) {
                this->tail->next = createNode(this->nodeSize);
                this->tail = this->tail->next;
            }
            this->tail->objects[(this->tail->usedSize - (u64)16) / sizeof(T)] = obj;
            this->tail->usedSize += sizeof(T);
        }

        u32 getNumberOfNodes() const { return this->numberOfNodes; }

        template<typename F> void forEach(F func) {
            ListArrayNode* current = this->head;
            while(current != nullptr) {
                for(size_t i = 0; i < (current->usedSize - (u64)16) / sizeof(T); i++) {
                    func(current->objects[i]);
                }
                current = current->next;
            }
        }
};