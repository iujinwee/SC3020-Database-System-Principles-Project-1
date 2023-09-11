//
// Created by Eugene Wee on 27/8/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H

#include "Record.h"
#include "iostream"
#include "memory"


class MemoryPool {
    const int recordSize = sizeof(Record);

    std::size_t totalMemorySize;        // Total size of the memory pool
    std::size_t currentMemorySize = 0;  // Current size of the memory pool
    std::size_t blockSize;              // Memory size of 1 block
    std::size_t currentBlockSize = 0;       // Memory size of current block
    std::size_t numUsedBlocks = 0;      // Count of used blocks
    std::size_t numUsedRecords = 0;     // Count of initialised records
    std::size_t numAccessedBlocks = 0;  // Count of accessed blocks

public:
    u_char *memPoolPtr;     // Pointer to memory pool
    u_char *nodeBlkPtr;     // Pointer B+ tree nodes
    u_char *dataBlkPtr;     // Pointer data records

    MemoryPool(int totalMemorySize, int blockSize);

    bool allocateBlock();

    RecordAddress allocate(std::size_t requiredSize);

    RecordAddress saveRecord(Record newRecord);

    static Record *loadRecord(RecordAddress record_address);

    static void displayRecord(RecordAddress record_address);

    [[nodiscard]] double getTotalMemory() const;

    [[nodiscard]] double getCurrentMemory() const;

    [[nodiscard]] std::size_t getNumUsedBlocks() const;

    [[nodiscard]] std::size_t getNumUsedRecords() const;

    [[nodiscard]] std::size_t getRecordSize() const;

    [[nodiscard]] std::size_t getNumRecordsInBlock() const;

    ~MemoryPool();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORYPOOL_H
