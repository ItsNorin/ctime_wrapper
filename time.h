#pragma once
#include <ctime>
#include <string>

// A wrapper for CTime to make it less annoying to use
// Written by ItsNorin
// https://github.com/ItsNorin

constexpr const std::tm TIME_ZERO = { 0,0,0, 1,0,0, 0,0,-1 };

namespace Format {
	struct Format {
		const char * format_str;
		const int max_size;
		Format(const char *format_str, const int max_size) : format_str(format_str), max_size(max_size) {}
	};

	const Format HH_MM_12("%I:%M", 6);
	const Format HH_MM_AMPM("%I:%M %p", 12);
	const Format HH_MM_24("%R", 6);
	const Format MM_DD_YYYY("%D", 12);

	const std::string hh_mm_apm = "%H:%M %P";
};

// combines two different tms
// if two values for a given value are positive, will default to a
std::tm operator|(const std::tm &a, const std::tm &b);

// see if two tms are identical
bool operator==(const std::tm &a, const std::tm &b);
inline bool operator!=(const std::tm &a, const std::tm &b) { return !(a == b); }

// convert a tm into a duration. only uses hours, minutes, and seconds
std::time_t mkduration(const std::tm &t);

// convert seconds to a tm for displaying
std::tm durationToTm(const std::time_t &t);

// tm formatter
std::string timeToStr(const std::tm &tm, std::string format);

// tm formatted with strftime
std::string strftime(const std::tm &t, const Format::Format &format);

// formatted string of time_t based on local time with strftime
std::string strftimeTimeLocal(const std::time_t &t, const Format::Format &format);

// extract time from string, returns TIME_ZERO if could not valid time
std::tm parseTime(std::string input);

// takes a date in MM/DD/YYYY format
std::tm parseDate(std::string input);

// tm from user parsed by given parser
std::tm tmFromUser(const char *msg, std::tm(*parser)(std::string));
