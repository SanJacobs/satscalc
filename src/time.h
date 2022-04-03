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

void wind(moment &input_moment, int minutes, int hours, int days);

int days_in(int month, int year);

std::string timeprint(moment input_moment);

