//
// Created by Eugene Wee on 1/10/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_BLOCK_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_BLOCK_H


class Block {
public:
    int num_records = 0;
    int size = sizeof(Block);

    //int blockID ;  // kelly
    

    void* block_ptr;

    explicit Block(void* allocatedAddress);

    Block* findBlock(void *recordAddress);

    void* addRecord(void *recordAddress);

    void* addNode(void *nodeAddress);

    void* deleteRecord(void *recordAddress);

    void* deleteNode(void *nodeAddress);

    void* addTree(void *treeAddress);

    

};

#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_BLOCK_H
