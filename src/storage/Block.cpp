//
// Created by Eugene Wee on 1/10/23.
//

#include "Block.h"
#include "MemoryPool.h"

/*
 *  Stores the Block contents at the allocated memory address.
 */
Block::Block(void* allocatedAddress){
    block_ptr = allocatedAddress;
}

/*
 *  Stores the Record contents in the allocated memory address given by the recordAddress.
 *  Returns the memory address of the stored record.
 */
void* Block::addRecord(void *recordAddress){
    void* dest_ptr = (void*)((char*) block_ptr + num_records * RECORD_SIZE);
    memmove(dest_ptr, recordAddress, RECORD_SIZE);

    this->size += RECORD_SIZE;
    this->num_records++;

    return dest_ptr;
};

void* Block::addNode(void *nodeAddress){
    auto node_size = (int)sizeof(BPlusTreeNode);
    void* dest_ptr = (void*)((char*) block_ptr);
    memmove(dest_ptr, nodeAddress, node_size);

    this->size += node_size;
    this->num_records++;

    return dest_ptr;
}