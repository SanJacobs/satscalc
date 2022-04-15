#pragma once
#include <iostream>

struct moment{
	signed int minutes;
	signed int hours; // 24-hour format. 23:59 progresses to 00:00
	signed int day;
	signed int month;
	signed int year;
};

struct timeblock{
	moment start;
	moment end;
};

class day{
	
};

timeblock timesplit(timeblock &input_block, moment splitpoint);
	// Splits a timeblock at splitpoint.
	// It changes the input_block to end at splitpoint, and returns a new timeblock
	// that lasts from splitpoint to where the input_block used to end.

void wind(moment &input_moment, int minutes, int hours, int days);

int days_in(int month, int year);

std::string timeprint(moment input_moment);

