//
// Created by Eugene Wee on 27/8/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H

#include "../indexing/BPlusTree.h"
#include "Record.h"
#include "iostream"
#include "memory"

struct BPlusTreeNode;

const int record_size = sizeof(Record);

class Block {
public:
    int num_records = 0;
    int block_size;
    int size = 0;

    explicit Block(int size);
};

class MemoryPool {
    int total_memory_size;           // Total size of the memory pool
    int current_memory_size = 0;     // Current size of the memory pool
    int num_used_blocks = 0;         // Count of used blocks
    int num_used_records = 0;        // Count of initialised records
    int num_accessed_blocks = 0;     // Count of accessed blocks
    int block_size;

public:
    void *mem_pool_ptr;   // Pointer to memory pool
    Block *current_data_block;   // Pointer to current data block

    MemoryPool(int totalMemorySize, int blockSize);

    Block* allocateBlock();

    void* allocateRecord(Block* cur);

    void* allocateBPlusTreeNode();

    void saveBPlusTreeNode(BPlusTreeNode *newNode);

    void* saveRecord(Record newRecord);

    Record *loadRecord(void* recordAddress);

    void displayRecord(void* recordAddress);

    [[nodiscard]] double getTotalMemory() const;

    [[nodiscard]] double getCurrentMemory() const;

    [[nodiscard]] int getNumUsedBlocks() const;

    [[nodiscard]] int getNumUsedRecords() const;

    [[nodiscard]] int getNumAccessedBlocks() const;

    [[nodiscard]] int getRecordSize() const;

    [[nodiscard]] int getNumRecordsInBlock() const;

    ~MemoryPool();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H
