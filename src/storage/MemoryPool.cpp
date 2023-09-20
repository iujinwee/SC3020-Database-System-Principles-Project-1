//
// Created by Eugene Wee on 27/8/23.
//

#include "MemoryPool.h"

using namespace std;

MemoryPool::MemoryPool(int totalMemorySize, int blockSize) {
    /*
     * Create a fixed-size memory pool with the given total memory size
     * and block size.
     */
    this->total_memory_size = totalMemorySize;
    this->block_size = blockSize;

    /*
     * Assign blocks to memory pool and initialize every bit to 0.
     */
    this->mem_pool_ptr = new u_char[totalMemorySize];
    memset((void *) mem_pool_ptr, 0, totalMemorySize);

    this->b_tree_ptr = nullptr;
    this->data_ptr = nullptr;
}

MemoryPool::~MemoryPool() = default;

/*
 * Allocate new block within the memory pool if sufficient remaining memory space in the pool.
 */
bool MemoryPool::allocateBlock() {
    bool sufficientTotalMemory = current_memory_size + block_size <= total_memory_size;
    if (sufficientTotalMemory) {
        current_memory_size += block_size;
        data_ptr = mem_pool_ptr + num_used_blocks * block_size;
        num_used_blocks += 1;
        current_block_size = 0;
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
    if (requiredSize > block_size) {
        throw invalid_argument("Insufficient space in memory block!");
    }

    bool sufficientMemory = current_block_size + requiredSize <= block_size;
    if (!num_used_blocks || !sufficientMemory) {
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
    std::size_t offset = current_block_size;
    current_memory_size += requiredSize;

    return RecordAddress{data_ptr, offset};
}

/*
 *  Save record by allocating memory space then updating relevant counters.
 *  We used memcpy to transfer the content to the allocated record address.
 */
RecordAddress MemoryPool::saveRecord(Record newRecord) {
    RecordAddress newRecordAdd = allocate(sizeof(newRecord));
    memmove((void *) (newRecordAdd.address + newRecordAdd.offset), (void *) &newRecord, sizeof(newRecord));

    current_block_size += sizeof(newRecord);
    num_used_records += 1;
    num_accessed_blocks += 1;

    return newRecordAdd;
}

/*
 *  Load record given its record address
 */
Record *MemoryPool::loadRecord(RecordAddress recordAddress) {
    void *record = new Record{};
    int size = sizeof(RecordAddress);

    memcpy(record, (char *) (recordAddress.address + recordAddress.offset), size);

    return (Record *) record;
}

void MemoryPool::displayRecord(RecordAddress recordAddress) {
    auto *record = new Record;
    int size = sizeof(Record);

    memcpy(record, (char *) (recordAddress.address + recordAddress.offset), size);

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
    return round(total_memory_size / (1024 * 1024));
}

double MemoryPool::getCurrentMemory() const {
    return (double) current_memory_size / (1024 * 1024);
}

std::size_t MemoryPool::getNumUsedBlocks() const {
    return num_used_blocks;
}

std::size_t MemoryPool::getNumUsedRecords() const {
    return num_used_records;
}

std::size_t MemoryPool::getRecordSize() const {
    return record_size;
}

std::size_t MemoryPool::getNumRecordsInBlock() const {
    std::size_t utilisedBlockSize = block_size - (block_size % record_size);
    return utilisedBlockSize / record_size;
}

