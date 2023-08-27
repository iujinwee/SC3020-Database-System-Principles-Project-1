//
// Created by Eugene Wee on 27/8/23.
//

#include "memory_pool.h"

MemoryPool::MemoryPool(uint memPoolSize, uint32_t blockSize) {
    memPoolPtr = nullptr;

    this->memPoolSize = memPoolSize;
    this->blockSize = blockSize;

    this->memPoolPtr = new Record[memPoolSize];

}

MemoryPool::~MemoryPool() {
    delete memPoolPtr;
    memPoolPtr = nullptr;
}

double MemoryPool::getMemPoolSize() {
    return round(memPoolSize / 1e6);
}
