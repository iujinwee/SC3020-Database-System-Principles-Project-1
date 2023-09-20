//
// Created by Eugene Wee on 29/8/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_RECORD_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_RECORD_H

#include <string>
#include <cstring>
#include <iostream>
#include <stdint.h>

using namespace std;

struct RecordAddress {
    unsigned char *address;
    std::size_t offset;
};

struct Record {
    uint32_t bin_game_date;  // Binary representation of Game Date DD|MM|YYYY
    uint32_t team_id_home;   // Team ID, 32 bits
    uint16_t pts_home;       // Points
    float fg_pct_home;       // Field Goal Percentage
    float ft_pct_home;       // Free Throw Percentage
    float fg3_pct_home;      // Three-Point Percentage
    uint16_t ast_home;        // Assists Home
    uint16_t reb_home;        // Rebounds Home
    uint16_t home_team_wins;  // Home Team Wins (1 bit for boolean)

private:
    void packDate(const string &date);

public:
    void store(const string &data);

    [[nodiscard]] string getDate() const;
};


#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_RECORD_H
