//
// Created by Eugene Wee on 29/8/23.
//

#include "Record.h"
#include <sstream>

using namespace std;

/*
 *  Writing string data into Record class by parsing linestream and packing
 *  date into uint32_t.
 */
void Record::store(const string &data) {
    istringstream linestream(data);
    string tempDate;

    linestream >> tempDate >> team_id_home >> pts_home
               >> fg_pct_home >> ft_pct_home >> fg3_pct_home
               >> ast_home >> reb_home >> home_team_wins;

    packDate(tempDate);
}

/*
 * Bitwise operation to store date from uint32_t (bit-masking)
 * i.e. 0x1F: 00011111 (we apply this to get the lowest 5 bits of the bin_game_date)
 *
 * DD - 5 bits (0 - 31)         [-----------DDDDD]
 *                              [0000000000011111]
 * MM - 4 bits (0 - 12)         [-------MMMM-----]
 * YYYY - 14 bits (0001 - 9999) [YYYYYYY---------]
 */
void Record::packDate(const std::string &date) {

    stringstream datestream(date);
    string token;
    uint8_t day, month;
    uint16_t year;

    getline(datestream, token, '/');
    day = (uint8_t) stoi(token);

    getline(datestream, token, '/');
    month = (uint8_t) stoi(token);

    getline(datestream, token, '/');
    year = (uint16_t) stoi(token);

    bin_game_date = (day & 0x1F) | ((month & 0x0F) << 5) | ((year & 0x3FFF) << 9);
}

/* Bitwise operation to retrieve date from uint32_t. We apply bit-masking
 * on the variable to retrieve only the relevant values.
 * i.e. 0x1F: 00011111 (we apply this to get the lowest 5 bits of the bin_game_date)
 *
 * DD - 5 bits
 * MM - 4 bits
 * YYYY - 15 bits
 */
string Record::getDate() const {
    return to_string(bin_game_date & 0x1F) + "/" +
           to_string((int) (bin_game_date >> 5 & 0x0F)) + "/" +
           to_string((int) (bin_game_date >> 9 & 0x3FFF));
}

void* Record::getBlockAddress(void *recordAddress)
{
    return this->block_add;
}