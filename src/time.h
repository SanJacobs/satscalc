#pragma once
#include <iostream>
#include <sstream>
#include <ios>
#include <iomanip>
#include <string.h>
#include <vector>
#include <algorithm>

enum weekday{
	monday,
	tuesday,
	wednesday,
	thursday,
	friday,
	saturday,
	sunday
};

struct delta{
	unsigned int minutes;
	unsigned int hours;
	unsigned int days;
};
std::ostream& operator<<(std::ostream& stream, const delta& other);

struct moment{
	signed int minutes;
	signed int hours; // 24-hour format. 23:59 progresses to 00:00
	signed int day;
	signed int month;
	signed int year;
	
	weekday getweekday();
	
	bool operator<(const moment& other) const;
	bool operator>(const moment& other) const;
	bool operator<=(const moment& other) const;
	bool operator>=(const moment& other) const;
	bool operator==(const moment& other) const;
	bool operator!=(const moment& other) const;
	delta operator-(const moment& other) const;
	moment operator+(const delta& other) const;
	moment operator-(const delta& other) const;
};

struct timeblock{
	moment start;
	moment end;
	double hourcount();
	float valuefactor = 1;
	float upvalue(float suggestion);
};

struct workday{
	moment call;
	moment wrap;
	moment planned_wrap;
	timeblock blocks[15];
	int total_timeblocks;
	// total_timeblocks exsists because blocks can't be shrunk,
	// so total_timeblocks is the point at which blocks
	// just contains garbage data.
	//
	// 1.  call
	// 2.  sleepbreach
	// 3.  6:00
	// 4.  started 2 hours before 7
	// 5.  1st hr overtime
	// 6.  post-1 hour overtime
	// 7.  end of warned ot
	// 8.  14-hour mark
	// 9.  22:00
	// 10. midnight crossing
	// 11. 06:00
	// 12. wrap
	//
	workday(const moment& previous_wrap,
				const moment& calltime,
				const moment& wraptime,
				const moment& planned_wraptime);
	void lunch(const moment& lunch_start, const moment& lunch_end);
};

std::string padint(const int input, const int minimum_signs);

timeblock timesplit(timeblock& input_block, const moment splitpoint);
	// Splits a timeblock at splitpoint.
	// It changes the input_block to end at splitpoint, and returns a new timeblock
	// that lasts from splitpoint to where the input_block used to end.

void wind(moment& input_moment, const int minutes, const int hours, const int days);
void wind(moment& input_moment, const delta& time_delta);

int days_in(const int month, const int year);

std::string timeprint(const moment input_moment);
std::string timeprint(const timeblock input_timeblock);
long sortable_time(const timeblock input_timeblock);

moment timeinput(moment input_moment);
moment timeinput();
// TODO: Completely re-write timeinput to be beautiful and enforce valid dates

