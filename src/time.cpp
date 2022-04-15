#include "time.h"

// Look. listen here. There's no way I'm going to start taking DST into account.
// I have to draw the line somewhere, and frankly, once you start doing "Change an hour on the 4th moon of the 2nd week of March in France, but only if the tulips haven't sprung... etc... etc.." I'm out.

// Operator overloads:
bool moment::operator<(const moment& other) const{
	// Converts moments to strings and casts them as ints to compare
	using namespace std;
	string left_moment_string{
			to_string(year)+
			padint(month,2)+
			padint(day,2)+
			padint(hours,2)+
			padint(minutes,2)};
	string right_moment_string{
			to_string(other.year)+
			padint(other.month,2)+
			padint(other.day,2)+
			padint(other.hours,2)+
			padint(other.minutes,2)};
	long left_int = stol(left_moment_string);
	long right_int = stol(right_moment_string);
	return bool(left_int < right_int);
}
bool moment::operator>(const moment& other) const{
	using namespace std;
	string left_moment_string{
			to_string(year)+
			padint(month,2)+
			padint(day,2)+
			padint(hours,2)+
			padint(minutes,2)};
	string right_moment_string{
			to_string(other.year)+
			padint(other.month,2)+
			padint(other.day,2)+
			padint(other.hours,2)+
			padint(other.minutes,2)};
	long left_int = stol(left_moment_string);
	long right_int = stol(right_moment_string);
	return bool(left_int > right_int);
}
bool moment::operator=(const moment& other) const {
	return bool(year==other.year &&
			month==other.month &&
			day==other.day &&
			hours==other.hours &&
			minutes==other.minutes);
}

std::string padint(const int input, const int minimum_signs) {
	std::ostringstream output;
	output << std::internal << std::setfill('0') << std::setw(minimum_signs) << input;
	return output.str();
}

timeblock timesplit(timeblock &input_block, const moment splitpoint) {
	// Splits a timeblock at splitpoint.
	// It changes the input_block to end at splitpoint, and returns a new timeblock
	// that lasts from splitpoint to where the input_block used to end.
	if(splitpoint < input_block.start || splitpoint > input_block.end) {
		std::cerr << "ERROR: Splitpoint outside of timeblock!\n";
		std::cerr << "Timeblock: " << timeprint(input_block) << std::endl;
		std::cerr << "Splitpoint: " << timeprint(splitpoint) << std::endl;
	}
	timeblock output{splitpoint, input_block.end};
	input_block.end = splitpoint;
	return output;
}

void wind(moment &input_moment, const int minutes, const int hours, const int days) {
	
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
	// XXX: This will cause trouble if you wind time by more than a month's length in days.
	// So, let's just not do that... heh...
	input_moment.day += days;
	int current_month_length = days_in(input_moment.month, input_moment.year);
	while(input_moment.day > current_month_length) {
		input_moment.day -= current_month_length;
		input_moment.month++;
		if(input_moment.month > 12) {
			input_moment.month -= 12;
			input_moment.year++;
		}
		current_month_length = days_in(input_moment.month, input_moment.year);
	}
	while(input_moment.day < 1) {
		input_moment.month--;
		if(input_moment.month < 1) {
			input_moment.month += 12;
			input_moment.year--;
		}
		current_month_length = days_in(input_moment.month, input_moment.year);
		input_moment.day += current_month_length;
	}
}

std::string timeprint(moment input_moment) {
	using namespace std;
	string output =
		padint(input_moment.hours, 2) + ":"
		+ padint(input_moment.minutes, 2) + " "
		+ to_string(input_moment.year) + "-"
		+ padint(input_moment.month, 2) + "-"
		+ padint(input_moment.day, 2);
	return output;
}

std::string timeprint(timeblock input_timeblock) {
	std::string output{timeprint(input_timeblock.start) + " --> " + timeprint(input_timeblock.end)};
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
