#include "time.h"

// Look. listen here. There's no way I'm going to start taking DST into account.
// I have to draw the line somewhere, and frankly, once you start doing "Change an hour on the 4th moon of the 2nd week of March in France, but only if the tulips haven't sprung... etc... etc.." I'm out.
// Just do it on a set day, God dammit.

timeblock timesplit(timeblock &input_block, moment splitpoint) {
	timeblock output{splitpoint, input_block.end};
	input_block.end = splitpoint;
	return output;
}

void wind(moment &input_moment, int minutes, int hours, int days) {
	
	// Adding minutes
	input_moment.minutes += minutes;
	while(input_moment.minutes > 59) {
		input_moment.minutes -= 60;
		input_moment.hours++;
	}
	while(input_moment.minutes < 0) {
		input_moment.minutes += 60;
		input_moment.hours--;
	}
	
	// Adding hours
	input_moment.hours += hours;
	while(input_moment.hours > 23) {
		input_moment.hours -= 24;
		input_moment.day++;
	}
	while(input_moment.hours < 0) {
		input_moment.hours += 24;
		input_moment.day--;
	}
	
	// Adding days
	input_moment.day += days;
	// FIXME: Months don't roll over.'The -3th of January will cause problems.
	//											  ^^^^^^^
	// Fixing this will just take some simple modulo math.
	int current_month_length = days_in(input_moment.month, input_moment.year);
	while(input_moment.day > current_month_length) {
		input_moment.day -= current_month_length;
		input_moment.month++;
		current_month_length = days_in(input_moment.month, input_moment.year);
	while(input_moment.day < 1) {
		input_moment.day += days_in(((input_moment.month-1)%12)+1); // FIXME, this is absolutely retarded at the moment
		input_moment.month++;
		current_month_length = days_in(input_moment.month, input_moment.year);
	}
}

std::string timeprint(moment input_moment) {
	using namespace std;
	// TODO: Add leading zeroes to hours and minutes
	string output =
		to_string(input_moment.hours) + ":"
		+ to_string(input_moment.minutes) + " "
		+ to_string(input_moment.year) + "-"
		+ to_string(input_moment.month) + "-"
		+ to_string(input_moment.day);
	return output;
}

int days_in(int month, int year) {
	// Kind of a stupid and slow way to do this
	// But it's nice to have it as a function
	// because of the leap year arithmatic
	switch (month) {
		case 1:
			return 31;
		case 2:
			if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)){
				return 29;
			}
			return 28;
		case 3:
			return 31;
		case 4:
			return 30;
		case 5:
			return 31;
		case 6:
			return 30;
		case 7:
			return 31;
		case 8:
			return 31;
		case 9:
			return 30;
		case 10:
			return 31;
		case 11:
			return 30;
		case 12:
			return 31;
	}
	std::cout << "Something just went very wrong. You found month #" << std::to_string(month) << '\n';
	return 5;
}
