#include "BPlusTree.hpp"

#include <ranges>

template<typename Type, std::size_t N>
void arrInsert(std::array<Type, N> &arr, Type val, std::size_t index, std::size_t size) {
    if (index != size) {
        for (size_t i = size - 1; i >= index; i--) {
            arr[i + 1] = arr[i];
        }
    }
    arr[index] = val;
}

template<typename Type, std::size_t N>
void arrErase(std::array<Type, N> &arr, std::size_t index, std::size_t size) {
    //if invalid index then return early
    if (index >= size) return;

    //start at the index and then move the index + 1 into it
    //go until you reach size-2 and then replace the size-1 with Type{}
    for (size_t i = index; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }

    arr[size - 1] = Type{};
}

template<typename Type, std::size_t N>
std::pair<int, bool> arrBinarySearch(const std::array<Type, N> &arr, Type target, int left, int right,
                                     std::size_t size) {
    if (left > right) return std::make_pair(-1, false);

    int mid = (left + right) / 2;

    //if mid refers to target, return -1 because it is already in the tree
    if (arr[mid] == target) return std::make_pair(mid, true);

    //check left cases
    if (target < arr[mid]) {
        //if you are smaller than current index and to the left of you is nothing then follow mid index
        if (mid - 1 < 0) return std::make_pair(mid, false);
        //if you are smaller than current index but larger than the element left of current then follow mid index
        if (target > arr[mid - 1]) return std::make_pair(mid, false);
        //else, continue searching
        return arrBinarySearch(arr, target, left, mid - 1);
    }

    //check right cases
    else {
        //if you are larger than current index and nothing is to the right of you then follow mid+1 index
        if (mid + 1 >= size - 1) return std::make_pair(mid + 1, false);
        //if you are larger than the current index but smaller than right element then follow mid+1
        if (target < arr[mid + 1]) return std::make_pair(mid + 1, false);;
        //else, continue searching
        return arrBinarySearch(arr, target, mid + 1, right);
    }
}


//insertion:
template<typename T, std::size_t Order>
void BPlusTree<T, Order>::split(BPNode *&parent, std::size_t index) {
    //get what we will call the LEFT node pointer
    BPNode *left = parent->edges[index];

    //then we will make a new node which will go on the right and insert it
    auto *right = new BPNode();
    arrInsert(parent->edges, right, index + 1, parent->size);

    //the element that will move up at the end
    T element;

    //and finally the last thing we need is the middle index of the left child's data
    std::size_t midIndex = left->size / 2;


    //now to split

    //if it is a leaf then we just have to split it up and move data
    if (left->isLeaf()) {
        right->next = left->next;
        left->next = right;

        for (size_t i = midIndex; i < left->data.size(); ++i) {
            //move from the left into the right
            right->data[i - midIndex] = left->data[i];
            left->data[i] = T{};

            //update sizes since we are deleting from the left and adding to the right
            left->size = left->size - 1;
            right->size = right->size + 1;
        }

        //get the element to be sent up
        element = right->data[0];
    }
    //else it is an internal meaning we need to do data and edges
    else {
        //move the data array but leave the middle in the left data because it is supposed to be moved up to the parent not left anywhere in the children
        for (size_t i = midIndex + 1; i < left->data.size(); ++i) {
            //move from the left into the right
            right->data[i - (midIndex + 1)] = left->data[i];
            left->data[i] = T{};

            //update sizes since we are deleting from the left and adding to the right
            left->size = left->size - 1;
            right->size = right->size + 1;
        }

        //move the edges
        for (size_t i = midIndex + 1; i < left->edges.size(); ++i) {
            //move from the left ot the right
            right->edges[i - (midIndex + 1)] = left->edges[i];
            left->edges[i] = nullptr;
            //no need to update sizes because size keeps track of Data and the size of Edges is calculated from size.
        }

        //grab that middle element to be sent up to the parent and purge it.
        element = left->data[left->size - 1];
        left->data[left->size - 1] = T{};
    }

    //insert that middle element into the parent
    arrInsert(parent->data, element, index, parent->size);
    parent->size = parent->size + 1;
}

template<typename T, std::size_t Order>
std::pair<typename BPlusTree<T, Order>::BPNode *, bool> BPlusTree<T, Order>::recursiveInsert(BPNode *node, T val) {
    auto [index, foundExactIndex] = arrBinarySearch(node->data, val, 0, node->size - 1, node->size);

    //if the item already exists in the tree, return early
    if (foundExactIndex) return std::make_pair(node, false);

    //decision process of a single node
    if (node->isLeaf()) {
        arrInsert(node->data, val, index, node->size);
        node->size = node->size + 1;
        return std::make_pair(node, true);
    }

    //else its an internal node

    //so index into the edges array and follow the path
    auto [newChild, didInsert] = recursiveInsert(node->edges[index], val);
    node->edges[index] = newChild;
    //if it did not insert, return false so we don't lose the false up the tree
    if (!didInsert) return std::make_pair(node, false);

    //return from recursive function calls:
    //now we are unwinding the stack frames and going back up the tree to check if any invariants are broken

    //check the node you just altered
    BPNode *child = node->edges[index];

    //if data is breaking the size invariant of the altered child then fix it
    if (child == nullptr || child->size >= child->data.size()) {
        split(node, index);
    }

    //finally return current Node to ensure that our changes are linked up the tree.
    return std::make_pair(node, true);
}


template<typename T, std::size_t Order>
bool BPlusTree<T, Order>::insert(T val) {
    //unpack pair from recursiveInsert call
    auto [newRoot, inserted] = recursiveInsert(root, val);

    //if it did not insert return false and make no change to root
    if (!inserted) return false;

    //if it did insert check the case that the root is breaking the data size invariant
    if (newRoot->size < newRoot->data.size()) {
        //if all good then
        root = newRoot;
        return true;
    }

    //if not, then we must handle the case that the root needs to split
    auto *newParent = new BPNode();
    newParent->edges[0] = newRoot;
    split(newParent, 0);

    //now newParent is the new root and newRoot has successfully split into two
    root = newParent;
    return true;
}


//deletion:
template<typename T, std::size_t Order>
bool BPlusTree<T, Order>::borrow(BPNode *&parent, std::size_t edgeIndex, std::size_t min) {
    //return true if successful borrow
    //return false if needs to merge

    BPNode *child = parent->edges[edgeIndex];

    //try borrow left
    if (edgeIndex > 0 && parent->edges[edgeIndex - 1]->size - 1 >= min) {
        BPNode *left = parent->edges[edgeIndex - 1];

        //logic for leaf
        if (child->isLeaf()) {
            //move last key of left into first spot of child
            arrInsert(child->data, left->data[left->size - 1], 0, child->size);
            child->size = child->size + 1;
            left->data[left->size - 1] = T{};
            left->size = left->size - 1;

            //replace the separator key in the parent
            parent->data[edgeIndex - 1] = child->data[0];
        }
        //logic for internal *rotate*
        else {
            arrInsert(child->data, parent->data[edgeIndex - 1], 0, child->size);
            child->size = child->size + 1;
            parent->data[edgeIndex - 1] = left->data[left->size - 1];
            left->data[left->size - 1] = T{};
            arrInsert(child->edges, left->edges[left->size], 0, child->size);
            left->edges[left->size] = nullptr;
            left->size = left->size - 1;
        }

        return true;
    }
    //try borrow right
    if (edgeIndex + 1 < parent->edges.size() && parent->edges[edgeIndex + 1]->size - 1 >= min) {
        BPNode *right = parent->edges[edgeIndex + 1];

        //if leaf
        if (child->isLeaf()) {
            //copy over right's first
            child->data[child->size] = right->data[0];
            child->size = child->size + 1;

            //delete right's first
            arrErase(right->data, 0, right->size);
            right->size = right->size - 1;

            //steal right's new first
            parent->data[edgeIndex] = right->data[0];
        }
        //if internal (rotate)
        else {
            child->data[child->size] = parent->data[edgeIndex];
            parent->data[edgeIndex] = right->data[0];
            arrErase(right->data, 0, right->size);

            child->edges[child->size + 1] = right->edges[0];
            arrErase(right->edges, 0, right->size + 1);

            child->size = child->size + 1;
            right->size = right->size - 1;
        }

        return true;
    }

    //else, merge
    return false;
}

template<typename T, std::size_t Order>
void BPlusTree<T, Order>::merge(BPNode *&parent, std::size_t edgeIndex, std::size_t min) {
    BPNode *child = parent->edges[edgeIndex];

    //try merge left
    if (edgeIndex > 0) {
        BPNode *left = parent->edges[edgeIndex - 1];

        if (!child->isLeaf()) {
            //copy over separator into one of the children
            left->data[left->size] = parent->data[edgeIndex - 1];
            left->size = left->size + 1;
        }

        std::size_t tempSize = left->size;
        for (size_t i = left->size; i < tempSize + child->size; i++) {
            left->data[i] = child->data[i - tempSize];
            child->data[i - tempSize] = T{};

            if (!child->isLeaf()) {
                left->edges[i+1] = child->edges[i - tempSize];
                child->edges[i - tempSize] = nullptr;
            }

            left->size = left->size + 1;
        }
        if (!child->isLeaf()) {
            left->edges[left->size+1] = child->edges[child->size];
            child->edges[child->size] = nullptr;
        }

        //move keys:
        if (child->isLeaf()) {
            left->next = child->next;
            child->next = nullptr;
        }

        arrErase(parent->data, edgeIndex - 1, parent->size);

        delete child;

        arrErase(parent->edges, edgeIndex, parent->size + 1);
        parent->size = parent->size - 1;
    }

    //if not merge right
    else {
        BPNode *right = parent->edges[edgeIndex + 1];

        if (!child->isLeaf()) {
            //copy over separator into one of the children
            child->data[child->size] = parent->data[edgeIndex];
            child->size = child->size + 1;
        }

        std::size_t tempSize = child->size;
        for (size_t i = child->size; i < tempSize + right->size; i++) {

            child->data[i] = right->data[i - tempSize];
            right->data[i - tempSize] = T{};

            if (!child->isLeaf()) {
                child->edges[i+1] = right->edges[i - tempSize];
                right->edges[i - tempSize] = nullptr;
            }

            child->size = child->size + 1;
        }

        if (!child->isLeaf()) {
            child->edges[child->size+1] = right->edges[right->size];
            right->edges[right->size] = nullptr;
        }

        //move keys
        if (child->isLeaf()) {
            child->next = right->next;
            right->next = nullptr;
        }

        arrErase(parent->data, edgeIndex, parent->size);

        delete right;

        arrErase(parent->edges, edgeIndex+1, parent->size + 1);
        parent->size = parent->size - 1;
    }
}

template<typename T, std::size_t Order>
std::pair<typename BPlusTree<T, Order>::BPNode *, bool> BPlusTree<T, Order>::recursiveRemove(BPNode *node, T val) {
    //search through current node to get either the index of val or the index you must follow to edges
    auto [dataIndex, foundExactIndex] = arrBinarySearch(node->data, val, 0, node->size - 1, node->size);

    int edgeIndex = foundExactIndex ? dataIndex + 1 : dataIndex;

    //if its a leaf
    if (node->isLeaf()) {
        //and you have the actual index then delete it
        if (foundExactIndex) {
            arrErase(node->data, dataIndex, node->size);
            node->size = node->size - 1;
            return std::make_pair(node, true);
        }
        //if not, then there is nothing to delete so just return early.
        else return std::make_pair(node, false);
    }

    //if not, then we have an internal
    //so index into the edges array and follow the path through recursive call
    auto [newChild, didRemove] = recursiveRemove(node->edges[edgeIndex], val);
    node->edges[edgeIndex] = newChild;
    if (!didRemove) return std::make_pair(node, false);
    //return from recursive call

    //check the node you just altered
    BPNode *child = node->edges[edgeIndex];
    std::size_t minSize = child->isLeaf() ? (Order + 1) / 2 : (Order + 1) / 2 - 1;

    //if the node underflowed then fix it
    if (child == nullptr || child->size < minSize) {
        if (!borrow(node, edgeIndex, minSize)) merge(node, edgeIndex, minSize);
    }

    //finally once that is resolved return the node to link it with the rest of the tree.
    return std::make_pair(node, true);
}

template<typename T, std::size_t Order>
bool BPlusTree<T, Order>::remove(T val) {
    auto [newRoot, didRemove] = recursiveRemove(root, val);

    if (!didRemove) return false;

    //if it did remove check the case that the root has underflow
    if (newRoot->size < 1) {
        if (newRoot->isLeaf()) {
            delete root;
            root = nullptr;
        }
        else {
            root = newRoot->edges[0];
            delete newRoot;
        }
    }
    else {
        root = newRoot;
    }

    return true;
}

template<typename T, std::size_t Order>
bool BPlusTree<T, Order>::recursiveSearch(BPNode* node, T val) {
    auto [index, foundExactIndex] = arrBinarySearch(node->data, val, 0, node->size - 1, node->size);

    if (node->isLeaf()) return foundExactIndex;

    return recursiveSearch(node->edges[index], val);

}

template<typename T, std::size_t Order>
void BPlusTree<T, Order>::destructorHelper(BPNode *node) {

    if (node == nullptr) return;

    if (!node->isLeaf()) {
        for (size_t i = 0; i < node->size+1; i++) {
            destructorHelper(node->edges[i]);
        }
    }

    delete node;

}


template<typename T, std::size_t Order>
typename BPlusTree<T, Order>::BPNode * BPlusTree<T, Order>::findNode(BPNode *node, T val) {
    auto [index, foundExactIndex] = arrBinarySearch(node->data, val, 0, node->size - 1, node->size);

    if (node->isLeaf()) return node;

    return findNode(node->edges[index], val);
}

template<typename T, std::size_t Order>
bool BPlusTree<T, Order>::search(T val) {

    return recursiveSearch(root, val);

}

template<typename T, std::size_t Order>
std::vector<T> BPlusTree<T, Order>::searchRange(T start, T end) {
    auto current = findNode(root, start);

    auto [startIndex, didFind] = arrBinarySearch(current->data, start, 0, current->size - 1, current->size);

    std::vector<T> v;

    if (startIndex < 0) return v;

    while (current->data[startIndex] < start) {
        ++startIndex;
    }

    for (size_t i = startIndex; i < current->size; i++) {
        if (current->data[i] > end) return v;
        v.push_back(current->data[i]);
    }

    current = current->next;
    while (current != nullptr) {
        for (size_t i = 0; i < current->size; i++) {
            if (current->data[i] > end) return v;
            v.push_back(current->data[i]);
        }
        current = current->next;
    }

    return v;

}

//constructor
template<typename T, std::size_t Order>
BPlusTree<T, Order>::BPlusTree() {
    root = nullptr;
}

//destructor
template<typename T, std::size_t Order>
BPlusTree<T, Order>::~BPlusTree() {

    destructorHelper(root);

}
