//
// Created by Eugene Wee on 1/10/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_BLOCK_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_BLOCK_H


class Block {
public:
    int num_records = 0;
    int size = sizeof(Block);

    void* block_ptr;

    explicit Block(void* allocatedAddress);

    void* addRecord(void *recordAddress);
};

#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_BLOCK_H
