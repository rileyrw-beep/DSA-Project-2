
#ifndef DSA_PROJECT_2_BPLUSTREE_HPP
#define DSA_PROJECT_2_BPLUSTREE_HPP

#include "DataStructure.hpp"
#include <array>
#include <vector>
#include <cstdint>
#include <utility>


template <typename T, std::size_t Order>
class BPlusTree : public DataStructure<T> {

    struct BPNode {
        std::array<T, Order> data;
        std::array<BPNode*, Order+1> edges;
        std::size_t size;
        BPNode* next;
        BPNode() {
            size = 0;
            next = nullptr;
        }
        bool isLeaf() {return edges[0] == nullptr;}

    };

    BPNode* root;


    //helper functions
    void split(BPNode*& parent, std::size_t index);
    bool borrow(BPNode*& parent, std::size_t edgeIndex, std::size_t min);
    void merge(BPNode*& parent, std::size_t edgeIndex, std::size_t min);

    std::pair<BPNode*, bool> recursiveInsert(BPNode* root, T val);
    std::pair<BPNode*, bool> recursiveRemove(BPNode* root, T val);
    bool recursiveSearch(BPNode* root, T val);

    void destructorHelper(BPNode* root);
    BPNode* findNode(BPNode* root, T val);

public:

    BPlusTree();
    ~BPlusTree() override;

    bool insert(T val) override;
    bool remove(T val) override;
    bool search(T val) override;

    std::vector<T> searchRange(T start, T end);








};




#endif //DSA_PROJECT_2_BPLUSTREE_HPP