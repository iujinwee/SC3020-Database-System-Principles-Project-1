//
// Created by Eugene Wee on 27/8/23.
//

#include "memory_pool.h"

using namespace std;

MemoryPool::MemoryPool(int totalMemorySize, int blockSize) {
    /*
     * Create a fixed-size memory pool with the given total memory size
     * and block size.
     */
    this->totalMemorySize = totalMemorySize;
    this->blockSize = blockSize;

    /*
     * Assign blocks to memory pool and initialize every bit to 0.
     */
    this->memPoolPtr = new u_char[totalMemorySize];
    memset((void *) memPoolPtr, 0, totalMemorySize);

    this->nodeBlkPtr = nullptr;
    this->dataBlkPtr = nullptr;
}

MemoryPool::~MemoryPool() = default;

/*
 * Allocate new block within the memory pool if sufficient remaining memory space in the pool.
 */
bool MemoryPool::allocateBlock() {
    bool sufficientTotalMemory = currentMemorySize + blockSize <= totalMemorySize;
    if (sufficientTotalMemory) {
        currentMemorySize += blockSize;
        dataBlkPtr = memPoolPtr + numUsedBlocks * blockSize;
        numUsedBlocks += 1;
        currentBlockSize = 0;
        return true;
    } else {
        cout << "Insufficient memory space in Memory Pool to allocate new block." << endl;
    }
    return false;
}

/*
 *  Allocates memory space for the record.
 *  Creates new block if insufficient space in current block.
 *  Throws error if required size exceed memory of block size.
 */
RecordAddress MemoryPool::allocate(std::size_t requiredSize) {
    if (requiredSize > blockSize) {
        throw invalid_argument("Insufficient space in memory block!");
    }

    bool sufficientMemory = currentBlockSize + requiredSize <= blockSize;
    if (!numUsedBlocks || !sufficientMemory) {
        bool allocatedNewBlock = allocateBlock();
        if (!allocatedNewBlock) {
            string error = "Insufficient Memory " +
                           to_string(getCurrentMemory()) +
                           "MB / " +
                           to_string(getTotalMemory()) +
                           "MB";
            throw logic_error(error);
        }
    }

    // Updates the current block size
    std::size_t offset = currentBlockSize;
    currentMemorySize += requiredSize;

    return RecordAddress{dataBlkPtr, offset};
}

/*
 *  Save record by allocating memory space then updating relevant counters.
 *  We used memcpy to transfer the content to the allocated record address.
 */
RecordAddress MemoryPool::saveRecord(Record newRecord) {
    RecordAddress newRecordAdd = allocate(sizeof(newRecord));
    memmove((void *) (newRecordAdd.address + newRecordAdd.offset), (void *) &newRecord, sizeof(newRecord));

    currentBlockSize += sizeof(newRecord);
    numUsedRecords += 1;
    numAccessedBlocks += 1;

    return newRecordAdd;
}

/*
 *  Load record given its record address
 */
Record *MemoryPool::loadRecord(RecordAddress record_address) {
    void *record = new Record{};
    int size = sizeof(RecordAddress);

    memcpy(record, (char *) (record_address.address + record_address.offset), size);

    return (Record *) record;
}

void MemoryPool::displayRecord(RecordAddress record_address) {
    auto *record = new Record;
    int size = sizeof(Record);

    memcpy(record, (char *) (record_address.address + record_address.offset), size);

    string delimiter = " ";

    cout << record->getDate() << delimiter
         << record->team_id_home << delimiter
         << record->pts_home << delimiter
         << record->fg_pct_home << delimiter
         << record->ft_pct_home << delimiter
         << record->fg3_pct_home << delimiter
         << record->ast_home << delimiter
         << record->reb_home << delimiter
         << record->home_team_wins << delimiter
         << endl;
}


double MemoryPool::getTotalMemory() const {
    return round(totalMemorySize / (1024 * 1024));
}

double MemoryPool::getCurrentMemory() const {
    return (double) currentMemorySize / (1024 * 1024);
}

std::size_t MemoryPool::getNumUsedBlocks() const {
    return numUsedBlocks;
}

std::size_t MemoryPool::getNumUsedRecords() const {
    return numUsedRecords;
}

std::size_t MemoryPool::getRecordSize() const {
    return recordSize;
}

std::size_t MemoryPool::getNumRecordsInBlock() const {
    std::size_t utilisedBlockSize = blockSize - (blockSize % recordSize);
    return utilisedBlockSize / recordSize;
}

