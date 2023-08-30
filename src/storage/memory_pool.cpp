//
// Created by Eugene Wee on 27/8/23.
//

#include "memory_pool.h"
#include "iostream"

using namespace std;

MemoryPool::MemoryPool(int memSize, int blockSize) {
    memPoolPtr = (int *) this;
    currentBlkPtr = (int *) this;

    this->totalMemPoolSize = memSize;
    this->blockSize = blockSize;
    this->recordSize = 0;
    this->currentMemPoolSize = 0;
    this->currentBlockSize = 0;
    this->memPoolUsedRecords = 0;
    this->memPoolUsedBlocks = 0;
    this->memPoolAvailableBlocks = totalMemPoolSize / blockSize;
}

MemoryPool::~MemoryPool() {
//    delete memPoolPtr;
//    delete currentBlkPtr;
    currentBlkPtr = nullptr;
    memPoolPtr = nullptr;
}

void MemoryPool::addRecord(int size) {
    recordSize = size;
    if (!memPoolUsedBlocks) memPoolUsedBlocks += 1;

    // Check if sufficient memory in current block to add record
    bool sufficientMemory = currentBlockSize + recordSize <= blockSize;
    if (!sufficientMemory) {
        // Check if there are available blocks to allocate new memory
        if (!memPoolAvailableBlocks) {
            cout << "Insufficient Memory! "
                 << currentMemPoolSize << "MB / "
                 << totalMemPoolSize << "MB"
                 << endl;
            return;
        }
        allocateBlock();
    }
    allocateRecord();
}

void MemoryPool::allocateRecord() {
    currentBlkPtr += recordSize;
    currentBlockSize += recordSize;
    memPoolUsedRecords += 1;
}

/* Allocate block when there is sufficient memory in the memory pool.
 * This is done by adding an offset to the memory pointer, which is computed as the
 * # of used blocks * single block size
 */
void MemoryPool::allocateBlock() {
    int offset = memPoolUsedBlocks * blockSize;
    currentBlkPtr = memPoolPtr + offset; // Create a new block at the end of memory space
    currentMemPoolSize += blockSize;
    memPoolUsedBlocks += 1;
    memPoolAvailableBlocks -= 1;
    currentBlockSize = 0;
}

double MemoryPool::getTotalMemPoolSize() const {
    return round(totalMemPoolSize / 1e6);
}

double MemoryPool::getCurrentMemPoolSize() const {
    return currentMemPoolSize / 1e6;
}

int MemoryPool::getNumUsedRecords() const {
    return memPoolUsedRecords;
}

int MemoryPool::getNumUsedBlocks() const {
    return memPoolUsedBlocks;
}

int MemoryPool::getRecordSize() const {
    return recordSize;
}

int MemoryPool::numRecordsInBlock() const {
    int utilisedBlockSize = blockSize - (blockSize % recordSize);
    return utilisedBlockSize / recordSize;
}

