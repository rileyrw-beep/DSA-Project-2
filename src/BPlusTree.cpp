
#include "BPlusTree.hpp"

template <typename Type, std::size_t N>
void arrInsert(std::array<Type, N>& arr, Type val, std::size_t index, std::size_t size) {
    if (index != size) {
        for (size_t i = size-1; i >= index; i--) {
            arr[i+1] = arr[i];
        }
    }
    arr[index] = val;
    //dont forget to do BPNode->size++ after this if you manipulate BPNode->data
}

template <typename Type, std::size_t N>
int arrBinarySearch(const std::array<Type, N>& arr, Type target, int left, int right) {

    if (left > right) return -1;

    int mid = (left + right) / 2;

    //if mid refers to target, return -1 because it is already in the tree
    if (arr[mid] == target) return -1;

    //check left cases
    if (target < arr[mid]) {

        //if you are smaller than current index and to the left of you is nothing then follow mid index
        if (mid-1 < 0) return mid;
        //if you are smaller than current index but larger than the element left of current then follow mid index
        if (target > arr[mid-1]) return mid;
        //else, continue searching
        return arrBinarySearch(arr, target, left, mid-1);

    }

    //check right cases
    else {

        //if you are larger than current index and nothing is to the right of you then follow mid+1 index
        if (mid+1 >= arr.size()-1) return mid+1;
        //if you are larger than the current index but smaller than right element then follow mid+1
        if (target < arr[mid+1]) return mid+1;
        //else, continue searching
        return arrBinarySearch(arr, target, mid+1, right);
    }

}


template<typename T, std::size_t Order>
BPlusTree<T, Order>::BPlusTree() {
    root = nullptr;
}


template<typename T, std::size_t Order>
void BPlusTree<T, Order>::split(BPNode*& node, std::size_t index) {

    //get what we will call the LEFT node pointer
    BPNode* left = node->edges[index];

    //then we will make a new node which will go on the right and insert it
    auto* right = new BPNode();
    arrInsert(node->edges, right, index+1, node->size+1);

    //and finally the last thing we need is the middle index of the left child's data
    std::size_t midIndex = left->size / 2;

    //now to split

    //if it is a leaf then we just have to split it up and move data
    if (left->isLeaf()) {

        for (size_t i = midIndex+1; i < left->data.size(); i++) {
            right->data[i-(midIndex+1)] = left->data[i];
            left->data[i] = T{};
        }

    }
    //else it is an internal meaning we need to do data and edges
    else {



    }
}

template<typename T, std::size_t Order>
BPlusTree<T, Order>::BPNode *BPlusTree<T, Order>::recursiveInsert(BPNode *root, T val) {

    int index = arrBinarySearch(root->data, val, 0, root->size);

    if (index < 0) return root;

    //decision process of a single node
    if (root->isLeaf()) {
        arrInsert(root->data, val, index, root->size);
        root->size = root->size + 1;
        return root;
    }

    //else its an internal node

    //so index into the edges array and follow the path
    root->edges[index] = recursiveInsert(root->edges[index], val);

    //return from recursive function calls:
    //now we are unwinding the stack frames and going back up the tree to check if any invariants are broken

    //check the node you just altered
    BPNode* child = root->edges[index];

    //if data is not breaking the size invariant of the altered child then return early
    if (child->size < child->data.size()) {
        return root;
    }

    //if not, then we got a problem to fix
    split(root, index);



}





