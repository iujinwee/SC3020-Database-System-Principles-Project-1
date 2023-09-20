//
// Created by Eugene Wee on 10/9/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H

#include "../storage//Record.h"
#include <vector> 

const int BLOCK_SIZE = 400;

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
    BPlusTreeNode* next;  // Stores the next BPlusTree node
    BPlusTreeNode* parent;  // Stores the previous BPlusTree node

    std::vector<BPlusTreeKey> keys;         // Stores the BPlusTreeKey structure
    std::vector<BPlusTreeNode *> children;  // Stores a list of BPlusTreeNodes children


    friend class BPlusTree;

    explicit BPlusTreeNode(bool isLeafNode = false) {
        is_leaf = isLeafNode;
        next = nullptr;
        parent = nullptr;
    }
};

class BPlusTree {
private:
    BPlusTreeNode *root;
    int m; // Maximum keys that can be stored

    static BPlusTreeKey newKey(float key, void *recordAddress);

    static void insertIntoLeafNode(BPlusTreeNode *leafNode, float key, void *recordAddress);

    static BPlusTreeNode *splitLeafNode(BPlusTreeNode *node, float key, void *recordAddress);

    void propagateUpwards(BPlusTreeNode *oldNode, BPlusTreeNode *newNode);

public:
    explicit BPlusTree(int nodeSize);

    void displayTree();

    BPlusTreeNode *searchNode(float key);

    void insertKey(float key, void *recordAddress);

    static void deleteKey(float key);

    ~BPlusTree();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
