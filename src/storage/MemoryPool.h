//
// Created by Eugene Wee on 27/8/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H

#include "Record.h"
#include "iostream"
#include "memory"


class MemoryPool {
    const int record_size = sizeof(Record);

    std::size_t total_memory_size;           // Total size of the memory pool
    std::size_t current_memory_size = 0;     // Current size of the memory pool
    std::size_t block_size;                  // Memory size of 1 block
    std::size_t current_block_size = 0;      // Memory size of current block
    std::size_t num_used_blocks = 0;         // Count of used blocks
    std::size_t num_used_records = 0;        // Count of initialised records
    std::size_t num_accessed_blocks = 0;     // Count of accessed blocks

public:
    u_char *mem_pool_ptr;   // Pointer to memory pool
    u_char *b_tree_ptr;     // Pointer B+ tree nodes
    u_char *data_ptr;       // Pointer data records

    MemoryPool(int totalMemorySize, int blockSize);

    bool allocateBlock();

    RecordAddress allocate(std::size_t requiredSize);

    RecordAddress saveRecord(Record newRecord);

    static Record *loadRecord(RecordAddress recordAddress);

    static void displayRecord(RecordAddress recordAddress);

    [[nodiscard]] double getTotalMemory() const;

    [[nodiscard]] double getCurrentMemory() const;

    [[nodiscard]] std::size_t getNumUsedBlocks() const;

    [[nodiscard]] std::size_t getNumUsedRecords() const;

    [[nodiscard]] std::size_t getRecordSize() const;

    [[nodiscard]] std::size_t getNumRecordsInBlock() const;

    ~MemoryPool();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H
