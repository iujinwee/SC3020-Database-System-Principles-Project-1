#include "storage/MemoryPool.h"
#include "storage/Record.h"
#include "indexing/BPlusTree.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int MEMORY_POOL_SIZE = 100 * 1024 * 1024; // 100MB
string DATA_DIR = "../data/games.txt";

int main()
{

    cout << "================================================" << endl;
    cout << "  SC3020 Database System Principles Project 1  " << endl;
    cout << "================================================\n"
         << endl;

    // Allocating memory space in MM for memory pool
    MemoryPool disk(MEMORY_POOL_SIZE, BLOCK_SIZE);
    cout << "Allocating " << disk.getTotalMemory() << "MB for memory pool\n"
         << endl;

    // Reading data from games.txt into memory pool
    ifstream datafile(DATA_DIR);

    // Initialize B+ Tree
    BPlusTree tree;
    int count = 0;
    int data_limit = 20;

    bool header = true;
    if (datafile.is_open())
    {
        cout << "Reading data from text files...\n"
             << endl;
        string line;

        while (getline(datafile, line) && data_limit > 0) /*DATA LIMIT FOR DELETION TESTING REMOVE LATER*/
        {

            // Skip header of the txt file
            if (header)
            {
                header = false;
                continue;
            }

            // Write new record into memory pool
            Record new_record = {};
            new_record.store(line);
            auto new_record_address = disk.saveRecord(new_record);

            // Add to B+ Tree sequentially
            tree.insertKey(&disk, new_record.fg_pct_home, new_record_address);
            //            cout << count++ << endl;
            tree.displayTree();
            data_limit--;
        }

        // STORE THE BPLUSTREE ONCE READING IS DONE
        disk.saveBPlusTree(tree);

        cout << "--------------  DATA READING COMPLETE ----------------" << endl;
        cout << "Summary of Memory Pool: " << endl;
        cout << disk.getCurrentMemory() << "MB / "
             << disk.getTotalMemory() << "MB"
             << endl
             << endl;

        datafile.close();
    }
    else
    {
        cout << "Data file could not be found at '" << DATA_DIR << "'" << endl;
    }

    // BEGIN EXPERIMENTS

    // We will simulate reading from the disk, by loading the tree from the memory pool.
    auto disk_tree = (BPlusTree *)(((Block *)disk.bplustree_ptr)->block_ptr);

    // Experiment 1 Results
    cout << "==================================================================" << endl;
    cout << "Experiment 1: Reading data text file into DB system." << endl;
    cout << " - Number of records: " << disk.getNumUsedRecords() << endl;
    cout << " - Size of a record: " << disk.getRecordSize() << endl;
    cout << " - Number of records stored in a block: " << disk.getNumRecordsInBlock() << endl;
    cout << " - Number of blocks for storing the data: " << disk.getNumUsedDataBlocks() << endl;
    cout << endl;

    // Experiment 2 Results
    disk_tree->displayExp2Results();

    // Experiment 3 Results
    //    disk_tree->searchNode(0.5);
    //    disk_tree->displayExp3Results();
    //

    // TESTING EXPT 3 HELPER FUNCTIONS
    // 1. Test ShiftKeysToBack
    //  BPlusTreeNode *node = static_cast<BPlusTreeNode *>(disk_tree->root->children[1]);
    //  node->ShiftKeysToBack(node, 1);

    // 2. BorrowFromRight
    //  BPlusTreeNode *leftNode = static_cast<BPlusTreeNode *>(disk_tree->root->children[1]);
    //  BPlusTreeNode *rightNode = static_cast<BPlusTreeNode *>(disk_tree->root->children[2]);
    //  disk_tree->BorrowFromRight(1, leftNode, rightNode);
    //  disk_tree->displayTree();

    // 3. findLB_rightSubTree
    BPlusTreeNode *node = static_cast<BPlusTreeNode *>(disk_tree->root->children[0]);
    BPlusTreeKey key = disk_tree->findLB_rightSubTree(node, 0);
    cout << " LB of right subtree of given index : " << key.key << endl;

    // Experiment 4 Results
    //    disk_tree->searchRange(0.6, 1);
    //    disk_tree->displayExp4Results();

    // Experiment 5 Results
    //    disk_tree->deleteKey(0.35);
    //    disk_tree->displayExp5Results();

    cout << "==================================================================" << endl;

    // Experiment 3 (KELLY VERSION)

    //    auto *record = new Record{};
    //    int flag ;
    //    int counter = 0 ;
    //
    //    int size = sizeof(Record);
    //
    //
    //    for (RecordAddress t: record_address_list)
    //    {
    //        memcpy(record, (char *) (t.address + t.offset), size);
    //
    //        if (record->fg_pct_home == 0.5) count = count + 1 ;
    //
    //    }
    //
    //    cout << " - Number of records with fg_pct_home = 0.5: " << count << endl;

    ////   DOCUMENTATION HERE !!
    //    // For demo on how to retrieve content of data records)
    //    auto data = (Block*) disk.current_data_block;
    //    cout << "Number of records in current data block: " << data->num_records << endl;
    //
    //    for (int i = 0; i < data->num_records; i++){
    //        auto r = (Record*) ((char*) data->block_ptr + i * RECORD_SIZE);
    //        cout << r->fg_pct_home << endl;
    //    }

    ////  FIXED BPLUS TREE WHEEE
    //    auto bptree_ptr = (Block*) disk.bplustree_ptr;
    //    auto bptree = (BPlusTree*) bptree_ptr->block_ptr;
    //    cout << bptree->nodes << endl;
    //    auto bptree_root = bptree->root;
    //    cout << bptree_root->size << endl;
    //    for(int i = 0; i < bptree_root->size; i ++){
    //        cout << bptree_root->keys[i].key << endl;
    //    }

    return 0;
}
