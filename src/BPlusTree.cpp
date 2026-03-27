#include "BPlusTree.hpp"

template<typename Type, std::size_t N>
void arrInsert(std::array<Type, N> &arr, Type val, std::size_t index, std::size_t& size) {
    if (index != size) {
        for (size_t i = size - 1; i >= index; i--) {
            arr[i + 1] = arr[i];
        }
    }
    arr[index] = val;
    size++;
}

template<typename Type, std::size_t N>
int arrBinarySearch(const std::array<Type, N> &arr, Type target, int left, int right, const std::size_t size) {
    if (left > right) return -1;

    int mid = (left + right) / 2;

    //if mid refers to target, return -1 because it is already in the tree
    if (arr[mid] == target) return -1;

    //check left cases
    if (target < arr[mid]) {
        //if you are smaller than current index and to the left of you is nothing then follow mid index
        if (mid - 1 < 0) return mid;
        //if you are smaller than current index but larger than the element left of current then follow mid index
        if (target > arr[mid - 1]) return mid;
        //else, continue searching
        return arrBinarySearch(arr, target, left, mid - 1);
    }

    //check right cases
    else {
        //if you are larger than current index and nothing is to the right of you then follow mid+1 index
        if (mid + 1 >= size - 1) return mid + 1;
        //if you are larger than the current index but smaller than right element then follow mid+1
        if (target < arr[mid + 1]) return mid + 1;
        //else, continue searching
        return arrBinarySearch(arr, target, mid + 1, right);
    }
}




//insertion:
template<typename T, std::size_t Order>
void BPlusTree<T, Order>::split(BPNode *&node, std::size_t index) {
    //get what we will call the LEFT node pointer
    BPNode *left = node->edges[index];

    //then we will make a new node which will go on the right and insert it
    auto *right = new BPNode();
    arrInsert(node->edges, right, index + 1, node->size);

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
        element = left->data[left->size-1];
        left->data[left->size-1] = T{};

    }

    //insert that middle element into the parent
    arrInsert(node->data, element, index, node->size);

}

template<typename T, std::size_t Order>
std::pair<typename BPlusTree<T, Order>::BPNode*, bool> BPlusTree<T, Order>::recursiveInsert(BPNode *node, T val) {
    int index = arrBinarySearch(node->data, val, 0, node->size-1, node->size);

    if (index < 0) return std::make_pair(node, false);

    //decision process of a single node
    if (node->isLeaf()) {
        arrInsert(node->data, val, index, node->size);
        return std::make_pair(node, true);
    }

    //else its an internal node

    //so index into the edges array and follow the path
    node->edges[index] = recursiveInsert(node->edges[index], val).first;

    //return from recursive function calls:
    //now we are unwinding the stack frames and going back up the tree to check if any invariants are broken

    //check the node you just altered
    BPNode *child = node->edges[index];

    //if data is not breaking the size invariant of the altered child then return early
    if (child == nullptr || child->size < child->data.size()) {
        return std::make_pair(node, true);
    }

    //if not, then we got a problem to fix
    split(node, index);

    //finally return root to ensure that our changes are linked up the tree.
    return std::make_pair(node, true);
}


template <typename T, std::size_t Order>
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
    auto* newParent = new BPNode();
    newParent->edges[0] = newRoot;
    split(newParent, 0);

    //now newParent is the new root and newRoot has successfully split into two
    root = newParent;
    return true;

}


//deletion:





//big 3 or what not ():
template<typename T, std::size_t Order>
BPlusTree<T, Order>::BPlusTree() {
    root = nullptr;
}

