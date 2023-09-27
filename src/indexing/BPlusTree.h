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
struct BPlusTreeKey
{
    float key;
    int count;
};

class BPlusTreeNode
{
    bool is_leaf;
    int size;
    BPlusTreeNode *next;   // Stores the next BPlusTree node
    BPlusTreeNode *parent; // Stores the parent BPlusTree node

    BPlusTreeKey keys[m]{};
    void *children[m + 1]{};

    friend class BPlusTree;

    explicit BPlusTreeNode(bool isLeafNode = false)
    {
        is_leaf = isLeafNode;
        next = nullptr;
        parent = nullptr;
        size = 0;
    }

    void deleteKeyInLeafNode();

    int findIndexChild(BPlusTreeNode *childNode);

    BPlusTreeNode* ShiftKeysToBack(BPlusTreeNode *node, int num_indexes_shift);

    void deleteKeyInNonLeafNode();

    BPlusTreeNode *ShiftKeysToFront(int start_index_remaining_keys, BPlusTreeNode *node);
};

class BPlusTree
{
private:
    static BPlusTreeKey getSmallestRightSubtree(BPlusTreeNode *node);

    void propagate(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address);

    BPlusTreeNode *split(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address);

    BPlusTreeKey getInsertionBPKey(BPlusTreeNode *target, float key);

    void printRootKeys();

    void printNode(BPlusTreeNode *node, int level);

    [[nodiscard]] BPlusTreeNode *searchInsertionNode(float key) const;

    static void swapTemp(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address);

    static void swapNonLeafTemp(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address);

    static void addNewKey(BPlusTreeNode *node, int index, float key, int count, void *address);

    static void insertIntoLeafNode(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *recordAddress);

    static void insertIntoNonLeafNode(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address);

    void MergeWithRight(int num_keys_merge, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode);

    void BorrowFromRight(int num_keys_borrow, BPlusTreeNode *leftNode,BPlusTreeNode *rightNode);

public:
    BPlusTreeNode *root = nullptr;
    int nodes;
    int levels;

    explicit BPlusTree();

    void displayTree();

    BPlusTreeNode *searchNode(float key);

    void insertKey(float key, void *recordAddress);

    int deleteKey(BPlusTreeNode *node, float dkey);

    void displayStatistics();

    BPlusTreeKey findLB_rightSubTree(BPlusTreeNode *node, int index_key);


    ~BPlusTree();
};

#endif // SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
