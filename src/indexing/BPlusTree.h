//
// Created by Eugene Wee on 10/9/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H

#include "../storage//Record.h"

const int BLOCK_SIZE = 400;

/*
 *  Key structure stores the
 *  1) Key value
 *  2) Memory addresses of the records with the same key value
 */
struct BPlusTreeKey {
    int key{};
    vector<void *> record_addresses;
};

class BPlusTreeNode {
    bool isLeaf;

    std::vector<BPlusTreeKey> keys;         // Stores a list of BPlusTreeKeys
    std::vector<BPlusTreeNode *> children;  // Stores a list of BPlusTreeNodes children

    friend class BPlusTree;

    explicit BPlusTreeNode(bool isLeafNode = false) {
        isLeaf = isLeafNode;
    }
};

class BPlusTree {
private:
    BPlusTreeNode *root;
    int m;

    static BPlusTreeKey newKey(int key, void *recordAddress);

    static void insertIntoLeafNode(BPlusTreeNode *leafNode, int key, void *recordAddress);

    static BPlusTreeNode *splitLeafNode(BPlusTreeNode *node, int key, void *recordAddress);

    void propagateUpwards(BPlusTreeNode *oldNode, BPlusTreeNode *newNode);

public:
    explicit BPlusTree(int nodeSize);

    void displayTree();

    BPlusTreeNode *searchNode(int key);

    void insertKey(int key, void *recordAddress);

    static void deleteKey(int key);

    ~BPlusTree();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
