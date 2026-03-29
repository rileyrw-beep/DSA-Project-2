#include "DataStructure.hpp"
#include <vector>
#include <cstdint>
#include <random>

template <typename T>
class SkipList : public DataStructure<T>
{
    struct Node
    {
        T data;
        Node *right;
        Node *left;
        Node *down;
        Node *up;
        Node(T data) : right(nullptr), left(nullptr), up(nullptr), down(nullptr), data(data) {};
    };

    Node *head;
    std::size_t height;

public:
    SkipList() : head(nullptr), height(0) {}

    bool insert(T val)
    {
        if (!this->head)
        {
            this->head = new Node(val);
            this->height = 1;

            return true;
        }
        Node *current = this->head;

        while (current->right && val >= current->right->data)
            current = current->right;
        while (current->down)
        {
            current = current->down;
            while (current->right && val >= current->right->data)
                current = current->right;
        }

        Node *node = new Node(val);
        node->right = current->right;
        if (node->right)
            node->right->left = node;
        current->right = node;
        node->left = current;

        static std::mt19937 engine(82);
        std::uniform_int_distribution dist(0, 1);

        Node *p = current;
        Node *prev = node;
        std::size_t h = 1;

        while (dist(engine) >= 0.5)
        {
            h++;
            // Add layer if needed
            if (h > this->height)
            {
                this->height++;
                Node *oldHead = this->head;
                this->head = new Node(this->head->data);
                this->head->down = oldHead;
                oldHead->up = this->head;
            }

            // create node on top
            Node *topNode = new Node(val);
            topNode->down = prev;
            prev->up = topNode;

            // find pivot
            while (!p->up && p->left) // Shouldn't have to include p->left, but just in case
                p = p->left;
            if (!p->up)
                throw std::runtime_error("idk how this happened. pivot wants to go higher but no higher node exists.");
            p = p->up;

            // Insert new level's node after pivot
            topNode->right = p->right;
            if (topNode->right)
                topNode->right->left = topNode;
            p->right = topNode;
            topNode->left = p;

            // rerun on topNode if needed
            prev = topNode;
        }
        return true;
    }

    bool remove(T val)
    {
        Node *current = this->head;

        while (current->right && val >= current->right->data)
            current = current->right;
        while (current->down)
        {
            current = current->down;
            while (current->right && val >= current->right->data)
                current = current->right;
        }

        if (current->data != val)
            return false;

        while (current)
        {
            current->left->right = current->right;
            if (current->right)
                current->right->left = current->left;

            Node *up = current->up;
            delete current;
            current = up;
        }

        return true;
    }

    bool search(T val)
    {
        Node *current = this->head;

        while (current->right && val >= current->right->data)
            current = current->right;
        while (current->down)
        {
            current = current->down;
            while (current->right && val >= current->right->data)
                current = current->right;
        }

        if (current->data == val)
            return true;
        return false;
    }

    void print()
    {
        Node *current = this->head;
        while (current->down)
        {
            current = current->down;
        }
        while (current)
        {
            Node *node = current;
            while (node)
            {
                std::cout << node->data << " ";
                node = node->up;
            }
            std::cout << std::endl;
            current = current->right;
        }
    }

    ~SkipList()
    {
        Node *current = this->head;
        while (current->down)
        {
            current = current->down;
        }
        while (current)
        {
            Node *node = current;
            Node *right = current->right;
            while (node)
            {
                Node *up = node->up;
                delete node;
                node = up;
            }
            current = right;
        }
    }
};