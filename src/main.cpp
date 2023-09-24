#include "storage/MemoryPool.h"
#include "storage/Record.h"
#include "indexing/BPlusTree.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int MEMORY_POOL_SIZE = 100 * 1024 * 1024; // 100MB
string DATA_DIR = "../data/games.txt";

int main() {

    cout << "================================================" << endl;
    cout << "  SC3020 Database System Principles Project 1  " << endl;
    cout << "================================================\n" << endl;

    // Allocating memory space in MM for memory pool
    MemoryPool disk(MEMORY_POOL_SIZE, BLOCK_SIZE);
    cout << "Allocating " << disk.getTotalMemory() << "MB for memory pool\n" << endl;

    // Reading data from games.txt into memory pool
    ifstream datafile(DATA_DIR);
    vector<RecordAddress> record_address_list;

    // Initialize B+ Tree
    BPlusTree tree;

    bool header = true;
    if (datafile.is_open()) {
        cout << "Reading data from text files...\n" << endl;
        string line;

        while (getline(datafile, line)) {

            // Skip header of the txt file
            if (header) {
                header = false;
                continue;
            }

            // Write new record into memory pool
            Record new_record = {};
            new_record.store(line);
            RecordAddress new_record_address= disk.saveRecord(new_record);
            record_address_list.push_back(new_record_address);

            // Add to B+ Tree sequentially
            tree.insertKey(new_record.fg_pct_home, &new_record);
            tree.displayTree();
        }

        cout << "--------------  DATA READING COMPLETE ----------------" << endl;
        cout << "Summary of Memory Pool: " << endl;
        cout << disk.getCurrentMemory() << "MB / "
             << disk.getTotalMemory() << "MB"
             << endl;

        datafile.close();

    } else {
        cout << "Data file could not be found at '" << DATA_DIR << "'" << endl;
    }

    // Experiment 1 Results
    cout << "==================================================================" << endl;
    cout << "Experiment 1: Reading data text file into DB system." << endl;
    cout << " - Number of records: " << disk.getNumUsedRecords() << endl;
    cout << " - Size of a record: " << disk.getRecordSize() << endl;
    cout << " - Number of records stored in a block: " << disk.getNumRecordsInBlock() << endl;
    cout << " - Number of blocks for storing the data: " << disk.getNumUsedBlocks() << endl;
    cout << "==================================================================" << endl;


//    cout << record_address_list.size() << endl;
//    for (RecordAddress t: record_address_list) {
//        MemoryPool::displayRecord(t);
//    }


    return 0;
}
