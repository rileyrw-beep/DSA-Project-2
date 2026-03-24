
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


template<typename T, std::size_t Order>
BPlusTree<T, Order>::BPlusTree() {
    root = nullptr;
}





