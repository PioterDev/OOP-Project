#include <algorithm>

#include "ArrayTree.hpp"

/* template<class Key, class Value>
int ArrayTree<Key, Value>::insert(Key key, Value value) {
    struct ArrayTreeNode<Key, Value>* current = this->root;
    while(true) {
        //case 1: current node is empty
        if(current->numberOfElements == 0) {
            current->elements[0].key = key;
            current->elements[0].value = value;
            current->numberOfElements++;
            return 0;
        }
        //case 2: current has at least 1 empty slot
        if((current->numberOfElements - 1) * sizeof(current->elements[0]) + sizeof(current) <= this->nodeSize) {
            //do a binary search to find the index
            int L = 0, R = current->numberOfElements - 1;
            int deltaIndex = 0;
            int M, previous;
            do {
                M = (L + R) / 2;
                if(current->elements[index].key < key) {
                    previous = L;
                    L = M + 1;
                    deltaIndex = previous - L;
                }
                else if(current->elements[index].key > key) {
                    previous = R;
                    R = M - 1;
                    deltaIndex = previous - R;
                }
                //there already is an element with the given key
                else {
                    return 2;
                }
            } while(deltaIndex != 0);
            //move memory 1 slot
            memmove(
                &current->elements[M],
                &current->elements[M + 1],
                (current->numberOfElements - M) * sizeof(current->elements[0])
            );
            current->elements[M].key = key;
            current->elements[M].value = value;
            current->numberOfElements++;
            return 0;
        }
        //case 3: current node is full
        else {
            //continue left
            if(current->elements[0].key > key) {
                if(current->left == nullptr) {
                    current->left = (struct ArrayTreeNode<Key, Value>*)malloc(this->nodeSize);
                    if(current->left == nullptr) return 1;
                    current->left->parent = current;
                    
                }
                current = current->left;
                continue;
            }
            //continue right
            else if(current->elements[current->numberOfElements - 1].key < key) {
                current = current->right;
                continue;
            }
            //the bad case: move rightmost element to the next node
            else {

            }
        }
    }   
} */