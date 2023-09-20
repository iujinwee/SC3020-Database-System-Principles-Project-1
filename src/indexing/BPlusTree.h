//
// Created by Eugene Wee on 10/9/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H

#include "../storage//Record.h"
#include <vector> 

const int BLOCK_SIZE = 400;
const int m = 23;

/*
 *  Key structure stores the
 *  1) Key value
 *  2) Count of keys (to handle duplicate keys)
 */
struct BPlusTreeKey {
    float key;
    int count;
};

class BPlusTreeNode {
    bool is_leaf;
    int size;
    BPlusTreeNode* next;  // Stores the next BPlusTree node
    BPlusTreeNode* parent;  // Stores the parent BPlusTree node

    BPlusTreeKey keys[m];
    void* children[m+1];

//    void* children [10];  // Stores a list of BPlusTreeNodes children
//    std::vector<BPlusTreeKey> keys;         // Stores the BPlusTreeKey structure

    friend class BPlusTree;

    explicit BPlusTreeNode(bool isLeafNode = false) {
        is_leaf = isLeafNode;
        next = nullptr;
        parent = nullptr;
        size = 0;
    }
};

class BPlusTree {
private:
    BPlusTreeNode *root = nullptr;

    BPlusTreeNode* searchInsertionNode(float key);

    static void addNewKey(BPlusTreeNode *node, int index, float key, int count, void* address);

    static void insertIntoLeafNode(BPlusTreeNode *leafNode, float key, void *recordAddress);

    static BPlusTreeNode *splitLeafNode(BPlusTreeNode *node, float key, void *recordAddress);

    void propagateUpwards(BPlusTreeNode *oldNode, BPlusTreeNode *newNode);

public:
    explicit BPlusTree();

    void displayTree();

    BPlusTreeNode *searchNode(float key);

    void insertKey(float key, void *recordAddress);

    static void deleteKey(float key);

    ~BPlusTree();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
