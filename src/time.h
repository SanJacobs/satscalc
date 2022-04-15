#pragma once
#include <iostream>
#include <sstream>
#include <ios>
#include <iomanip>

struct moment{
	signed int minutes;
	signed int hours; // 24-hour format. 23:59 progresses to 00:00
	signed int day;
	signed int month;
	signed int year;
	
	bool operator<(const moment& other) const;
	bool operator>(const moment& other) const;
	bool operator=(const moment& other) const;
};

struct timeblock{
	moment start;
	moment end;
};

std::string padint(const int input, const int minimum_signs);

timeblock timesplit(timeblock &input_block, const moment splitpoint);
	// Splits a timeblock at splitpoint.
	// It changes the input_block to end at splitpoint, and returns a new timeblock
	// that lasts from splitpoint to where the input_block used to end.

void wind(moment &input_moment, const int minutes, const int hours, const int days);

int days_in(int month, int year);

std::string timeprint(moment input_moment);

std::string timeprint(timeblock input_timeblock);

