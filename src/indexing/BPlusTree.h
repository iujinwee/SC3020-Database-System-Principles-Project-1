//
// Created by Eugene Wee on 10/9/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H

#include "../storage//Record.h"

struct BPlusTreeNode {
    bool isLeafNode;
    int size;
    BPlusTreeNode *nextLeaf;

    vector<int> keys;
    vector<vector<int>> values;
    vector<BPlusTreeNode *> children;

    explicit BPlusTreeNode(bool isLeaf = false) {
        size = 0;
        isLeafNode = isLeaf;
        nextLeaf = nullptr;
    }
};

class BPlusTree {
private:
    int degree;
    BPlusTreeNode *root;

    void inOrderTransversal(BPlusTreeNode *node);

    void insertNonFull(BPlusTreeNode *node, int key, int value);

    void splitChild(BPlusTreeNode *parentNode, int childIndex);

public:
    explicit BPlusTree(int degree);

    void displayTree();

    BPlusTree searchTree();

    bool insertNode(int key, int value);

    bool deleteNode();

    ~BPlusTree();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
