#include "HashTable.h"
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

HashTable::Node::Node(const std::string& k, int v){
    this->key = k;
    this->value = v;
    this->next = nullptr;
}

std::size_t HashTable::hash(const std::string& key) const {
   return std::hash<std::string>{}(key) % capacity;
}

void HashTable::rehash() {
    std::size_t newCapacity = capacity*2;
    Node** newBuckets = new Node*[newCapacity]();
    for (std::size_t i = 0; i < capacity; ++i){
    Node* curr = buckets[i];
        while (curr != nullptr){
            Node* next = curr->next;
            std::size_t idx = std::hash<std::string>{}(curr->key) % newCapacity;
            curr->next = newBuckets[idx];
            newBuckets[idx] = curr;
            curr = next;
    }
}
        delete[] buckets;
        buckets = newBuckets;
        capacity = newCapacity;
}
 HashTable::HashTable(std::size_t initialCapacity){

    buckets = new Node*[initialCapacity]();
    capacity = initialCapacity;
    count = 0;


}
HashTable::~HashTable(){
 clear();
 delete[] buckets;

}

void HashTable::insert(const std::string& key, int value){
    if (static_cast<float>(count + 1) / static_cast<float>(capacity) > MAX_LOAD_FACTOR)
{
    rehash();
}
        std::size_t idx = hash(key);
        Node* curr = buckets[idx];

while (curr != nullptr){
      if (curr->key == key){
      curr->value = value;
      return;
       }
curr = curr->next;
}
    Node* node = new Node(key, value);
    node->next = buckets[idx];
    buckets[idx] = node;
    ++count;


}
bool HashTable::remove(const std::string& key){
    std::size_t idx = hash(key);
    Node* curr = buckets[idx];
    Node* prev = nullptr;
    while (curr != nullptr){
    if (curr->key == key){
        if (prev) prev->next = curr->next;
        else buckets[idx] = curr->next;
        delete curr;
        --count;
        return true;
}
prev = curr;
curr = curr->next;

}
return false;
}

int HashTable::get(const std::string& key)const{
    std::size_t idx = hash(key);
    Node* curr = buckets[idx];
    while (curr!= nullptr){
if (curr->key == key) return curr->value;
        curr = curr->next;

}
    throw std::out_of_range("key not found");

}

bool HashTable::contains(const std::string& key)const{
    std::size_t idx = hash(key);
    Node* curr = buckets[idx];
while (curr != nullptr){
if (curr->key == key) return true;
curr = curr->next;
}
return false;
}

int& HashTable::operator[] (const std::string& key){
    if(!contains(key)) {insert(key, 0);}

std::size_t idx = hash(key);
Node* curr = buckets[idx];
while (curr != nullptr){
if (curr->key == key){ return curr->value;
}
    curr=curr->next;
}
    throw std::out_of_range("unexpected missing key");
}

std::size_t HashTable::size()const{
    return count;
}

bool HashTable::empty()const{
    return count == 0;
}

void HashTable::clear(){
    for (std::size_t i = 0; i < capacity; ++i){
    Node* curr = buckets[i];
while (curr != nullptr){
    Node* next = curr->next;
delete curr;
curr = next;
}
buckets[i] = nullptr;
}
count = 0;
}

void HashTable::print()const{
    for (std::size_t i = 0; i < capacity; ++i){
    Node* curr = buckets[i];
if (!curr) continue;
        std::cout << "[" << i << "] ";
        while (curr != nullptr) {
            std::cout << "(" << curr->key << " -> " << curr->value << ")";
            if (curr->next) std::cout << " -> ";
            curr = curr->next;
        }
        std::cout << "\n";

    }
}