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
            uint32_t nodeSize;
            uint32_t usedSize;
            T objects[1];
        } ListArrayNode;

        ListArrayNode* head;
        ListArrayNode* tail;
        uint32_t numberOfNodes = 0;
        uint32_t nodeSize = 1024;

        ListArrayNode* createNode(size_t size) {
            ListArrayNode* node = (ListArrayNode*)malloc(size);
            if(node == nullptr) return nullptr;

            node->next = nullptr;
            node->nodeSize = size;
            node->usedSize = 16ull;
            this->numberOfNodes++;

            return node;
        }
    public:
        ListArray(uint32_t nodeSize) {
            this->nodeSize = nodeSize;
            this->head = createNode(nodeSize);
            if(this->head == nullptr) return;
            tail = this->head;
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

        Status prepend(T obj) {
            if(head->usedSize + sizeof(T) > head->nodeSize) {
                ListArrayNode* newHead = createNode(this->nodeSize);
                if(newHead == nullptr) return Status::FAILURE;
                newHead->next = head;
                head = newHead;
            }
            head->objects[(head->usedSize - 16ull) / sizeof(T)] = obj;
            head->usedSize += sizeof(T);
            return Status::SUCCESS;
        }
        
        Status append(T obj) {
            if(tail->usedSize + sizeof(T) > tail->nodeSize) {
                tail->next = createNode(this->nodeSize);
                if(tail->next == nullptr) return Status::FAILURE;
                tail = tail->next;
            }
            tail->objects[(tail->usedSize - 16ull) / sizeof(T)] = obj;
            tail->usedSize += sizeof(T);
            return Status::SUCCESS;
        }

        uint32_t getNumberOfNodes() const { return this->numberOfNodes; }

        template<typename F> void forEach(F func) {
            ListArrayNode* current = this->head;
            while(current != nullptr) {
                for(size_t i = 0; i < (current->usedSize - (uint64_t)16) / sizeof(T); i++) {
                    func(current->objects[i]);
                }
                current = current->next;
            }
        }
};