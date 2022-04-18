#include "time.h"

// Look. listen here. There's no way I'm going to start taking DST into account.
// I have to draw the line somewhere, and frankly, once you start doing "Change an hour on the 4th moon of the 2nd week of March in France, but only if the tulips haven't sprung... etc... etc.." I'm out.
// The fact that DST is designed this way, though, luckily makes it so you are unlikely to be on the clock during a DST transition.


//
// --- OPERATOR OVERLOADS ---
//

long sortable_time(const moment input_moment) {
		return stol(std::to_string(input_moment.year)+
		padint(input_moment.month,2)+
		padint(input_moment.day,2)+
		padint(input_moment.hours,2)+
		padint(input_moment.minutes,2));
}

bool moment::operator<(const moment& other) const{
	return bool(sortable_time(*this) < sortable_time(other));
}

bool moment::operator>(const moment& other) const{
	return bool(sortable_time(*this) > sortable_time(other));
}

bool moment::operator==(const moment& other) const {
	return bool(year==other.year &&
			month==other.month &&
			day==other.day &&
			hours==other.hours &&
			minutes==other.minutes);
}

bool moment::operator!=(const moment& other) const {
	return bool(year!=other.year ||
			month!=other.month ||
			day!=other.day ||
			hours!=other.hours ||
			minutes!=other.minutes);
}

delta moment::operator-(const moment& other) const {
	// Uses what I call an accumulator-decumulator design
	// Count how long it takes to approach a benchmark,
	// and that's the difference
	// 
	if(*this==other) return{0,0,0};
	delta accumulator{0,0,0};
	
	// smallest operand becomes benchmark to approach
	const bool reverse{*this<other};
	const moment& benchmark = reverse? *this : other;
	moment decumulator = reverse? other : *this;
	
	// It is possible to write something that does this in months at a time, instead of days,
	// which would be faster, but I am not expecting to have to do this with such
	// long periods of time, so screw that.
	while(decumulator.year - benchmark.year > 1 ||
			decumulator.month - benchmark.month > 1 ||
			decumulator.day - benchmark.day > 1) {
		wind(decumulator, 0, 0, -1);
		accumulator.days++;
	}
	while(decumulator.hours - benchmark.hours > 1) {
		wind(decumulator, 0, -1, 0);
		accumulator.hours++;
	}
	while(decumulator != benchmark) {
		wind(decumulator, -1, 0, 0);
		accumulator.minutes = accumulator.minutes+1;
	}
	while(accumulator.minutes > 59) {
		accumulator.minutes -= 60;
		accumulator.hours++;
	}
	return accumulator;
}

std::ostream& operator<<(std::ostream& stream, const delta& other) {
	if(other.days) stream << other.days << " days, ";
	if(other.hours) stream << other.hours << " hours, ";
	if(other.minutes) stream << other.minutes << " minutes.";
	return stream;
}


//
// --- METHODS ---
//

double timeblock::hourcount() {
	delta timedelta = end-start;
	return (timedelta.minutes/60.0f +
			timedelta.hours +
			timedelta.days*24);
}



//
// --- FUNCTIONS ---
//

std::string padint(const int input, const int minimum_signs) {
	std::ostringstream output;
	output << std::internal << std::setfill('0') << std::setw(minimum_signs) << input;
	return output.str();
}

timeblock timesplit(timeblock& input_block, const moment splitpoint) {
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

void wind(moment& input_moment, const int minutes, const int hours, const int days) {
	
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

// TODO: Add checks for correct formatting, and ask for new input if wrong
moment timeinput(int or_year, int or_month, int or_day) {
	char input_string[5];
	std::cout << "Input time\nHHMM (24-hour format, no space)\n";
	std::cin >> input_string;
	moment output{std::stoi(std::string(std::string(1, input_string[2])+input_string[3])),
			std::stoi(std::string(std::string(1, input_string[0])+input_string[1])),
			or_day, or_month, or_year};
			// This is retarded
	return output;
}
moment timeinput() {
	char input_string[17];
	std::cout << "Input date and time\nYEAR MM DD hh mm (24-hour format, use spaces)\n";
	std::cin.getline(input_string, 17);
	
	// This uglyness is just how you use strtok() to split a string, apparently
	const char* p;
	int split_input[5];
	int i{0};
	p = strtok(input_string, " ");
	while (p != NULL) {
		split_input[i] = int(atoi(p));
		i++;
		p = strtok(NULL, " ");
	}
	
	moment output{split_input[4],
		split_input[3],
		split_input[2],
		split_input[1],
		split_input[0]};
	return output;
}
