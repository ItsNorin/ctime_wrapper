#pragma once
#include <ctime>
#include <string>

// A wrapper for CTime to make it less annoying to use
// Written by ItsNorin
// https://github.com/ItsNorin

constexpr const std::tm TIME_ZERO = { 0,0,0, 1,0,0, 0,0,-1 };

namespace Format {
	const std::string hh_mm_apm = "%h:%M %P";
	const std::string hh_mm_24 = "%H:%M";
	const std::string m_d_yyyy = "%o/%d/%Y";
	const std::string m_d_yy = "%o/%d/%y";
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

// tm of time_t based on local time
std::tm timeToLocalTm(const std::time_t &t);

// tm formatter
std::string timeToStr(const std::tm &tm, std::string format);

inline std::string timeToStr(const std::time_t t, std::string format) { return timeToStr(timeToLocalTm(t), format); }

// extract time from string, returns TIME_ZERO if could not valid time
std::tm parseTime(std::string input);

// takes a date in MM/DD/YYYY format
std::tm parseDate(std::string input);

// tm from user parsed by given parser
std::tm tmFromUser(const char *msg, std::tm(*parser)(std::string));