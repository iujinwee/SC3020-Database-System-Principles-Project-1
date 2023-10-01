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
    cout << "BLOCK " << allocatedAddress << endl;
}

/*
 *  Stores the Record contents in the allocated memory address given by the recordAddress.
 *  Returns the memory address of the stored record.
 */
void* Block::addRecord(void *recordAddress){
    // ERROR HELP ME!
    void* dest_ptr = block_ptr + num_records * RECORD_SIZE;
    cout << num_records << " " <<  block_ptr << endl;
    memmove(dest_ptr, recordAddress, RECORD_SIZE);

    this->size += RECORD_SIZE;
    this->num_records++;

    return dest_ptr;
};
