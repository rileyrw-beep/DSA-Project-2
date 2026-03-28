#ifndef SRC_HASHTABLE_H
#define SRC_HASHTABLE_H

#include <string>
#include <cstddef>

class HashTable {
    struct Node{
    std::string key;
    int value;
    Node* next;
    Node(const std::string& key, int value);
    };

    Node** buckets;
    std::size_t capacity;
    std::size_t count;
    static constexpr float MAX_LOAD_FACTOR = 0.75f;

    std::size_t hash(const std::string& key);
    void rehash();

    public:
    explicit HashTable(std::size_t initialCapacity=16);
    ~HashTable();

    void insert(const std::string& key, int value);
    bool remove(const std::string& key);
    bool contains(const std::string& key) const;
    int get(const std::string& key) const;
    int& operator[](const std::string& key);
    std::size_t size() const;
    bool empty() const;
    void print () const;
    void clear();

};

#endif