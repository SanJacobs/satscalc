#pragma once
#include <iostream>
#include <sstream>
#include <ios>
#include <iomanip>
#include <string.h>
#include <vector>

struct delta{
	signed int minutes;
	signed int hours;
	signed int days;
};

struct moment{
	signed int minutes;
	signed int hours; // 24-hour format. 23:59 progresses to 00:00
	signed int day;
	signed int month;
	signed int year;
	
	bool operator<(const moment& other) const;
	bool operator>(const moment& other) const;
	bool operator==(const moment& other) const;
	bool operator!=(const moment& other) const;
	delta operator-(const moment& other) const;
};

struct timeblock{
	moment start;
	moment end;
	double hourcount();
};

struct workday{
	moment call;
	moment wrap;
	moment planned_wrap;
	timeblock blocks[12];
	//
	// 1.  sleepbreach
	// 2.  Call
	// 3.  Early morning
	// 4.  start of day
	// 5.  1st hr overtime
	// 6.  post-1 hour overtime
	// 7.  14-hour mark
	// 8.  22:00
	// 9.  midnight crossing
	// 10. 06:00
	// 11. wrap
	//
	workday(moment calltime, moment wraptime, moment planned_wraptime) {
		
	}
	workday(moment previous_wrap, moment calltime, moment wraptime, moment planned_wraptime) {
		
	}
};

std::string padint(const int input, const int minimum_signs);

timeblock timesplit(timeblock& input_block, const moment splitpoint);
	// Splits a timeblock at splitpoint.
	// It changes the input_block to end at splitpoint, and returns a new timeblock
	// that lasts from splitpoint to where the input_block used to end.

void wind(moment &input_moment, const int minutes, const int hours, const int days);

int days_in(const int month, const int year);

std::string timeprint(const moment input_moment);
std::string timeprint(const timeblock input_timeblock);
long sortable_time(const timeblock input_timeblock);

moment timeinput(const int or_year, const int or_month, const int or_day);
moment timeinput();

