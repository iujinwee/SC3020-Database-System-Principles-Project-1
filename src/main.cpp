#include "storage/memory_pool.h"
#include "storage/Record.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int BLOCK_SIZE = 400; // 400B
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
    vector <RecordAddress> recordAddressList;

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
            Record newRecord = {};
            newRecord.store(line);
            recordAddressList.push_back(disk.saveRecord(newRecord));
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

    cout << recordAddressList.size() << endl;
    for (RecordAddress t: recordAddressList) {
        MemoryPool::displayRecord(t);
//        cout << &t.address << endl;
    }


    return 0;
}
