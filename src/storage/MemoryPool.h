//
// Created by Eugene Wee on 27/8/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H

#include "../indexing/BPlusTree.h"
#include "Record.h"
#include "Block.h"
#include "iostream"
#include "memory"
#include <vector>


struct BPlusTreeNode;
struct BPlusTree;
struct Block;

const int RECORD_SIZE = sizeof(Record);
const int BYTE = 8;


class MemoryPool {
    int total_memory_size;           // Total size of the memory pool
    int current_memory_size = 0;     // Current size of the memory pool
    int num_used_blocks = 0;         // Count of used blocks
    int num_used_data_blocks = 0;         // Count of used blocks
    int num_used_records = 0;        // Count of initialised records
    int num_accessed_blocks = 0;     // Count of accessed blocks
    int block_size;

public:
    void *mem_pool_ptr;   // Pointer to memory pool
    void *bplustree_ptr;  // Pointer to BPlusTree
    Block *current_data_block;   // Pointer to current data block

    MemoryPool(int totalMemorySize, int blockSize);

    vector <void *> BFSearchAddresslist;

    Block* allocateBlock();

    void allocateRecord();

    void* saveRecord(Record newRecord);

    void saveBPlusTreeNode(BPlusTreeNode *newNode);

    void saveBPlusTree(BPlusTree tree);

    Record *loadRecord(void* recordAddress);

    void displayRecord(void* recordAddress);

    void getAverage();

    float loadRecordfcg3(void *recordAddress);

    int getBlockID(void *recordAddress);

    // vector <void *> block_ptr_list ;  // Kelly

    //void getBlocksAccessedByForce(float lower, float upper) ;  

    int getBlocksAccessedByBruteForce(float lowerkey, float upperkey);

    [[nodiscard]] double getTotalMemory() const;

    [[nodiscard]] double getCurrentMemory() const;

    [[nodiscard]] int getNumUsedDataBlocks() const;

    [[nodiscard]] int getNumUsedRecords() const;

    [[nodiscard]] int getNumAccessedBlocks() const;

    [[nodiscard]] int getRecordSize() const;

    [[nodiscard]] int getNumRecordsInBlock() const;

    ~MemoryPool();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H
