#include "time.h"
#include "console_prompt/console_prompt.h"
#include <iostream>
#include <sstream>

// combines two different tms
// if two values for a given value are positive, will default to a
std::tm operator|(const std::tm &a, const std::tm &b) {
	std::tm result;

	int * r_ptr = (int*)(&result);
	int * a_ptr = (int*)(&a);
	int * b_ptr = (int*)(&b);

	for (int i = 0; i < 9; i++)
		r_ptr[i] = (a_ptr[i] != 0) ? a_ptr[i] : b_ptr[i];

	return result;
}

// see if two tms are identical
bool operator==(const std::tm &a, const std::tm &b) {
	int * a_ptr = (int*)(&a);
	int * b_ptr = (int*)(&b);

	for (int i = 0; i < 9; i++)
		if (a_ptr[i] != b_ptr[i])
			return false;

	return true;
}



// converts 24 hour date and time to time_t
std::time_t mktime(const int month, const int day, const int year, const int hour, const int minute) {
	std::tm tp = TIME_ZERO;
	tp.tm_mday = day;
	tp.tm_mon = month - 1;
	tp.tm_year = year - 1900;
	tp.tm_hour = hour;
	tp.tm_min = minute;
	return mktime(&tp);
}

// convert a tm into a duration. only uses hours, minutes, and seconds
std::time_t mkduration(const std::tm &t) {
	return (t.tm_hour * 60 + t.tm_min) * 60 + t.tm_sec;
}

// string in M/D/Y format
std::string tmToMDY(const std::tm &t) {
	return std::to_string(t.tm_mon + 1) + '/' + std::to_string(t.tm_mday) + '/' + std::to_string(t.tm_year + 1900);
}
// string in HH:MM am/pm format
std::string tmToHM(const std::tm &t) {
	char temp[20] = { 0 };
	std::strftime(temp, 19, "%I:%M %p", &t);
	return temp;
}

// M/D/Y format of time_t based on local time
std::string timeToLocalMDY(const std::time_t &t) {
	std::tm tm;
	localtime_s(&tm, &t);
	std::string str = tmToMDY(tm);
	return str;
}

// H:M format of time_t based on local time
std::string timeToLocalHM(const std::time_t &t) {
	std::tm tm;
	localtime_s(&tm, &t);
	std::string str = tmToHM(tm);
	return str;
}


// extract time from string, returns TIME_ZERO if could not valid time
std::tm parseTime(std::string input) {
	bool success = false;
	std::tm tm = TIME_ZERO;

	std::size_t colonPos = input.find_first_of(':');

	if (colonPos != std::string::npos) {
		std::stringstream(input) >> tm.tm_hour; // hours
		input.erase(0, colonPos + 1); // remove everything before :
		std::stringstream(input) >> tm.tm_min; // minutes

		bool hoursValid = (0 <= tm.tm_hour && tm.tm_hour < 24);

		if (input.find_first_of("pP") != std::string::npos)  // if a p is found, time must be in 12 hour format
			if (tm.tm_hour > 12 || tm.tm_hour == 0)
				hoursValid = false;
			else
				tm.tm_hour += 12; // convert to 24 hour if valid

		success = hoursValid && (0 <= tm.tm_min && tm.tm_min < 60);
	}
	return (success) ? tm : TIME_ZERO;
}


// true if given year is a leap year
inline bool isLeap(const int year) {
	return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

// true if given year is valid
bool isValidDate(const int d, const int m, const int y) {
	if (m < 0 || m > 11 || y < 0 || d < 0 || d > 31)
		return false;

	if (m == 2) // handles leap years
		return isLeap(y) ? d <= 29 : d <= 28;

	if (m == 4 || m == 6 || m == 9 || m == 11) // April, June, Sept and Nov must have number of days <= 30. 
		return d <= 30;

	return true;
}

// takes a date in MM/DD/YYYY format
std::tm parseDate(std::string input) {
	bool success = false;
	std::tm tm = TIME_ZERO;

	std::size_t firstDashPos = input.find_first_of("\\/");
	if (firstDashPos != std::string::npos) {
		std::stringstream(input) >> tm.tm_mon; // month
		tm.tm_mon -= 1;

		input.erase(0, firstDashPos + 1); // remove everything before first dash
		std::stringstream(input) >> tm.tm_mday; // day

		std::size_t secondDashPos = input.find_first_of("\\/");

		if (secondDashPos != std::string::npos) {
			input.erase(0, secondDashPos + 1); // remove everything before second dash

			std::stringstream(input) >> tm.tm_year; // year
			tm.tm_year -= 1900;

			success = isValidDate(tm.tm_mday, tm.tm_mon, tm.tm_year);
		}
	}

	return (success) ? tm : TIME_ZERO;
}

// tm from user parsed by given parser
std::tm tmFromUser(const char *msg, std::tm(*parser)(std::string)) {
	std::tm tm;
	do {
		std::string temp = prompt<std::string>(msg);
		tm = parser(temp);
	} while (tm == TIME_ZERO);
	return tm;
}