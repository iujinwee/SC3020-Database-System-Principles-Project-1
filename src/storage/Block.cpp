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

Block* Block::findBlock(void *recordAddress){
    Record* dRecord=(Record*)recordAddress;
    Block* dBlock = (Block*) (dRecord->getBlockAddress(dRecord));
    return dBlock;
}

void* Block::addRecord(void *recordAddress){
    void* dest_ptr = (void*)((char*) block_ptr + num_records * RECORD_SIZE);
    memmove(dest_ptr, recordAddress, RECORD_SIZE);

    this->size += RECORD_SIZE;
    this->num_records++;
    

    return dest_ptr;
};

void* Block::deleteRecord(void *recordAddress){
    this->size -= RECORD_SIZE;
    this->num_records--;
    
    memset(recordAddress, 0, RECORD_SIZE);

    return recordAddress;
};

void* Block::addNode(void *nodeAddress){
    auto node_size = (int)sizeof(BPlusTreeNode);
    void* dest_ptr = (void*)((char*) block_ptr);
    memmove(dest_ptr, nodeAddress, node_size);

    this->size += node_size;
    // this->num_records++;

    return dest_ptr;
}

void* Block::deleteNode(void *nodeAddress){
    auto node_size = (int)sizeof(BPlusTreeNode);
    memset(nodeAddress, 0, node_size);

    this->size -= node_size;
    // this->num_records--;

    return nodeAddress;
}

void* Block::addTree(void *treeAddress){
    auto node_size = (int)sizeof(BPlusTree);
    void* dest_ptr = (void*)((char*) block_ptr);
    memmove(dest_ptr, treeAddress, node_size);

    this->size += node_size;
    // this->num_records++;

    return dest_ptr;
}

