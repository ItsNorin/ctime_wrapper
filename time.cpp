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

// convert a tm into a duration. only uses hours, minutes, and seconds
std::time_t mkduration(const std::tm &t) {
	return (t.tm_hour * 60 + t.tm_min) * 60 + t.tm_sec;
}

// convert seconds to a tm for displaying
std::tm durationToTm(const std::time_t &t) {
	std::tm tm = TIME_ZERO;
	tm.tm_min = t / 60;
	tm.tm_sec = t % 60;
	tm.tm_hour = tm.tm_min / 60;
	tm.tm_min %= 60;
	return tm;
}

// ensures an int takes up at least 2 spaces
inline std::string to_string_with0(const int i) {
	return (i < 10) ? ('0' + std::to_string(i)) : (std::to_string(i));
}

inline std::string fullMonthName(const int month) {
	static const std::string names[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
	return (0 <= month && month < 12) ? names[month] : "Invalid Month";
}

inline std::string shortMonthName(const int month) {
	static const std::string names[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	return (0 <= month && month < 12) ? names[month] : "Inv";
}

// tm of time_t based on local time
std::tm timeToLocalTm(const std::time_t &t) {
	std::tm tm;
	localtime_s(&tm, &t);
	return tm;
}

std::string timeToStr(const std::tm &tm, std::string format) {
	for (std::size_t i = 0; i < format.size(); i++) {
		if (format[i] == '%') {
			char cmd = format[i + 1]; // character after %
			std::string toIns;

			switch (cmd) {
				case 'Y': // year as number
				case 'y': // last 2 digits of year
					toIns = std::to_string(tm.tm_year + 1900);
					if (cmd == 'y')
						toIns = toIns.substr(toIns.size() - 3, 2);
					break;

				case 'O': // month as decimal with preceeding 0, ex: 03
					toIns = to_string_with0(tm.tm_mon);
					break;

				case 'o': // month as decimal
					toIns = std::to_string(tm.tm_mon);
					break;

				case 'B': // full month name
					toIns = fullMonthName(tm.tm_mon);
					break;

				case 'b': // abbreviated month 
					toIns = shortMonthName(tm.tm_mon);
					break;

				case 'D': // day of the month with preceeding 0, ex: 03
					toIns = to_string_with0(tm.tm_mday);
					break;

				case 'd': // day of the month
					toIns = std::to_string(tm.tm_mday);
					break;

				case 'H': // hour using 24 hour clock
					toIns = std::to_string(tm.tm_hour);
					break;

				case 'h': // hour using 12 hour clock
					toIns = std::to_string(tm.tm_hour - ((tm.tm_hour > 12) ? 12 : 0));
					break;

				case 'M': // minute with preceeding 0
					toIns = to_string_with0(tm.tm_min);
					break;

				case 'm': // minutes without preceeding 0
					toIns = std::to_string(tm.tm_min);
					break;

				case 'S': // seconds with preceeding 0
					toIns = to_string_with0(tm.tm_sec);
					break;

				case 's': // seconds without preceeding 0
					toIns = std::to_string(tm.tm_sec);
					break;

				case 'p':  // whether it is AM or PM, lowercase
					toIns = (tm.tm_hour > 12) ? "pm" : "am";
					break;

				case 'P': // whether it is AM or PM
					toIns = (tm.tm_hour > 12) ? "PM" : "AM";
					break;

				case 'j': // day of the year
					toIns = std::to_string(tm.tm_yday);
					break;

				case '%':
					toIns = '%';
					break;

				default: break;
			}
			format.insert(i + 2, toIns); // insert after command
			format.erase(i, 2); // remove original command

			if (toIns.size() > 0) // skip any inserted characters
				i += toIns.size() - 1;
		}
	}

	return format;
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
		std::string temp = prompt(msg);
		tm = parser(temp);
	} while (tm == TIME_ZERO);
	return tm;
}