#pragma once

// A wrapper for ctime

// Written by ItsNorin
// https://github.com/ItsNorin


#include <ctime>
#include <string>

constexpr std::tm TIME_ZERO = { 0,0,0, 1,0,0, 0,0,-1 };

// combines two different tms
// if two values for a given value are positive, will default to a
std::tm operator|(const std::tm &a, const std::tm &b);

// see if two tms are identical
bool operator==(const std::tm &a, const std::tm &b);
bool operator!=(const std::tm &a, const std::tm &b) { return !(a == b); }

// converts 24 hour date and time to time_t
std::time_t mktime(const int month, const int day, const int year, const int hour, const int minute);
// convert a tm into a duration. only uses hours, minutes, and seconds
std::time_t mkduration(const std::tm &t);


// string in M/D/Y format
std::string tmToMDY(const std::tm &t);
// string in HH:MM am/pm format
std::string tmToHM(const std::tm &t);

// M/D/Y format of time_t based on local time
std::string timeToLocalMDY(const std::time_t &t);
// H:M format of time_t based on local time
std::string timeToLocalHM(const std::time_t &t);



// extract time from string, returns TIME_ZERO if could not valid time
std::tm parseTime(std::string input);

// takes a date in MM/DD/YYYY format
std::tm parseDate(std::string input);


// tm from user parsed by given parser
std::tm tmFromUser(const char *msg, std::tm(*parser)(std::string));
