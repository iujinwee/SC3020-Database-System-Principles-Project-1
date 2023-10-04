//
// Created by Eugene Wee on 10/9/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H

#include "../storage//Record.h"
#include "../storage/MemoryPool.h"
#include <vector>

const int BLOCK_SIZE = 400;
const int m = 3; // 22
struct MemoryPool;

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
public:
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

    ~BPlusTreeNode();

    void deleteKeyInLeafNode(MemoryPool *disk);

    int findIndexChild(BPlusTreeNode *childNode);

    BPlusTreeNode *ShiftKeysToBack(BPlusTreeNode *node, int num_indexes_shift);

    void deleteKeyInNonLeafNode();

    BPlusTreeNode *ShiftKeysToFront_leafNode(int start_index_remaining_keys, BPlusTreeNode *node);
};

class BPlusTree
{
private:
    static BPlusTreeKey getSmallestRightSubtree(BPlusTreeNode *node);

    void propagate(MemoryPool *disk, BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address);

    BPlusTreeNode *split(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address);

    BPlusTreeKey getInsertionBPKey(BPlusTreeNode *target, float key);

    void printRootKeys();

    

    [[nodiscard]] BPlusTreeNode *searchInsertionNode(float key) const;

    static void swapTemp(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address);

    static void swapNonLeafTemp(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address);

    static void addNewKey(BPlusTreeNode *node, int index, float key, int count, void *address);

    static void insertIntoLeafNode(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *recordAddress);

    static void insertIntoNonLeafNode(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address);

public:
    BPlusTreeNode *root = nullptr;
    int nodes = 0;
    int levels = 0;

    void displayTree();

    void printNode(BPlusTreeNode *node, int level);

    BPlusTreeNode *searchNode(float key);

    void insertKey(MemoryPool *disk, float key, void *recordAddress);

    int deleteKey(MemoryPool *disk, BPlusTreeNode *node, float dkey);

    void displayExp2Results();

    BPlusTreeKey findLB_rightSubTree(BPlusTreeNode *node, int index_key);

    BPlusTreeNode *findNextNonLeafNode(BPlusTreeNode *node);

    void checkKey(BPlusTreeNode *node);

    void BorrowFromRight(int num_keys_borrow, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode);

    void MergeWithRight_LeafNode(int num_keys_merge, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode);

    void MergeWithRight_NonLeafNode(int num_keys_merge, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode);

    ~BPlusTree();
};

#endif // SC3020_DATABASE_SYSTEM_PRINCIPLES_BPLUSTREE_H
