#pragma once

#include "Bindings.h"

#include <cstdlib>
#include <string>

#include "deus.hpp"

template<typename T> class ArrayTree {
    private:
        typedef struct ArrayTreeNode {
            struct ArrayTreeNode* left;
            struct ArrayTreeNode* right;
            uint32_t nodeSize;
            uint32_t usedSize;
            uint8_t leftHeight;
            uint8_t rightHeight;
            uint16_t padding1;
            uint32_t padding2;
            T elements[];
        } ArrayTreeNode;

        ArrayTreeNode* root;
        uint32_t numberOfNodes = 0;
        uint32_t nodeSize = 1024;

        ArrayTreeNode* createNode(size_t size) {
            ArrayTreeNode* node = (ArrayTreeNode*)malloc(size);
            if(node == nullptr) return nullptr;

            node->left = node->right = nullptr;
            node->nodeSize = size;
            node->usedSize = 32u;
            node->leftHeight = node->rightHeight = 0;

            return node;
        }

        void deleteTree(ArrayTreeNode* root) {
            if(root == nullptr) return;
            deleteTree(root->left);
            deleteTree(root->right);
            free(root);
        }
    public:
        ArrayTree(uint32_t nodeSize) {
            this->nodeSize = nodeSize;
            this->root = createNode(nodeSize);
        }

        ~ArrayTree() {
            deleteTree(this->root);
        }

        Status add(T t) {
            ArrayTreeNode* current = this->root;
            if(current->usedSize + sizeof(T) > current->nodeSize) {
                bool cmp1 = t < current->elements[0];
                bool cmp2 = t > current->elements[(current->usedSize - 32u) / sizeof(T)];
                if(cmp1 ^ cmp2) {
                    //this is too brain-intensive for me atm...
                }
            }
            else {

            }
        }
};