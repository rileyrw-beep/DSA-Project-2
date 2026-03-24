
#ifndef DSA_PROJECT_2_BPLUSTREE_HPP
#define DSA_PROJECT_2_BPLUSTREE_HPP

#include "DataStructure.hpp"
#include <array>
#include <cstdint>


template <typename T, std::size_t Order>
class BPlusTree : public DataStructure<T> {

    struct BPNode {
        bool isLeaf;
        std::array<T, Order-1> data;
        std::array<BPNode*, Order> edges;
        std::size_t size;
        BPNode* next;
    };

    BPNode* root;








};




#endif //DSA_PROJECT_2_BPLUSTREE_HPP