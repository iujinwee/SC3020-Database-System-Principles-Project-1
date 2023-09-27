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

    /*
     * Assign blocks to memory pool and initialize every bit to 0.
     */
    this->mem_pool_ptr = allocateBPlusTreeNode();
    this->current_data_block = nullptr;
}

MemoryPool::~MemoryPool() = default;

/*
 *  Allocate new block within the memory pool if sufficient remaining memory space in the pool.
 *  Returns the address of the new block if created
 */
Block *MemoryPool::allocateBlock() {
    bool sufficientTotalMemory = current_memory_size + block_size <= total_memory_size;

    if (sufficientTotalMemory) {
        auto new_block = new Block();

        // Update block
        current_memory_size += block_size;
        num_used_blocks += 1;

        return new_block;
    }

    cout << "Insufficient memory space in Memory Pool to allocate new block." << endl;
    return nullptr;
}

/*
 *  Allocates memory space for the record.
 *  Creates new block if insufficient space in current block.
 *  Throws error if required size exceed memory of block size.
 */
void *MemoryPool::allocateRecord(Block *cur) {

    // Create block if null
    if (cur == nullptr) {
        Block *new_block = allocateBlock();
        current_data_block = new_block;
        return (void *) current_data_block;
    }

    // Checks if sufficient memory in current block
    bool sufficientMemory = cur->size + record_size <= block_size;

    if (!num_used_blocks || !sufficientMemory) {

        // Allocates new block and replace current
        Block *new_block = allocateBlock();
        current_data_block = new_block;

        if (!new_block) {
            string error = "Insufficient Memory " +
                           to_string(getCurrentMemory()) +
                           "MB / " +
                           to_string(getTotalMemory()) +
                           "MB";
            throw logic_error(error);
        }
    }

    // Allocate memory space within memory pool
    void *new_address = current_data_block + record_size;

    return new_address;
}

/*
 *  Allocates memory space for new BPlusTreeNode
 */
void *MemoryPool::allocateBPlusTreeNode() {
    auto new_address = (void *) ((int *) mem_pool_ptr + num_used_blocks + block_size);
    return new_address;
}

/*
 *  Stores the new BPlusTreeNode into memory pool
 */
void MemoryPool::saveBPlusTreeNode(BPlusTreeNode *newNode) {
    auto new_address = allocateBPlusTreeNode();
    memmove(new_address, (void *) newNode, block_size);
}

/*
 *  Save record by allocating memory space then updating relevant counters.
 *  We used memmove to transfer the content to the allocated record address.
 */
void *MemoryPool::saveRecord(Record newRecord) {
    auto new_record_address = allocateRecord(current_data_block);
    memmove(new_record_address, (void *) &newRecord, record_size);

    current_data_block->size += record_size;
    current_data_block->num_records += 1;
    num_used_records += 1;
    num_accessed_blocks += 1;

    return new_record_address;
}

/*
 *  Load record given its record address
 */
Record *MemoryPool::loadRecord(void *recordAddress) {
    return (Record *) recordAddress;
}

void MemoryPool::displayRecord(void *recordAddress) {
    auto *record = new Record;

    memcpy(record, recordAddress, record_size);

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

int MemoryPool::getNumUsedBlocks() const {
    return num_used_blocks;
}

int MemoryPool::getNumUsedRecords() const {
    return num_used_records;
}

int MemoryPool::getNumAccessedBlocks() const {
    return num_accessed_blocks;
}

int MemoryPool::getRecordSize() const {
    return record_size;
}

int MemoryPool::getNumRecordsInBlock() const {
    int utilised_block_size = block_size - (block_size % record_size);
    return utilised_block_size / record_size;
}

