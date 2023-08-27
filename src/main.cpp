#include "storage/memory_pool.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int BLOCK_SIZE = 400; // 400B
uint MEMORY_POOL_SIZE = 100000000; // 100MB
string DATA_DIR = "../data/games.txt";

int main() {

    cout << "================================================" << endl;
    cout << "  SC3020 Database System Principles Project 1  " << endl;
    cout << "================================================\n" << endl;

    MemoryPool disk(MEMORY_POOL_SIZE, BLOCK_SIZE);
    cout << "Allocating " << disk.getMemPoolSize() << "MB for memory pool\n" << endl;
    int num_of_records = 0;

    // Reading data from games.txt into memory pool
    ifstream datafile(DATA_DIR);

    if (datafile.is_open()) {
        cout << "Reading data from text files...\n" << endl;
        string line;

        while (getline(datafile, line)) {
            stringstream linestream(line);
            Record record;
            string tempDate;

            linestream << tempDate << record.team_id_home << record.pts_home
                       << record.fg_pct_home << record.ft_pct_home << record.fg3_pct_home
                       << record.ast_home << record.reb_home << record.home_team_wins;

            char delimiter = '/';

            istringstream dateStream(tempDate);
            dateStream >> record.game_day.to >> delimiter >> record.game_month >> delimiter >> record.game_year;

            cout << record.game_day << endl;
            num_of_records += 1;
        }

        cout << "========  Data reading complete  ========\n" << endl;
        datafile.close();
    } else {
        cout << "Data file could not be found at '" << DATA_DIR << "'" << endl;
    }

    // Experiment 1 Results
    cout << "==================================================================" << endl;
    cout << "Experiment 1: Reading data text file into DB system." << endl;
    cout << " - Number of records: " << num_of_records << endl;
    cout << " - Size of a record: " << 0 << endl;
    cout << " - Number of records stored in a block: " << 0 << endl;
    cout << " - Number of blocks for storing the data: " << 0 << endl;
    cout << "==================================================================" << endl;


    return 0;
}
