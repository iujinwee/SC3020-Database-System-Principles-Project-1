//
// Created by Eugene Wee on 27/8/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORY_POOL_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORY_POOL_H

#include "iostream"

class MemoryPool {
private:
    int totalMemPoolSize;
    int blockSize;
    int recordSize;
    int currentMemPoolSize;
    int currentBlockSize;
    int memPoolAvailableBlocks;
    int memPoolUsedBlocks;
    int memPoolUsedRecords;

public:
    int *memPoolPtr;
    int *currentBlkPtr;

    MemoryPool(int memSize, int blockSize);

    void allocateBlock();

    void addRecord(int recordSize);

    void allocateRecord();

    [[nodiscard]] double getTotalMemPoolSize() const;

    [[nodiscard]] double getCurrentMemPoolSize() const;

    [[nodiscard]] int getNumUsedRecords() const;

    [[nodiscard]] int getNumUsedBlocks() const;

    [[nodiscard]] int getRecordSize() const;

    [[nodiscard]] int numRecordsInBlock() const;

    ~MemoryPool();

};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORY_POOL_H
