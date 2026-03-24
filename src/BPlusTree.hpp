
#ifndef DSA_PROJECT_2_BPLUSTREE_HPP
#define DSA_PROJECT_2_BPLUSTREE_HPP

#include "DataStructure.hpp"
#include <array>
#include <vector>
#include <cstdint>


template <typename T, std::size_t Order>
class BPlusTree : public DataStructure<T> {

    struct BPNode {
        std::array<T, Order> data;
        std::array<BPNode*, Order+1> edges;
        std::size_t size;
        BPNode* next;
        bool isLeaf() {return edges[0] == nullptr;}
    };

    BPNode* root;

    //helper functions
    std::size_t findInsertIndex(BPNode* node, T val);
    BPNode* insertHelper(BPNode* root, T val);
    BPNode* removeHelper(BPNode* root, T val);

public:

    BPlusTree();
    ~BPlusTree();

    bool insert(T val);
    bool remove(T val);
    bool search(T val);

    std::vector<BPNode*> traverseInorder();
    std::vector<BPNode*> searchRange(T start, T end);








};




#endif //DSA_PROJECT_2_BPLUSTREE_HPP