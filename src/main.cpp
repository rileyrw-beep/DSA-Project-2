#include <iostream>
#include <sstream>
#include "SkipList.hpp"
#include "BPlusTree.hpp"
#include "BPlusTree.cpp"
#include "HashTable.h"
#include "chrono"
#include <array>
#include <cstdint>

enum DataStructureType
{
    HASHTABLE,
    SKIPLIST,
    BPTREE
};

// Fills containers with dummy data
void propagate(BPlusTree<int, 100> &b, HashTable &ht, SkipList<int> sl)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(1, 10000000);

    unsigned long long count = 0;
    while (count < 100000)
    {
        auto rand = distribution(generator);
        if (b.insert(rand))
        {
            ht.insert(std::to_string(rand), 1);
            sl.insert(rand);
            count++;
        }
    }
}

int main()
{
    std::cout << "Starting\n";
    bool isRunning = true;

    // Manges shell state
    std::array<DataStructureType, 3> dataTypes = {DataStructureType::BPTREE, DataStructureType::SKIPLIST, DataStructureType::HASHTABLE};
    std::uint8_t typeIndex = 0;
    DataStructureType type = DataStructureType::BPTREE;

    // Init empty containers
    BPlusTree<int, 100> bpTree = BPlusTree<int, 100>();
    SkipList<int> skipList = SkipList<int>();
    HashTable hashTable = HashTable();
    // Populate containers
    propagate(bpTree, hashTable, skipList);

    // Main loop
    while (isRunning)
    {
        //
        std::cout << "cmd > ";
        std::string res;
        std::getline(std::cin, res);

        std::stringstream ss(res);

        std::string cmd;
        ss >> cmd;

        std::string arg;
        std::vector<std::string> args;

        while (ss >> arg)
        {
            args.push_back(arg);
        }

        if (cmd.size() == 0)
            continue;

        // Handle Meta Commands
        if (cmd.at(0) == '.')
        {
            if (cmd == ".switch")
            {
                typeIndex += 1;
                typeIndex %= 3;
                type = dataTypes.at(typeIndex);
                switch (type)
                {
                case DataStructureType::BPTREE:
                    std::cout << "Now using: B+ Tree" << std::endl;
                    break;
                case DataStructureType::SKIPLIST:
                    std::cout << "Now using: Skip List" << std::endl;
                    break;
                case DataStructureType::HASHTABLE:
                    std::cout << "Now using: Hash Table" << std::endl;
                    break;
                }
            }
            else if (cmd == ".exit")
            {
                isRunning = false;
                std::cout << "Bye bye!";
                continue;
            }
            else if (cmd == ".help")
            {
                std::cout << " -- META COMMANDS --" << std::endl;
                std::cout << ".switch - Switches active datastructure. Rotates through b+ tree, skip list, and hash table" << std::endl;
                std::cout << ".exit - Exits the program" << std::endl;
                std::cout << " -- DATASTRUCTURE COMMANDS --" << std::endl;
                std::cout << "insert [int value] - Inserts value into the list" << std::endl;
                std::cout << "search [int value] - Prints true if value is in the structure, prints false if value is not found" << std::endl;
                std::cout << "remove [int value] - Removes value if it exists in the tree, prints false if the value doesn't exist in the tree" << std::endl;
            }
            else
            {
                std::cout << "Unknown command: " << cmd << " - Type .help to list all commands" << std::endl;
            }
            continue;
        }

        // Actual command
        if (args.size() != 1)
        {
            std::cout << "Invalid Arguments. Expected 1 argument, received " << args.size() << std::endl;
        }
        auto start = std::chrono::steady_clock::now();

        // Handle Insert, Search, Remove
        int val = std::stoi(args.at(0));
        if (cmd == "insert")
        {
            bool inserted = false;
            if (type == DataStructureType::BPTREE)
                inserted = bpTree.insert(val);
            if (type == DataStructureType::SKIPLIST)
                inserted = skipList.insert(val);
            if (type == DataStructureType::HASHTABLE)
            {

                std::string key = std::to_string(val);
                if (hashTable.contains(key))
                    inserted = false;
                else
                {
                    hashTable.insert(key, 1);
                    inserted = true;
                }
            }
            if (inserted)
                std::cout << "Successfully inserted value " << std::to_string(val) << std::endl;
            else
                std::cout << "Failed to insert value " << std::to_string(val) << " - Value already exists" << std::endl;
        }
        if (cmd == "remove")
        {
            bool removed = false;
            if (type == DataStructureType::BPTREE)
                removed = bpTree.remove(val);
            if (type == DataStructureType::SKIPLIST)
                removed = skipList.remove(val);
            if (type == DataStructureType::HASHTABLE)
            {
                std::string key = std::to_string(val);
                if (!hashTable.contains(key))
                    removed = false;
                else
                {
                    hashTable.remove(key);
                    removed = true;
                }
            }
            if (removed)
                std::cout << "Successfully inserted value " << std::to_string(val) << std::endl;
            else
                std::cout << "Failed to remove value " << std::to_string(val) << std::endl;
        }
        if (cmd == "search")
        {
            bool searched = false;
            if (type == DataStructureType::BPTREE)
                searched = bpTree.search(val);
            if (type == DataStructureType::SKIPLIST)
                searched = skipList.search(val);
            if (type == DataStructureType::HASHTABLE)
            {

                std::string key = std::to_string(val);
                searched = hashTable.contains(key);
            }
            if (searched)
                std::cout << "Structure contains value " << std::to_string(val) << std::endl;
            else
                std::cout << "Structure does not contain value " << std::to_string(val) << std::endl;
        }
        auto end = std::chrono::steady_clock::now();

        // Compute time elapsed
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Time elapsed: " << duration.count() << " ms" << std::endl;
    }
    return 0;
}
