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

    // Missing counter
    int missing_data = 0;

    bool header = true;
    if (datafile.is_open())
    {
        cout << "Reading data from text files...\n"
             << endl;
        string line;

        while (getline(datafile, line))
        {

            // Skip header of the txt file
            if (header)
            {
                header = false;
                continue;
            }

            // Write new record into structure
            Record new_record = {};
            new_record.store(line);

            // Skip blank data
            if(new_record.fg_pct_home == 0.0){
                missing_data++;
                continue;
            }

            // Save new record into memory pool
            auto new_record_address = disk.saveRecord(new_record);

            // Add to B+ Tree sequentially
            tree.insertKey(&disk, new_record.fg_pct_home, new_record_address);
        }

        // STORE THE BPLUSTREE ONCE READING IS DONE
        disk.saveBPlusTree(tree);

        cout << "--------------  DATA READING COMPLETE ----------------" << endl;
        cout << "Summary of Memory Pool: " << endl;
        cout << disk.getCurrentMemory() << "MB / "
             << disk.getTotalMemory() << "MB"
             << endl
             << "Missing data: "
             << missing_data
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
    cout << "=============================================================================================================================================================" << endl;
    cout << "Experiment 1: Reading data text file into DB system." << endl;
    cout << " - Number of records: " << disk.getNumUsedRecords() << endl;
    cout << " - Size of a record: " << disk.getRecordSize() << endl;
    cout << " - Number of records stored in a block: " << disk.getNumRecordsInBlock() << endl;
    cout << " - Number of blocks for storing the data: " << disk.getNumUsedDataBlocks() << endl;
    cout << endl;

    // Experiment 2 Results
    disk_tree->displayExp2Results();

    // Experiment 3 Results
    cout << "\nExperiment 3: Retrieve those movies with the atttribute FG_PCT_home equal to 0.5" << endl;
    tree.displayExp3Results(&disk);

    // Experiment 4 Results
    cout << "\nExperiment 4: retrieve those movies with the attribute FG_PCT_home from 0.6 to 1" << endl;
    tree.displayExp4Results(&disk);

    // Experiment 5 Results
    cout << "\nExperiment 5: delete those movies with the attribute “FG_PCT_home” below 0.35 inclusively" << endl;
    disk_tree->displayExp5Results(&disk);

    cout << "==================================================================================================================================================================" << endl;


    cout << "\n\n\n=============================================================================================================================================================" << endl;
    cout << "=================================                            END OF ASSIGNMENT 1                                            =================================" << endl;
    cout << "=============================================================================================================================================================" << endl;

    return 0;
}
