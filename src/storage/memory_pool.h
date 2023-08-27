//
// Created by Eugene Wee on 27/8/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORY_POOL_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORY_POOL_H

#include "types.h"
#include "iostream"

class MemoryPool {
private:
    Record *memPoolPtr;

    uint memPoolSize;
    uint8_t blockSize;
    uint8_t memPoolUsedBlocks;
    uint8_t memPoolUsedRecords;
    uint8_t currentBlock;

public:
    MemoryPool(uint32_t memPoolSize, uint32_t blockSize);

    ~MemoryPool();

    double getMemPoolSize();
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_MEMORY_POOL_H
