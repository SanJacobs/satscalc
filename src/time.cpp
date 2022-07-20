#include "time.h"

// Look. listen here. There's no way I'm going to start taking DST into account.
// DST transition times are decided using skull dice, by the grand wizards of the state.
// The fact that DST is designed this way, though,
// makes it so you are unlikely to be on the clock during a DST transition.


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

bool moment::operator<=(const moment& other) const {
	return bool((*this < other) || *this == other);
}

bool moment::operator>=(const moment& other) const {
	return bool((*this > other) || *this == other);
}

bool moment::operator!=(const moment& other) const {
	return bool(year!=other.year ||
			month!=other.month ||
			day!=other.day ||
			hours!=other.hours ||
			minutes!=other.minutes);
}
moment moment::operator+(const delta& other) const {
	moment output{*this};
	wind(output, other.minutes, other.hours, other.days);
	return output;
}
moment moment::operator-(const delta& other) const {
	moment output{*this};
	wind(output, other.minutes*-1, other.hours*-1, other.days*-1);
	return output;
}

delta moment::operator-(const moment& other) const {
	// Uses what I call an accumulator-decumulator design
	// Count how long it takes to approach a benchmark,
	// and that count is the difference
	
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
	while(accumulator.hours > 23) {
		accumulator.hours -= 24;
		accumulator.days++;
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
	if(other.days==0 && other.hours==0 && other.minutes==0){
		stream << "None.";
		return stream;
	}
	if(other.days) stream << other.days << " days, ";
	if(other.hours) stream << other.hours << " hours, ";
	if(other.minutes) stream << other.minutes << " minutes.";
	return stream;
}


//
// --- CONSTRUCTORS ---
//

workday::workday(const moment& previous_wrap,
					const moment& calltime,
					const moment& wraptime,
					const moment& planned_wraptime) {
	call = calltime;
	wrap = wraptime;
	planned_wrap = planned_wraptime;
	timeblock initial_block{call, wrap};
	moment splitpoints[10]{ // --$-- Points where the price may change --$-- //
		
		// NOTE: Maybe this should also contain the valuefactor associated with the split.
		// Probably the valuefactor leading up to the splitpoint, not the one after.
		// Maybe this should be a struct?
		// Or maybe I should just implement this badly at first just to get it working, and replace it later?
		
		previous_wrap+(delta){0, 10, 0}, // Sleepbreach, 10 hours after previous wrap			0x
		(moment){0, 5, call.day, call.month, call.year}, // 2 hours before 7, aka 5				1
		(moment){0, 6, call.day, call.month, call.year}, // 6 in the morning					2
		call+(delta){0, 8, 0}, // Normal 8 hours of work										3x
		call+(delta){0, 9, 0}, // 1st hour of overtime is over									4
		planned_wraptime, // End of warned overtime												5
		call+(delta){0, 14, 0}, // The 14-hour mark												6x
		(moment){0, 22, call.day, call.month, call.year}, // 22:00 in the evening				7x
		(moment){0, 23, call.day, call.month, call.year}+(delta){0, 1, 0}, // Midnight			8x
		(moment){0, 23, call.day, call.month, call.year}+(delta){0, 7, 0}, // 6, next morning	9x
	};
	
	// Eliminate planned wrap, if it occurs within normal 8-hour period.
	// This is to make sure the first period of time becomes a pure 8 hours,
	// which makes detecting the main section of the workday easier.
	if(splitpoints[5] < splitpoints[3]){
		splitpoints[5] = splitpoints[3];
	}
	
	std::sort(splitpoints, splitpoints + 10);
	
	int j = 0;
	for(int i = 0; i<10; i++) {
		const moment* each_moment = &splitpoints[i];
		if(*each_moment > call && *each_moment < wrap) {
			blocks[j++] = timesplit(initial_block, *each_moment);
		}
	}
	
	blocks[j++] = initial_block;
	total_timeblocks = j;
	
	// THE VALUE-FACTOR CALCULATION PART
	
	// TODO: Implement a good system for this fuckin' paragraph:
	// A. 50 % tillegg for arbeid inntil 2 timer før, eller inntil 3 timer etter ordinær arbeidstid når arbeidstiden ikke er forskjøvet og overtiden er varslet. Dersom det varsles overtid både før og etter ordinær arbeidstid betales de to første timene med 50 % tillegg og de øvrige med 100 % tillegg.
	
	for(timeblock& each_block : blocks){ // C++11 stuff right here.
		if(each_block.hourcount() == 8) {
			each_block.valuefactor = 7.5/8.0;
			if(each_block.start.getweekday() == saturday) each_block.valuefactor *= 1.5;
			if(each_block.start.getweekday() == sunday) each_block.valuefactor *= 2;
			// TODO: This is a bad solution, but it makes other stuff easier for now.
			// Note: Yes, a simple float is capable of storing 7.5/8*1.5 perfectly.
			// The real solution is to be able to add lunchbreaks.
			// Note: It is starting to look like this band-aid solution is untennable even for basic functionality.
			continue;
		}
		if(each_block.end <= splitpoints[0]) each_block.upvalue(3); // +200% for sleep-breach
		if(each_block.start.hours >= 22) each_block.upvalue(2);			// Work between 22:00
		if((each_block.end.hours == 6 && each_block.end.minutes == 0) ||// And 06:00
		   (each_block.end.hours <= 5)) each_block.upvalue(2);			// is +100%
		if(each_block.start >= splitpoints[3]) {each_block.upvalue(1.5); // Overtime
			if(each_block.start.getweekday() == saturday) each_block.upvalue(2);// on saturdays
		}
		if(each_block.start >= splitpoints[6]) each_block.upvalue(3); // +200% beyond 14-hour mark
		if(each_block.start.getweekday() == saturday) each_block.upvalue(1.5);// Saturdays are +50%
		if(each_block.start.getweekday() == sunday) each_block.upvalue(2); // Sundays are +100%
	}
	
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

float timeblock::upvalue(float suggestion){
	if(suggestion>valuefactor) valuefactor = suggestion;
	return valuefactor;
}

weekday moment::getweekday() {
	// Based on implementation from NProg on StackOverflow. Thanks.
	int y = year;
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    y -= month < 3;
    return static_cast<weekday>((y + y / 4 - y / 100 + y / 400 + t[month - 1] + day - 1) % 7);
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
	if(splitpoint <= input_block.start || splitpoint >= input_block.end) {
		std::cerr << "ERROR: Splitpoint outside of timeblock!\n";
		std::cerr << "Timeblock: " << timeprint(input_block) << std::endl;
		std::cerr << "Splitpoint: " << timeprint(splitpoint) << std::endl;
	}
	timeblock output{input_block.start, splitpoint};
	input_block.start = splitpoint; // Note: Now, reversed.
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
void wind(moment& input_moment, const delta& time_delta) {
	wind(input_moment, time_delta.minutes, time_delta.hours, time_delta.days);
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
moment timeinput(moment input_moment) {
	char input_string[6];
	std::cout << "HH MM (24-hour format, use space)\n";
	std::cin.getline(input_string, 6);
	
	// This uglyness is just how you use strtok() to split a string, apparently
	const char* p;
	int split_input[2];
	int i{0};
	p = strtok(input_string, " ");
	while (p != NULL) {
		split_input[i] = int(atoi(p));
		i++;
		p = strtok(NULL, " ");
	}
	
	if((moment){split_input[1], split_input[0],
			input_moment.day, input_moment.month, input_moment.year} < input_moment)
	{
		wind(input_moment, 0, 0, 1);
	}
	
	moment output{split_input[1], split_input[0],
			input_moment.day, input_moment.month, input_moment.year};
	return output;
}

moment timeinput() {
	char input_string[17];
	std::cout << "YEAR MM DD hh mm (24-hour format, use spaces)\n";
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
