//
// Created by Eugene Wee on 27/8/23.
//

#include "MemoryPool.h"
#include <vector>

using namespace std;

vector <void *> block_ptr_list ; 

/*
 *  Create a fixed-size memory pool with the given total memory size
 *  and block size. Initialize every bit to 0.
 */
MemoryPool::MemoryPool(int totalMemorySize, int blockSize) {

    this->total_memory_size = totalMemorySize;
    this->block_size = blockSize;

    this->mem_pool_ptr = new char[total_memory_size];
    memset(mem_pool_ptr, 0, total_memory_size);

    this->current_data_block = nullptr;
    this->bplustree_ptr = nullptr;
}

MemoryPool::~MemoryPool() = default;

/*
 *  Allocate new block within the memory pool if sufficient remaining memory space in the pool.
 *  Returns the address of the new block if created
 */
Block *MemoryPool::allocateBlock() {
    bool sufficientTotalMemory = current_memory_size + block_size <= total_memory_size;

    if (sufficientTotalMemory) {
        /*
         * Allocate memory space for the new block and return the designated memory address
         * We calculate the allocated memory address using the (number of used blocks * block size).
         */
        auto allocated_memory_add = (char *) mem_pool_ptr + num_used_blocks * block_size + sizeof(Block);
        
        num_used_blocks += 1;

        auto new_block = new Block(allocated_memory_add);

        // new_block->blockID = num_used_blocks ; // Kelly

        // Update block
        current_memory_size += block_size;
        // num_used_blocks += 1;

        
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
void MemoryPool::allocateRecord() {

    // Create block if current data block is null
    if (current_data_block == nullptr) {
        Block *new_block = allocateBlock();
        current_data_block = new_block;
        num_used_data_blocks++;

        if (new_block != nullptr) {
            cout << "First data record stored at " << new_block->block_ptr << endl;
        // Kelly
        //push new block ptr to list of block pointers
        block_ptr_list.push_back(new_block->block_ptr);

        }
        return;
    }

    // Checks if sufficient memory in current block
    bool sufficientMemory = current_data_block->size + RECORD_SIZE <= block_size;

    if (!num_used_blocks || !sufficientMemory) {

        // Allocates new block and replace current
        Block *new_block = allocateBlock();
        
        block_ptr_list.push_back(new_block->block_ptr);
        
        current_data_block = new_block;
        num_used_data_blocks++;

        if (!new_block) {
            string error = "Insufficient Memory " +
                           to_string(getCurrentMemory()) +
                           "MB / " +
                           to_string(getTotalMemory()) +
                           "MB";
            throw logic_error(error);
        }
    }
}


/*
 *  Allocate memory block for new BPLusTree node, and move contents into allocated space in memory.
 */
void MemoryPool::saveBPlusTreeNode(BPlusTreeNode *newNode) {
    Block *new_block = allocateBlock();

    if (new_block != nullptr) {
        // Move the content of the BPlusTree node into the designated memory block
        new_block->addNode(&newNode);

        // Update block attributes
        // new_block->size += sizeof(BPlusTreeNode);
        // new_block->num_records++;
    }
}

void MemoryPool::deleteBPlusTreeNode(BPlusTreeNode *node) {

        Block *dBlock = (Block*) node;
        // Move the content of the BPlusTree node into the designated memory block
        dBlock->deleteNode(&dBlock);

        // Update block attributes
        // dBlock->size-=sizeof(BPlusTreeNode);
        // dBlock->num_records--;
        // Update block
        current_memory_size -= block_size;
        num_used_blocks -= 1;
}

void MemoryPool::saveBPlusTree(BPlusTree tree) {
    Block *new_block = allocateBlock();
    if (new_block != nullptr) {
        new_block->addTree(&tree);

        // Save the root of the BPlusTree
        if (bplustree_ptr == nullptr) {
            bplustree_ptr = new_block;
            cout << "BPlusTree stored at " << new_block->block_ptr << endl << endl;
        }

        // Update block attributes
        new_block->size += sizeof(BPlusTree);
        new_block->num_records++;
    }
}


/*
 *  Save record by allocating memory space
 */
void *MemoryPool::saveRecord(Record newRecord) {
    allocateRecord();
    newRecord.block_id = num_used_data_blocks;
    auto record_address = current_data_block->addRecord(&newRecord);

    num_used_records += 1;
    num_accessed_blocks += 1;

    return record_address;
}

void *MemoryPool::deletemRecord(Record* dRecord) {

    Block* dBlock = dBlock->findBlock(dRecord);
    cout << "==================================================================" << endl;
    dBlock->deleteRecord(dRecord);

    if(dBlock->size==0){
        delete dBlock;
        num_used_data_blocks--;

    }

    num_used_records -= 1;

    return dRecord;
}


/*
 *  Load record given its record address
 */
Record *MemoryPool::loadRecord(void *recordAddress) {
    return (Record *) recordAddress;
}

/*
 *  Displays the record given its record address
 */
void MemoryPool::displayRecord(void *recordAddress) {
    auto *record = new Record;

    memcpy(record, recordAddress, RECORD_SIZE);

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

float MemoryPool::loadRecordfcg3(void *recordAddress)
{
    auto *record = new Record;
    memcpy(record, recordAddress, RECORD_SIZE);

    return record->fg3_pct_home;

}

double MemoryPool::getTotalMemory() const {
    return round(total_memory_size / (1024 * 1024));
}

double MemoryPool::getCurrentMemory() const {
    return (double) current_memory_size / (1024 * 1024);
}

int MemoryPool::getNumUsedDataBlocks() const {
    return num_used_data_blocks;
}

int MemoryPool::getNumUsedRecords() const {
    return num_used_records;
}

int MemoryPool::getNumAccessedBlocks() const {
    return num_accessed_blocks;
}

int MemoryPool::getRecordSize() const {
    return RECORD_SIZE;
}

int MemoryPool::getNumRecordsInBlock() const {
    int utilised_block_size = (int) (block_size - sizeof(Block) / RECORD_SIZE);
    return utilised_block_size;
}

int MemoryPool::getBlockID(void *recordAddress)
{
    auto *record = new Record;
    memcpy(record, recordAddress, RECORD_SIZE);

    return record->block_id ;
}

/*
void MemoryPool::getBlocksAccessedByForce(float lower, float upper)
{
    cout << "size of block ptr list  " << block_ptr_list.size() << endl;
    int numRecord = 0 ;
    int datablks = 0;

    for (int i = 0; i < block_ptr_list.size(); i++)
    {
       // cout << "Block ptr " << block_ptr_list[i] << endl;
       int usedSize = 0 ;
       int numRecord = 0 ;

       while (usedSize <= block_size)
       {
       void* recordAddress = (void*)((char*) block_ptr_list[i] + numRecord * RECORD_SIZE);

       auto *record = new Record;
       record = loadRecord(recordAddress) ;
      
       if (record == nullptr) break ;
       else
       {
       if (record->fg_pct_home == 0.5) displayRecord(recordAddress) ;

       numRecord = numRecord + 1;
       usedSize = usedSize + RECORD_SIZE ;
       }
       }
       datablks++;

    }

    return datablks; ;
} 
*/


int MemoryPool::getBlocksAccessedByBruteForce(float lowerkey, float upperkey)
{
    int datablks = 0;

 
    for (int i = 0; i < block_ptr_list.size(); i++)
    {
        int num_records = 0;
        int used_size =0;
        
        while (used_size < block_size)
        {
            auto record = new Record();
            void* record_ptr = (void*)((char*) block_ptr_list[i] + num_records * RECORD_SIZE);

           // cout << " i num records: " << i << "  " << num_records << "  " << record_ptr << endl;

            if (record != nullptr)
            {
                record = loadRecord(record_ptr);
            }

            else
            {
                break;
            }

            float value = record->fg_pct_home;

            if (value >= lowerkey && value <= upperkey)
            {

                displayRecord(record);
                BFSearchAddresslist.push_back(record_ptr);

            }

            used_size = used_size + RECORD_SIZE;
            num_records++;
        } 

        datablks++;
    
    }
 //  cout << "block_ptr_list.size: " <<  block_ptr_list.size() << endl;
  cout << "BFSearchAddresslist: " <<  BFSearchAddresslist.size() << endl;
    return datablks;
}





