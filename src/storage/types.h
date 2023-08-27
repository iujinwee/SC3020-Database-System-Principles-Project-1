//
// Created by Eugene Wee on 27/8/23.
//

#ifndef SC3020_DATABASE_SYSTEM_PRINCIPLES_TYPES_H
#define SC3020_DATABASE_SYSTEM_PRINCIPLES_TYPES_H

#include <cstdint> // Include this header for fixed-size data types

struct Record {
    uint8_t game_day;      // Day of the month (0-31), 5 bits
    uint16_t game_month;    // Month (1-12), 4 bits
    uint16_t game_year;    // Year (0001 - 9999), 15 bits
    uint32_t team_id_home;  // Team ID, 32 bits
    uint8_t pts_home;      // Points
    double fg_pct_home;   // Field Goal Percentage
    float ft_pct_home;   // Free Throw Percentage
    float fg3_pct_home;  // Three-Point Percentage
    uint8_t ast_home;      // Assists Home
    uint8_t reb_home;      // Rebounds Home
    bool home_team_wins;    // Home Team Wins (1 byte for boolean)
};

#endif //SC3020_DATABASE_SYSTEM_PRINCIPLES_TYPES_H
