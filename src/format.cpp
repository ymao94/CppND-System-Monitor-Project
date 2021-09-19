#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

string padding(int time) {
    string padded = std::to_string(time);
    if (time < 10) 
        padded = "0" + padded;
    return padded;
}

string Format::ElapsedTime(long seconds) { 
    int HH = seconds / 3600;
    int remain_minutes = seconds % 3600; // the number of remaining less-than-one-hour seconds
    int MM = remain_minutes / 60;
    int SS = remain_minutes % 60;

    string standard_expr = padding(HH) + ":" + padding(MM) + ":" + padding(SS);
    return standard_expr; 
}