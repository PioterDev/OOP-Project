#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>

typedef int8_t i8;
typedef int32_t i32;
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

using std::cout, std::endl;

#ifndef ForceInline
#if ((defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__))
#define ForceInline inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ForceInline __forceinline /* MSVC */
#else
#define ForceInline inline
#endif /* Compiler */
#endif /* Defining ForceInline */


//TODO: change insertion so that the left-most element
//can be evicted (for less memmoves)
//TODO: add logic for the edge case of
//t being either:
//- ordered after the left-most element in the right subtree of the current node
//- ordered before the right-most element in the left subtree
/**
 * @brief An implementation of an unrolled
 * AVL BST. The container is responsible for
 * memory management of its stored members. Calling
 * destructors on them from outside is undefined behavior.
 * The implementation tries to use as little of
 * C++'s automatic memory management features as possible
 * for full predictability of operation (a very C-style approach).
 * 
 * In this household, we ignore safety! Go to hell, constructors, destructors,
 * copy and move constructors, copy and move assignment operators!
 * 
 * This data structure is meant to store multiple elements in a single
 * node for both spatial locality and less jumping in memory.
 * If each node is set to store only 1 element, it functions like
 * a slightly less memory-efficient AVL BST.
 * 
 * @tparam T 
 */
template<typename T> class ArrayTree {
    private:
        constexpr const static inline u32 LS24B = 0x00FFFFFF;
        constexpr const static inline u32 minUsedSize = 32u;
        typedef struct ArrayTreeNode {
            struct ArrayTreeNode* left;
            struct ArrayTreeNode* right;
            struct ArrayTreeNode* parent;
            /**
             * @brief The most significant bit is used in the tree traversal
             * to mark the left tree as visited and required to be 0 otherwise.
             * Bits 24-30 store bits store the height of the left subtree.
             */
            u32 nodeSize;
            /**
             * @brief The most significant bit is used in the tree traversal
             * to mark the right tree as visited and required to be 0 otherwise.
             * Bits 24-30 store bits store the height of the right subtree.
             */
            u32 usedSize;
            T elements[1];
        } ArrayTreeNode;
        u32 nodeSize = 4'096;
        u32 numberOfNodes = 0;
        ArrayTreeNode* root = nullptr;

        /**
         * @brief A linked list of nodes not currently in use.
         * Here the `left` field points to the previous node,
         * `right` points to the next node, while `parent`
         * is unused, except for the head, which points to the
         * last element of the list.
         */
        ArrayTreeNode* spareNodes = nullptr;

        ArrayTreeNode* createNode(size_t size, ArrayTreeNode* parent) {
            ArrayTreeNode* node = (ArrayTreeNode*)malloc(size);
            if(node == nullptr) throw std::bad_alloc();

            node->left = node->right = nullptr;
            node->parent = parent;
            node->nodeSize = size;
            node->usedSize = minUsedSize;

            this->numberOfNodes++;

            return node;
        }

        ArrayTreeNode* getNode() {
            if(this->spareNodes == nullptr) {
                return this->createNode(this->nodeSize, nullptr);
            }
            //only one spare node
            if(this->spareNodes->parent == this->spareNodes) {
                ArrayTreeNode* last = this->spareNodes;
                this->spareNodes = nullptr;
                last->parent = nullptr;
                return last;
            }
            else {
                ArrayTreeNode* last = this->spareNodes->parent;
                this->spareNodes->parent = last->left;
                last->left = nullptr;
                return last;
            }
        }

        void deleteTree(ArrayTreeNode* root) {
            if(root == nullptr) return;
            deleteTree(root->left);
            deleteTree(root->right);
            for(size_t i = 0; i < ((root->usedSize & LS24B) - minUsedSize) / sizeof(T); i++) {
                root->elements[i].~T();
            }
            free(root);
            this->numberOfNodes--;
        }

        /**
         * @brief Adds the element to the tree.
         * 
         * @tparam t pointer to the element to add
         * @param root root of the current subtree
         * @param mostRecentAdded placeholder for the pointer
         * to the added element so that it can be returned from `add(...)`.
         * @return whether the height of the subtree increased,
         * if so, rebalance if needed
         */
        bool __add(T* t, ArrayTreeNode* root, T** mostRecentAdded) {
            /**
             * @brief Case 1:
             * The element won't fit into the current node.
             * Decide whether to go left, right, or evict the last element in
             * the current node and recursively add it back in the right subtree.
             */
            if((root->usedSize & LS24B) + sizeof(T) > (root->nodeSize & LS24B)) {
                size_t lastIndex = ((root->usedSize & LS24B) - minUsedSize) / sizeof(T) - 1;

                //t is ordered before the 1st element of current node
                bool cmp1 = *t < root->elements[0];
                //t is ordered after the last element of current node
                bool cmp2 = *t > root->elements[lastIndex];
                
                //neither is true, evict the last element
                if(!(cmp1 || cmp2)) {
                    //find the position to insert new element
                    i32 left = 0, right = lastIndex, mid;
                    while(left < right) {
                        mid = (left + right) / 2;
                        if(root->elements[mid] < *t) left = mid + 1;
                        else right = mid - 1;
                    }
                    mid = (left + right) / 2;

                    char last[sizeof(T)];
                    memcpy(last, (char*)(&root->elements[lastIndex]), sizeof(T));
                    memmove(
                        (char*)(&root->elements[mid + 1]),
                        (char*)(&root->elements[mid]),
                        sizeof(T) * (lastIndex - mid)
                    );
                    memcpy((char*)(&root->elements[mid]), (char*)t, sizeof(T));

                    //if there's no node to the right, create it
                    if(root->right == nullptr) {
                        root->right = this->createNode(this->nodeSize, root);
                        memcpy((char*)(&root->right->elements[0]), last, sizeof(T));
                        this->incrementUsedNodeSize(root->right, sizeof(T));

                        if(this->incrementRightSubtreeHeight(root) > this->getLeftSubtreeHeight(root)) root->nodeSize |= (1 << 31);
                        //rebalancing here will never be needed here
                        //at least according to my (flawed as always) reasoning...
                    }
                    //else continue recursively down the right subtree
                    else {
                        if(this->__add(reinterpret_cast<T*>(last), root->right, mostRecentAdded)) {
                            //difference between the height of left and right subtrees
                            i8 dh = this->getLeftSubtreeHeight(root) - this->incrementRightSubtreeHeight(root);
                            if(dh < -1) {
                                if(this->getLeftSubtreeHeight(root->right) > this->getRightSubtreeHeight(root->right)) {
                                    this->rotateRightLeft(root);
                                }
                                else this->rotateLeft(root);
                            }
                            //mark that there was a new node added
                            root->nodeSize |= (1 << 31);
                        }
                    }

                    *mostRecentAdded = &root->elements[mid];
                    if(root->nodeSize & (1 << 31)) {
                        root->nodeSize &= ~(1 << 31);
                        return true;
                    }
                    else return false;
                }
                else if(cmp1) { //go left
                    if(root->left == nullptr) {
                        root->left = this->createNode(this->nodeSize, root);
                        memcpy((char*)root->left->elements, (char*)t, sizeof(T));
                        // this->incrementUsedNodeSize(root->left, sizeof(T));
                        root->left->usedSize += sizeof(T);
                        *mostRecentAdded = &root->left->elements[0];

                        if(this->incrementLeftSubtreeHeight(root) > this->getRightSubtreeHeight(root)) return true;
                        else return false;
                    }
                    else {
                        if(this->__add(t, root->left, mostRecentAdded)) {
                            //difference between the height of left and right subtrees
                            i8 dh = this->incrementLeftSubtreeHeight(root) - this->getRightSubtreeHeight(root);
                            if(dh > 1) {
                                if(this->getLeftSubtreeHeight(root->left) < this->getRightSubtreeHeight(root->left)) {
                                    this->rotateLeftRight(root);
                                }
                                else this->rotateRight(root);
                                return false;
                            }
                            else return true;
                        }
                        else return false;
                    }
                }
                else if(cmp2) { //go right
                    if(root->right == nullptr) {
                        root->right = this->createNode(this->nodeSize, root);
                        memcpy((char*)&root->right->elements[0], (char*)t, sizeof(T));
                        // this->incrementUsedNodeSize(root->right, sizeof(T));
                        root->right->usedSize += sizeof(T);
                        *mostRecentAdded = &root->right->elements[0];

                        if(this->incrementRightSubtreeHeight(root) > this->getLeftSubtreeHeight(root)) return true;
                        else return false;
                    }
                    else {
                        if(this->__add(t, root->right, mostRecentAdded)) {
                            //difference between the height of left and right subtrees
                            i8 dh = this->getLeftSubtreeHeight(root) - this->incrementRightSubtreeHeight(root);
                            if(dh < -1) {
                                if(this->getLeftSubtreeHeight(root->right) > this->getRightSubtreeHeight(root->right)) {
                                    this->rotateRightLeft(root);
                                }
                                else this->rotateLeft(root);
                                
                                return false;
                            }
                            else return true;
                        }
                        else return false;
                    }
                }
                else if(cmp1 && cmp2) { //by a miracle of god both are true
                    //This special case shall be celebrated!!!
                    //...
                    //By throwing an exception because this state
                    //should be unreachable.
                    throw std::runtime_error("Congratulations! You reached an impossible branch! Now terminate.");
                    *mostRecentAdded = nullptr;

                    return false; //for the compiler to shut up
                }
            }
            //Case 2: there is space left in the current node
            else {
                i32 lastIndex = ((root->usedSize & LS24B) - minUsedSize) / sizeof(T);
                if(lastIndex == 0) {
                    T* cmp = this->_min(root->right);
                    if(cmp == nullptr) goto insert0;
                    //left-most element of the right subtree is ordered after t
                    else if(*cmp > *t) {
                        
                    }
                    else if(*cmp < *t) goto insert0;
                    else {
                        cmp = this->_max(root->left);
                        if(cmp == nullptr) goto insert0;
                        //right-most element of the right subtree is ordered before t
                        else if(*cmp < *t) {

                        }
                        else {

                        }
                    }
                    insert0: {
                        memcpy((char*)&root->elements[0], (char*)t, sizeof(T));
                        *mostRecentAdded = &root->elements[0];
                    }
                }
                else {
                    i32 left = 0, right = lastIndex, mid;
                    while(left < right) {
                        mid = (left + right) / 2;
                        if(root->elements[mid] < *t) {
                            left = mid + 1;
                        }
                        else {
                            right = mid - 1;
                        }
                    }
                    mid = (left + right) / 2;
                    memmove(
                        (char*)(&root->elements[mid + 1]),
                        (char*)(&root->elements[mid]),
                        sizeof(T) * (lastIndex - mid)
                    );
                    memcpy((char*)(&root->elements[mid]), (char*)t, sizeof(T));

                    *mostRecentAdded = &root->elements[mid];
                }

                root->usedSize += sizeof(T);
                return false;
            }
            return false; //for the compiler to shut up
        }

        ArrayTreeNode* rotateLeft(ArrayTreeNode* root) noexcept {
            ArrayTreeNode* right = root->right;
            ArrayTreeNode* parent = root->parent;
            
            root->right = right->left;
            right->left = root;

            right->parent = parent;
            root->parent = right;

            //update the parent of root
            if(parent) {
                if(parent->right == root) parent->right = right;
                else parent->left = right;
            }
            if(root->right) root->right->parent = root;

            //root's right height = right's left height
            root->usedSize = (root->usedSize & LS24B) | (right->nodeSize & ~LS24B);

            //right's left height = root's left height + 1
            right->nodeSize = (right->nodeSize & LS24B) | ((root->nodeSize & ~LS24B) + (1 << 24));

            if(root == this->root) this->root = right;

            return right;
        }

        ArrayTreeNode* rotateRight(ArrayTreeNode* root) noexcept {
            ArrayTreeNode* left = root->left;
            ArrayTreeNode* parent = root->parent;

            root->left = left->right;
            left->right = root;

            left->parent = parent;
            root->parent = left;

            if(parent) {
                if(parent->right == root) parent->right = left;
                else parent->left = left;
            }
            if(root->left) root->left->parent = root;

            //root's left height = left's right height
            root->nodeSize = (root->nodeSize & LS24B) | (left->usedSize & ~LS24B);

            //left's right height = root's right height + 1
            left->usedSize = (left->usedSize & LS24B) | ((root->usedSize & ~LS24B) + (1 << 24));

            if(root == this->root) this->root = left;

            return left;
        }

        ArrayTreeNode* rotateRightLeft(ArrayTreeNode* root) noexcept {
            this->rotateRight(root->right);
            return this->rotateLeft(root);
        }

        ArrayTreeNode* rotateLeftRight(ArrayTreeNode* root) noexcept {
            this->rotateLeft(root->left);
            return this->rotateRight(root);
        }

        ForceInline u8 incrementLeftSubtreeHeight(ArrayTreeNode* root) noexcept {
            root->nodeSize += 1 << 24;
            return (root->nodeSize & ~LS24B) >> 24;
        }

        ForceInline u8 incrementRightSubtreeHeight(ArrayTreeNode* root) noexcept {
            root->usedSize += 1 << 24;
            return (root->usedSize & ~LS24B) >> 24;
        }

        ForceInline u32 incrementUsedNodeSize(ArrayTreeNode* node, u32 byHowMuch) noexcept {
            node->usedSize += byHowMuch;
            return node->usedSize & LS24B;
        }

        ForceInline u8 getLeftSubtreeHeight(ArrayTreeNode* node) const noexcept {
            return node != nullptr ? (node->nodeSize & ~LS24B) >> 24 : '\0';
        }

        ForceInline u8 getRightSubtreeHeight(ArrayTreeNode* node) const noexcept {
            return node != nullptr ? (node->usedSize & ~LS24B) >> 24 : '\0';
        }

        template<typename F> void __forEachInOrder(F f, ArrayTreeNode* root) {
            while(true) {
                //Case 1: both subtrees visited, go back up
                if(root->nodeSize & (1 << 31) && root->usedSize & (1 << 31)) {
                    root->nodeSize &= ~(1 << 31); //mark as unvisited
                    root->usedSize &= ~(1 << 31);

                    if(root == this->root) break;
                    else root = root->parent;
                }
                //Case 2: left subtree visited, right subtree not visited
                else if(root->nodeSize & (1 << 31)) {
                    for(size_t i = 0; i < ((root->usedSize & LS24B) - 32u) / sizeof(T); i++) {
                        f(root->elements[i]);
                    }
                    root->usedSize |= 1 << 31; //mark right subtree as visited
                    if(root->right != nullptr) root = root->right;
                }
                //Case 3: both subtrees not visited
                else if(!(root->nodeSize & (1 << 31) || root->usedSize & (1 << 31))) {
                    root->nodeSize |= 1 << 31;
                    if(root->left != nullptr) root = root->left;
                }
                else break; //logically impossible
            }
        }

        template<typename F> void __forEachPreOrder(F f, ArrayTreeNode* root) {
            while(true) {
                //Case 1: both subtrees visited, go back up
                if(root->nodeSize & (1 << 31) && root->usedSize & (1 << 31)) {
                    root->nodeSize &= ~(1 << 31); //mark as unvisited
                    root->usedSize &= ~(1 << 31);

                    if(root == this->root) break; //end of traversal: back at the root
                    else root = root->parent;
                }
                //Case 2: left subtree visited, right subtree not visited
                else if(root->nodeSize & (1 << 31)) {
                    root->usedSize |= 1 << 31; //mark right subtree as visited
                    if(root->right != nullptr) root = root->right;
                }
                //Case 3: both subtrees not visited
                else if(!(root->nodeSize & (1 << 31) || root->usedSize & (1 << 31))) {
                    for(size_t i = 0; i < ((root->usedSize & LS24B) - 32u) / sizeof(T); i++) {
                        f(root->elements[i]);
                    }
                    root->nodeSize |= 1 << 31;
                    if(root->left != nullptr) root = root->left;
                }
                else break; //right subtree visited, left subtree not visited - logically impossible here
            }
        }

        template<typename F> void __forEachPostOrder(F f, ArrayTreeNode* root) {
            while(true) {
                //Case 1: both subtrees visited, go back up
                if(root->nodeSize & (1 << 31) && root->usedSize & (1 << 31)) {
                    for(size_t i = 0; i < ((root->usedSize & LS24B) - 32u) / sizeof(T); i++) {
                        f(root->elements[i]);
                    }
                    root->nodeSize &= ~(1 << 31); //mark as unvisited
                    root->usedSize &= ~(1 << 31);

                    if(root->parent == nullptr) break; //end of traversal: back at the root
                    else root = root->parent;
                }
                //Case 2: left subtree visited, right subtree not visited
                else if(root->nodeSize & (1 << 31)) {
                    root->usedSize |= 1 << 31; //mark right subtree as visited
                    if(root->right != nullptr) root = root->right;
                }
                //Case 3: both subtrees not visited
                else if(!(root->nodeSize & (1 << 31) || root->usedSize & (1 << 31))) {
                    root->nodeSize |= 1 << 31;
                    if(root->left != nullptr) root = root->left;
                }
                else break; //right subtree visited, left subtree not visited - logically impossible here
            }
        }

        template<typename F> void __forEachInOrderRecursive(F f, ArrayTreeNode* root) {
            if(root == nullptr) return;
            this->__forEachInOrderRecursive(f, root->left);
            for(size_t i = 0; i < ((root->usedSize & LS24B) - 32u) / sizeof(T); i++) {
                f(root->elements[i]);
            }
            this->__forEachInOrderRecursive(f, root->right);
        }

        template<typename F> void __forEachPreOrderRecursive(F f, ArrayTreeNode* root) {
            if(root == nullptr) return;
            for(size_t i = 0; i < ((root->usedSize & LS24B) - 32u) / sizeof(T); i++) {
                f(root->elements[i]);
            }
            this->__forEachPreOrderRecursive(f, root->left);
            this->__forEachPreOrderRecursive(f, root->right);
        }

        template<typename F> void __forEachPostOrderRecursive(F f, ArrayTreeNode* root) {
            if(root == nullptr) return;
            this->__forEachPostOrderRecursive(f, root->left);
            this->__forEachPostOrderRecursive(f, root->right);
            for(size_t i = 0; i < ((root->usedSize & LS24B) - 32u) / sizeof(T); i++) {
                f(root->elements[i]);
            }
        }

        T* _min(ArrayTreeNode* root) noexcept {
            if(root == nullptr) return nullptr;
            while(root->left != nullptr) { root = root->left; }
            return &root->elements[0];
        }

        T* _max(ArrayTreeNode* root) noexcept {
            if(root == nullptr) return nullptr;
            while(root->right != nullptr) { root = root->right; }
            return &root->elements[((root->usedSize & LS24B) - 32u) / sizeof(T) - 1];
        }

        ArrayTreeNode* __minNode(ArrayTreeNode* root) noexcept {
            while(root->left != nullptr) { root = root->left; }
            return root;
        }

        ArrayTreeNode* __maxNode(ArrayTreeNode* root) noexcept {
            while(root->right != nullptr) { root = root->right; }
            return root;
        }

        void __addNodeToSpares(ArrayTreeNode* current) noexcept {
            memset((char*)current->elements, 0, (current->nodeSize & LS24B) - 32u); //a safety measure
            if(this->spareNodes == nullptr) {
                this->spareNodes = current;
                current->parent = current;
                current->left = current->right = nullptr;
            }
            else {
                //head of `spareNodes`'parent points to the last node
                ArrayTreeNode* last = this->spareNodes->parent;
                last->right = current;
                current->left = last;
                current->right = current->parent = nullptr;
                this->spareNodes->parent = current;
            }
            current->nodeSize &= LS24B; //reset heights
            current->usedSize = minUsedSize;
        }

        /**
         * @brief Removes the given node itself from the tree.
         * Only supposed to be used when removing another node
         * with 2 children.
         * 
         * @param node node to be temporarily removed
         * from the tree
         * 
         * This function is unnecesarily long and
         * complicated, but it seems to work
         */
        void __removeNode(ArrayTreeNode* node) {
            ArrayTreeNode* parent = node->parent;
            if(node->left == nullptr && node->right == nullptr) {
                if(node == this->root) return;
                if(parent->left == node) {
                    parent->left = nullptr;
                    parent->nodeSize -= 1 << 24;
                }
                else {
                    parent->right = nullptr;
                    parent->usedSize -= 1 << 24;
                }
                i8 dh = this->getLeftSubtreeHeight(parent) - this->getRightSubtreeHeight(parent);
                if(dh > 1) {
                    if(this->getLeftSubtreeHeight(parent->left) < this->getRightSubtreeHeight(parent->left)) {
                        parent = this->rotateLeftRight(parent);
                    }
                    else {
                        parent = this->rotateRight(parent);
                        if(parent->right->left != nullptr) return;
                    }

                    goto updateHeight;
                }
                else if(dh < -1) {
                    if(this->getLeftSubtreeHeight(parent->right) > this->getRightSubtreeHeight(parent->right)) {
                        parent = this->rotateRightLeft(parent);
                    }
                    else {
                        parent = this->rotateLeft(parent);
                        if(parent->left->right != nullptr) return;
                    }

                    goto updateHeight;
                }
                else if(dh == 0) goto updateHeight;
                else return;
            }
            else {
                if(node->left != nullptr && node->right != nullptr) {
                    int cmp = this->getLeftSubtreeHeight(node) - this->getRightSubtreeHeight(node);
                    ArrayTreeNode* replacement = nullptr;
                    
                    if(cmp < 0) replacement = this->__minNode(node->right); //left < right
                    else if(cmp > 0) replacement = this->__maxNode(node->left); //left > right
                    else replacement = this->__minNode(node->right); //left == right, choose arbirarily
                    assert(replacement != nullptr);
                    
                    ArrayTreeNode* p = replacement->parent;
                    if(replacement->left == nullptr && replacement->right == nullptr) { //leaf
                        if(p->left == replacement) {
                            p->left = nullptr;
                            p->nodeSize -= 1 << 24;
                        }
                        else {
                            p->right = nullptr;
                            p->usedSize -= 1 << 24;
                        }
                        if(p == node) goto replace;

                        i8 dh = this->getLeftSubtreeHeight(p) - this->getRightSubtreeHeight(p);
                        if(dh > 1) {
                            if(this->getLeftSubtreeHeight(p->left) < this->getRightSubtreeHeight(p->left)) {
                                p = this->rotateLeftRight(p);
                            }
                            else p = this->rotateRight(p);
                            goto updateHeight2;
                        }
                        else if(dh < -1) {
                            if(this->getLeftSubtreeHeight(p->right) > this->getRightSubtreeHeight(p->right)) {
                                p = this->rotateRightLeft(p);
                            }
                            else p = this->rotateLeft(p);
                            goto updateHeight2;
                        }
                        else if(dh == 0) goto updateHeight2;
                        else goto replace;
                    }
                    else {
                        //here there's no 2 children issue
                        ArrayTreeNode* child;
                        if(replacement->left) child = replacement->left;
                        else child = replacement->right;

                        if(p->left == replacement) {
                            p->left = child;
                            p->nodeSize -= 1 << 24;
                            child->parent = p;
                        }
                        else {
                            p->right = child;
                            p->usedSize -= 1 << 24;
                            child->parent = p;
                        }
                        if(p == node) goto replace;

                        i8 dh = this->getLeftSubtreeHeight(p) - this->getRightSubtreeHeight(p);
                        if(dh > 1) {
                            if(this->getLeftSubtreeHeight(p->left) < this->getRightSubtreeHeight(p->left)) {
                                p = this->rotateLeftRight(p);
                            }
                            else p = this->rotateRight(p);

                            goto updateHeight2;
                        }
                        else if(dh < -1) {
                            if(this->getLeftSubtreeHeight(p->right) > this->getRightSubtreeHeight(p->right)) {
                                p = this->rotateRightLeft(p);
                            }
                            else p = this->rotateLeft(p);

                            goto updateHeight2;
                        }
                        else if(dh == 0) goto updateHeight2;
                        else goto replace;
                    }
                    updateHeight2: {
                        ArrayTreeNode* next = p->parent;
                        while(true) { //rebalance the tree but only up to `node`
                            if(next->left == p) next->nodeSize -= 1 << 24;
                            else next->usedSize -= 1 << 24;
                            if(next == node) break;

                            i8 dh = this->getLeftSubtreeHeight(next) - this->getRightSubtreeHeight(next);
                            if(next->left == p) {
                                if(dh < -1) {
                                    if(this->getLeftSubtreeHeight(next->right) > this->getRightSubtreeHeight(next->right)) {
                                        next = this->rotateRightLeft(next);
                                    }
                                    else next = this->rotateRight(next);
                                    p = next;
                                    next = next->parent;
                                }
                                else if(dh == -1) break;
                                else if(dh == 0) {
                                    p = next;
                                    next = next->parent;
                                }
                            }
                            else {
                                if(dh > 1) {
                                    if(this->getLeftSubtreeHeight(next->left) < this->getRightSubtreeHeight(next->left)) {
                                        next = this->rotateLeftRight(next);
                                    }
                                    else next = this->rotateRight(next);
                                    p = next;
                                    next = next->parent;
                                }
                                else if(dh == 1) break;
                                else if(dh == 0) {
                                    p = next;
                                    next = next->parent;
                                }
                            }
                        }
                    }
                    replace: {
                        replacement->left = node->left;
                        replacement->right = node->right;
                        replacement->parent = node->parent;
                        replacement->nodeSize = (replacement->nodeSize & LS24B) | (node->nodeSize & ~LS24B);
                        replacement->usedSize = (replacement->usedSize & LS24B) | (node->usedSize & ~LS24B);
                        if(node == this->root) {
                            this->root = replacement;
                        }
                        else {
                            if(parent->left == node) {
                                parent->left = replacement;
                            }
                            else if(parent->right == node) {
                                parent->right = replacement;
                            }
                        }
                        if(replacement->left != nullptr) {
                            replacement->left->parent = replacement;
                        }
                        if(replacement->right != nullptr) {
                            replacement->right->parent = replacement;
                        }

                        goto updateHeight;
                    }
                }
                else {
                    ArrayTreeNode* child;
                    if(node->left) child = node->left;
                    else child = node->right;

                    if(parent->left == node) {
                        parent->left = child;
                        parent->nodeSize -= 1 << 24;
                        child->parent = parent;
                    }
                    else {
                        parent->right = child;
                        parent->usedSize -= 1 << 24;
                        child->parent = parent;
                    }
                    i8 dh = this->getLeftSubtreeHeight(parent) - this->getRightSubtreeHeight(parent);
                    if(dh > 1) {
                        if(this->getLeftSubtreeHeight(parent->left) < this->getRightSubtreeHeight(parent->left)) {
                            parent = this->rotateLeftRight(parent);
                        }
                        else parent = this->rotateRight(parent);

                        goto updateHeight;
                    }
                    else if(dh < -1) {
                        if(this->getLeftSubtreeHeight(parent->right) > this->getRightSubtreeHeight(parent->right)) {
                            parent = this->rotateRightLeft(parent);
                        }
                        else parent = this->rotateLeft(parent);

                        goto updateHeight;
                    }
                    else if(dh == 0) goto updateHeight;
                    else return;
                }
            }
            updateHeight: {
                if(parent == nullptr) return; //already balanced
                ArrayTreeNode* next = parent->parent;
                while(true) {
                    if(next == nullptr) break;
                    if(next->left == parent) {
                        next->nodeSize -= 1 << 24;
                        i8 dh = this->getLeftSubtreeHeight(next) - this->getRightSubtreeHeight(next);
                        if(dh < -1) {
                            if(this->getLeftSubtreeHeight(next->right) > this->getRightSubtreeHeight(next->right)) {
                                next = this->rotateRightLeft(next);
                            }
                            else {
                                next = this->rotateLeft(next);
                            }
                            parent = next;
                            next = next->parent;
                        }
                        else if(dh == -1) break;
                        else if(dh == 0) {
                            parent = next;
                            next = next->parent;
                        }
                    }
                    else {
                        next->usedSize -= 1 << 24;
                        i8 dh = this->getLeftSubtreeHeight(next) - this->getRightSubtreeHeight(next);
                        if(dh > 1) {
                            if(this->getLeftSubtreeHeight(next->left) < this->getRightSubtreeHeight(next->left)) {
                                next = this->rotateLeftRight(next);
                                parent = next;
                            }
                            else {
                                next = this->rotateRight(next);
                                parent = next;
                            }
                            next = next->parent;
                        }
                        else if(dh == 1) break;
                        else if(dh == 0) {
                            parent = next;
                            next = next->parent;
                        }
                    }
                }
            }
        }

        template<typename F> bool __remove(F compare, std::optional<T>* opt) {
            ArrayTreeNode* current = this->root;
            while(true) { //search
                if(current == nullptr) return false;
                int cmpLeft = compare(current->elements[0]);
                if(cmpLeft > 0) current = current->left;
                else {
                    i32 lastIndex = ((current->usedSize & LS24B) - minUsedSize) / sizeof(T) - 1;
                    int cmpRight = compare(current->elements[lastIndex]);
                    if(cmpRight < 0) current = current->right;
                    else { //search in current node
                        i32 left = 0, right = lastIndex, mid;
                        while(left < right) {
                            mid = (left + right) / 2;
                            if(compare(current->elements[mid]) < 0) left = mid + 1;
                            else right = mid - 1;
                        }
                        mid = (left + right) / 2;
                        if(compare(current->elements[mid]) == 0) {
                            if(opt) opt->emplace(std::move(current->elements[mid]));
                            else current->elements[mid].~T();
                            current->usedSize -= sizeof(T);
                            memmove(
                                (char*)&current->elements[mid],
                                (char*)&current->elements[mid + 1],
                                (lastIndex - mid) * sizeof(T)
                            );
                            break;
                        }
                        else return false;
                    }
                }
            }

            if((current->usedSize & LS24B) == minUsedSize) { //the node contains no elements
                //here we have a slight dilemma - we can either move the node
                //into `spareNodes` or delete it
                //for now they'll be moved into `spareNodes`
                this->__removeNode(current);
                this->__addNodeToSpares(current);
            }
            return true;
        }

        bool __remove(const T& element, std::optional<T>* opt) {
            ArrayTreeNode* current = this->root;
            while(true) {
                if(current == nullptr) return false;
                if(element < current->elements[0]) current = current->left;
                else {
                    i32 lastIndex = ((current->usedSize & LS24B) - minUsedSize) / sizeof(T) - 1;
                    if(element > current->elements[lastIndex]) current = current->right;
                    else {
                        i32 left = 0, right = lastIndex, mid;
                        while(left < right) {
                            mid = (left + right) / 2;
                            if(current->elements[mid] < element) left = mid + 1;
                            else right = mid - 1;
                        }
                        mid = (left + right) / 2;
                        if(element == current->elements[mid]) {
                            if(opt) opt->emplace(std::move(current->elements[mid]));
                            else current->elements[mid].~T();
                            current->usedSize -= sizeof(T);
                            memmove(
                                (char*)&current->elements[mid],
                                (char*)&current->elements[mid + 1],
                                (lastIndex - mid) * sizeof(T)
                            );
                            break;
                        }
                        else return false;
                    }
                }
            }

            if((current->usedSize & LS24B) == minUsedSize) {
                this->__removeNode(current);
                this->__addNodeToSpares(current);
            }
            return true;
        }

        /* void __test(ArrayTreeNode* root) {
            if(root == nullptr) return;
            cout << (int)this->getLeftSubtreeHeight(root) << ' ' << (int)this->getRightSubtreeHeight(root) << '\n';
            __test(root->left);
            __test(root->right);
        } */
    public:
        /**
         * @brief Construct a new ArrayTree object
         * with default node size of 4096 bytes.
         */
        ArrayTree() : nodeSize(4'096), root(createNode(nodeSize, nullptr)) {}
        /**
         * @brief Construct an ArrayTree object.
         * 
         * @param nodeSize size of each node of the tree in bytes,
         * 32 bytes are reserved for internal use in every node.
         * Passing a size which is unable to hold any element
         * causes a std::length_error exception to be thrown.
         * 
         * @throw
         * std::length_error if:
         * 
         * a) `nodeSize` < sizeof(type) + 32
         * 
         * b) `nodeSize` >= 2^24
         */
        ArrayTree(u32 nodeSize) {
            if(nodeSize < minUsedSize + sizeof(T)) {
                throw std::length_error(
                    "Attempted initialization of ArrayTree with"
                    "node size insufficient to hold any element"
                );
            }
            else if(nodeSize >= 1 << 24) {
                throw std::length_error(
                    "Attempted initialization of ArrayTree with"
                    "node size too large"
                );
            }
            this->nodeSize = nodeSize;
            this->root = createNode(nodeSize, nullptr);
        }

        ~ArrayTree() noexcept {
            this->deleteTree(this->spareNodes);
            this->deleteTree(this->root);
        }

        ArrayTree(const ArrayTree&) = delete; //for now no copying
        void operator=(const ArrayTree&) = delete; //same here

        ArrayTree(ArrayTree&& other) noexcept : root(other.root), spareNodes(other.spareNodes) {
            other.root = nullptr;
            other.spareNodes = nullptr;
        }

        /**
         * @brief Add an element to the tree, constructed in-place.
         * 
         * @tparam t element to add, constructed in-place 
         * If `t` has a non-trivial destructor, the function requires
         * it to have a defined move constructor, otherwise
         * expect a segfault.
         * 
         * Additionally, adding elements with duplicate keys
         * (for example adding two 0s) and later calling
         * `get(...)`/`remove(...)`/`delete(...)`
         * will return/return and remove/remove the first
         * entry found in the tree, so you as the user shall
         * keep this in mind.
         * 
         * @return reference to the added element
         */
       T& add(T t) {
            //Case 0: the tree is empty.
            if(this->root == nullptr) {
                this->root = this->createNode(nodeSize, nullptr);
                new (&this->root->elements[0]) T(std::move(t));
                return this->root->elements[0];
            }
            //All other cases are handles by this function here
            else {
                //this is so cursed...
                char data[sizeof(T)];
                T* tMoved = new (data) T(std::move(t));
                T* added = nullptr;
                this->__add(tMoved, this->root, &added);
                if(!added) throw std::logic_error("The program somehow managed to skew reality");
                return *added;
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
         * @return pointer to the element or NULL if not found.
         * 
         * Note that if the tree contains duplicate elements,
         * only the first one that's found is returned.
         */
        template<typename F> T* get(F compare) noexcept {
            ArrayTreeNode* current = this->root;
            while(true) {
                if(current == nullptr) return nullptr;
                int cmpLeft = compare(current->elements[0]);
                if(cmpLeft > 0) current = current->left;
                else {
                    i32 lastIndex = ((current->usedSize & LS24B) - minUsedSize) / sizeof(T);
                    int cmpRight = compare(current->elements[lastIndex - 1]);
                    if(cmpRight < 0) current = current->right;
                    else { //search in current node
                        i32 left = 0, right = lastIndex - 1, mid;
                        while(left < right) {
                            mid = (left + right) / 2;
                            if(compare(current->elements[mid]) < 0) left = mid + 1;
                            else right = mid - 1;
                        }
                        mid = (left + right) / 2;
                        if(compare(current->elements[mid]) == 0) return &current->elements[mid];
                        else return nullptr;
                    }
                }
            }
        }

        /**
         * @brief Removes the element identified by a comparison
         * function from the tree and returns it.
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
         * 
         * The comparison function is used here because you may want to remove an element
         * with its key, but don't have a reference to it
         * @return the object stored that was stored in the tree
         * or an empty std::optional.
         */
        template<typename F> std::optional<T> remove(F compare) {
            std::optional<T> opt;
            this->__remove(compare, &opt);
            return opt;
        }

        std::optional<T> remove(const T& element) {
            std::optional<T> opt;
            this->__remove(element, &opt);
            return opt;
        }

        /**
         * @brief Destroys the element identified by a comparison
         * function from the tree.
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
         * @return whether the element was deleted or not
         */
        template<typename F> bool destroy(F compare) { return this->__remove(compare, nullptr); }

        bool destroy(const T& element) { return this->__remove(element, nullptr); }

        /**
         * @brief Perform an operation on every element of the
         * tree according to the ordering.
         *
         * @param f function to perform on a reference to the element
         */
        template<typename F> void forEach(F f) { this->__forEachInOrder(f, this->root); }

        /**
         * @brief Perform an operation on every element of the
         * tree according to the ordering, recursively.
         * The effect is identical to forEachInOrder, but it may be
         * slower.
         *
         * @param f function to perform on a reference to the element
         */
        template<typename F> void forEachR(F f) { this->__forEachInOrderRecursive(f, this->root); }

        /**
         * @brief Perform an operation on every element of the
         * tree hierarchially.
         * 
         * Pre-order traversal in this kind of BST is somewhat of a misnomer.
         * In a standard BST traversing the tree in pre-order is topologically
         * sorted. However, since this BST implementation stores multiple
         * elements per node, the topological structure is more difficult
         * to recover.
         *
         * @param f function to perform on a reference to the element
         */
        template<typename F> void forEachHierarchically(F f) { this->__forEachPreOrder(f, this->root); }

        /**
         * @brief Perform an operation on every element of the
         * tree hierarchially, recursively.
         * The effect is identical to forEachHierarchically, but it may be
         * slower.
         * 
         * Pre-order traversal in this kind of BST is somewhat of a misnomer.
         * In a standard BST traversing the tree in pre-order is topologically
         * sorted. However, since this BST implementation stores multiple
         * elements per node, the topological structure is more difficult
         * to recover.
         *
         * @param f function to perform on a reference to the element
         */
        template<typename F> void forEachHierarchicallyR(F f) { this->__forEachPreOrderRecursive(f, this->root); }

        /**
         * @brief Perform an operation on every element of the
         * tree, starting from the bottom.
         * 
         * @param f function to perform on a reference to the element
         */
        template<typename F> void forEachBottomUp(F f) { this->__forEachPostOrder(f, this->root); }

        /**
         * @brief Perform an operation on every element of the
         * tree, starting from the bottom, recursively.
         * 
         * @param f function to perform on a reference to the element
         */
        template<typename F> void forEachBottomUpR(F f) { this->__forEachPostOrderRecursive(f, this->root); }

        /**
         * @brief Return a pointer to the element ordered first
         * in the tree.
         * 
         * @return pointer to element found or nullptr if the tree is empty 
         */
        T* min() { return  this->root != nullptr ? this->_min(this->root) : nullptr; }
        /**
         * @brief Return a pointer to the element ordered last
         * in the tree.
         * 
         * @return pointer to element found or nullptr if the tree is empty
         */
        T* max() { return  this->root != nullptr ? this->_max(this->root) : nullptr; }

        /**
         * @brief Return the height of the tree.
         * 
         * The reason why it's an 8-bit integer
         * is because there's absolutely no way that
         * a balanced BST would have more than 2^256 nodes.
         * And also because that's how it's stored.
         * 
         * @return height of the tree
         */
        u8 height() {
            if(this->root == nullptr) return 0;
            u8 l = this->getLeftSubtreeHeight(this->root);
            u8 r = this->getRightSubtreeHeight(this->root);
            return l > r ? l : r;
        }

        /* void test() {
            cout << this->root->elements[0] << '\n';
            this->__test(this->root);
        } */
};

class X {
    public:
        int x;  
        X(int x) : x(x) {}
        ~X() {
            cout << "Destruct\n";
        }
        X(const X& other) : x(other.x) {
            cout << "Copying\n";
        }
        X(X&& other) : x(other.x) {
            cout << "Moving\n";
        }
        X& operator=(const X& other) {
            x = other.x;
            cout << "Assigning\n";

            return *this;
        }

        bool operator<(const X& other) { return x < other.x; }
        bool operator>(const X& other) { return x > other.x; }
};

int main() {
    ArrayTree<int> tree(44);

    tree.add(50);
    tree.add(40);
    tree.add(80);
    tree.add(100);
    tree.add(90);
    tree.add(70);
    tree.add(60);

    tree.forEachHierarchically([](int& x) { cout << &x << " " << x << '\n'; });

    cout << *tree.min() << ", " << *tree.max() << "\n";
    cout << (int)tree.height() << "\n";

    tree.destroy([](int& other) { return other - 90; });
    tree.forEachHierarchically([](int& x) { cout << &x << " " << x << '\n'; });
    cout << '\n';
    std::optional<int> x = tree.remove([](int& other) { return other - 80; });
    tree.forEachHierarchically([](int& x) { cout << &x << " " << x << '\n'; });
    cout << *x << "\n";

    cout << (int)tree.height() << "\n";
    // ListArray<int> list(24);

    // list.append(1);
    // list.append(2);
    // list.append(3);

    // list.forEach([](int& x) { cout << x << endl; });
}