#include "time.h"

// Look. listen here. There's no way I'm going to start taking DST into account.
// DST transition times are decided using skull dice, by the grand wizards of the state.
// The fact that DST is designed this way, though,
// makes it so you are unlikely to be on the clock during a DST transition.


//
// --- OPERATOR OVERLOADS ---
//

long long sortable_time(const moment input_moment) {
		return stoll(std::to_string(input_moment.year)+
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
	timeblock initial_block{call,
		std::max(
		// Paragraph 6.7 says that up to 2 hours of unused warned overtime counts as worktime, though so that at least one hour of the unused overtime is not counted.
		// (It's unclear if an 8-hour day that ends 3 hours in counts as having 5 hours of unused overtime)
		std::clamp(planned_wrap-(delta){0, 1, 0}, wraptime, planned_wraptime+(delta){0, 2, 0}),
			call+(delta){0, 4, 0})}; 
		//  ^ Minimum 4 hour day ^
	
	const int sp_length = 11;
	moment splitpoints[sp_length]{ // --$-- Points where the price may change --$-- //
		
		// NOTE: Maybe this should also contain the valuefactor associated with the split.
		// Probably the valuefactor leading up to the splitpoint, not the one after.
		// Maybe this should be a struct?
		// Or maybe I should just implement this badly at first just to get it working, and replace it later?
		
		previous_wrap+(delta){0, 10, 0}, // Sleepbreach, 10 hours after previous wrap			0x
		(moment){0, 5, call.day, call.month, call.year}, // 2 hours before 7, aka 5				1x
		(moment){0, 6, call.day, call.month, call.year}, // 6 in the morning					2x
		call+(delta){0, 8, 0}, // Normal 8 hours of work										3x
		call+(delta){0, 9, 0}, // 1st hour of overtime is over									4x
		call+(delta){0, 11, 0}, // 3st hour of overtime is over									5x
		planned_wraptime, // End of warned overtime												6x
		call+(delta){0, 14, 0}, // The 14-hour mark												7x
		(moment){0, 22, call.day, call.month, call.year}, // 22:00 in the evening				8x
		(moment){0, 23, call.day, call.month, call.year}+(delta){0, 1, 0}, // Midnight			9x
		(moment){0, 23, call.day, call.month, call.year}+(delta){0, 7, 0}, // 6, next morning	10x
	};
	
	// Eliminate planned wrap, if it occurs within normal 8-hour period.
	// This is to make sure the first period of time becomes a pure 8 hours,
	// which makes detecting the main section of the workday easier.
	if(splitpoints[6] < splitpoints[3]){
		splitpoints[6] = splitpoints[3];
	}
	
	moment splitpoints_sorted[sp_length];
	std::copy(splitpoints, splitpoints+sp_length, splitpoints_sorted);
	std::sort(splitpoints_sorted, splitpoints_sorted + sp_length);
	
	int j = 0;
	for(int i = 0; i<sp_length; i++) {
		const moment* each_moment = &splitpoints_sorted[i];
		//std::cout << "Splitting: " << timeprint(*each_moment) << "\t\tJ: " << j << "\t\tI: " << i << std::endl;
		// If each splitpoint moment is within the workday, and is not equal to the start of the current block
		if(*each_moment > call && *each_moment < wrap && *each_moment != initial_block.start) {
			blocks[j++] = timesplit(initial_block, *each_moment);
		}
	}
	
	blocks[j++] = initial_block;
	total_timeblocks = j;
	
	// THE VALUE-FACTOR CALCULATION PART
	
	// TODO: Consider replacing splitpoints[x] with redoing the math for code flexibility?
	
	for(int ii=0; ii < total_timeblocks; ii++){
		timeblock& each_block = blocks[ii];
		//std::cout << "pricing: " << timeprint(each_block) << std::endl;
		
		if(each_block.end <= splitpoints[0]) each_block.upvalue(3, "Sleep-breach"); // +200% for sleep-breach
		if(each_block.start.hours >= 22) each_block.upvalue(2, "Night");			// Work between 22:00
		if((each_block.end.hours == 6 && each_block.end.minutes == 0) ||// And 06:00
		   (each_block.end.hours <= 5)) each_block.upvalue(2, "Night");			// is +100%
		if(each_block.start >= splitpoints[3]) {each_block.upvalue(1.5, "Overtime"); // Overtime
			if(each_block.start.getweekday() == saturday) each_block.upvalue(2, "Saturday overtime");// on saturdays
		}
		if(each_block.start >= splitpoints[5]) each_block.upvalue(2, "Overtime"); // End of 3-hour cheap planned overtime
		if(each_block.start >= planned_wraptime &&							// Unwarned overtime
				each_block.start >= splitpoints[4]) each_block.upvalue(2, "Overtime");	// +100% after first hour
		if(each_block.start >= splitpoints[7]) each_block.upvalue(3, "Far overtime"); // +200% beyond 14-hour mark
		if(each_block.start.getweekday() == saturday) each_block.upvalue(1.5, "Saturday");// Saturdays are +50%
		if(each_block.start.getweekday() == sunday) each_block.upvalue(2, "Sunday"); // Sundays are +100%
		
		if(!(call < (moment){0, 7, call.day, call.month, call.year} && // On an offset day...
		   std::min(call+(delta){0,8,0}, wrap) < (moment){0,17,call.day,call.month,call.year})) {
				// This was added for rule 6.11c, but in a world without a defined normal workday,
				// that rule is already covered already by 6.11g, so this is empty.
		}
		
		// Holidays!
		if(each_block.start.day==1 && each_block.start.month==1) each_block.upvalue(2, "New year");
		if(each_block.start.day==1 && each_block.start.month==5) each_block.upvalue(2, "1st of May");
		if(each_block.start.day==17 && each_block.start.month==5) each_block.upvalue(2, "17st of May");
		if((each_block.start.day==25 || each_block.start.day==26) && each_block.start.month==12)
			each_block.upvalue(2, "Christmas");
		moment easter = gaussEaster(each_block.start.year);
		if(each_block.start.day == (easter-(delta){0,0,3}).day &&
		   each_block.start.month == (easter-(delta){0,0,3}).month) each_block.upvalue(2, "Maundy Thursday");
		if(each_block.start.day == (easter-(delta){0,0,2}).day &&
		   each_block.start.month == (easter-(delta){0,0,2}).month) each_block.upvalue(2, "Good Friday");
		if(each_block.start.day == easter.day && each_block.start.month == easter.month) each_block.upvalue(2, "Easter");
		if(each_block.start.day == (easter+(delta){0,0,1}).day &&
		   each_block.start.month == (easter+(delta){0,0,1}).month) each_block.upvalue(2, "Easter");
		if(each_block.start.day == (easter+(delta){0,0,39}).day &&
		   each_block.start.month == (easter+(delta){0,0,39}).month) each_block.upvalue(2, "Feast of the Ascension");
		if(each_block.start.day == (easter+(delta){0,0,49}).day &&
		   each_block.start.month == (easter+(delta){0,0,49}).month) each_block.upvalue(2, "Pentecost");
		if(each_block.start.day == (easter+(delta){0,0,50}).day &&
		   each_block.start.month == (easter+(delta){0,0,50}).month) each_block.upvalue(2, "Pentecost Monday");
	}
	
}

void workday::lunch(const moment& lunch_start, const moment& lunch_end) {
	if(lunch_start > lunch_end){
		std::cout << "ERROR: Lunch ends before it began." << std::endl;
	}
	for(int ii=0; ii < total_timeblocks; ii++){
		timeblock& each_block = blocks[ii];
		timeblock& next_block = blocks[ii+1]; // FIXME: On final loop, next_block will be garbage data
		
		if(each_block.start < lunch_start && each_block.end > lunch_end){
			// If lunch simply occurs within a timeblock
			// Split out the section, discarding the middle
			timeblock first_half = timesplit(each_block, lunch_start);
			each_block.start = lunch_end;
			// Move all points after lunch out by 1
			for(int x=total_timeblocks; x>=ii; x--) {
				blocks[x+1] = blocks[x];
			}
			total_timeblocks++;
			// Re-insert second half of split section into ii+1
			blocks[ii] = first_half;
			return;
			
		} else if(ii!=total_timeblocks-1 && each_block.start < lunch_start && lunch_start < each_block.end &&
				  next_block.start < lunch_end && lunch_end < next_block.end) {
			// If we're not on the final block AND
			// If lunch occurs between two timeblocks
			each_block.end = lunch_start;
			next_block.start = lunch_end;
			return;
			
		} else if(each_block.start == lunch_start) {
			// If lunch starts at the beginning of a timeblock
			each_block.start = lunch_end;
			return;
			
		} else if(each_block.end == lunch_end) {
			// If lunch ends at the end of a timeblock
			each_block.start = lunch_end;
			return;
		}
		// FIXME: If lunch spans across more than 1 border between timeblocks, bad stuff will happen.
		// Maybe there is a more principled way of solving this, that doesn't require writing code
		// for a bunch of edge-cases. If not, write the code.
	}
	return;
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

float timeblock::upvalue(float suggestion, std::string reason){
	if(suggestion>valuefactor) {
		valuefactor  = suggestion;
		price_reason = reason;
	}
	return valuefactor;
}

weekday moment::getweekday() {
	// Based on implementation from NProg on StackOverflow. Thanks.
	int y = year;
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    y -= month < 3;
    return static_cast<weekday>((y + y / 4 - y / 100 + y / 400 + t[month - 1] + day - 1) % 7);
}

bool moment::isEaster() {
	moment easter = gaussEaster(year);
	return (easter.month==month && easter.day==day);
}


//
// --- FUNCTIONS ---
//

moment gaussEaster(int year) {
	// Thanks to Carl Friedrich Gauss for the algorythm
	// Thanks rahulhegde97, bansal_rtk_, code_hunt, sanjoy_62, simranarora5sos and aashutoshparoha on GeeksForGeeks for the implementation I based this on.
    float A, B, C, P, Q, M, N, D, E;
	int easter_month = 0;
	int easter_day = 0;
 
    // All calculations done
    // on the basis of
    // Gauss Easter Algorithm
    A = year % 19;
    B = year % 4;
    C = year % 7;
    P = std::floor((float)year / 100.0);
 
    Q = std::floor((float)(13 + 8 * P) / 25.0);
 
    M = (int)(15 - Q + P - (std::floor)(P / 4)) % 30;
 
    N = (int)(4 + P - (std::floor)(P / 4)) % 7;
 
    D = (int)(19 * A + M) % 30;
 
    E = (int)(2 * B + 4 * C + 6 * D + N) % 7;
 
    int days = (int)(22 + D + E);
	easter_day = days;
 
    // A corner case,
    // when D is 29
    if ((D == 29) && (E == 6)) {
		easter_month = 4;
		easter_day = 19;
    }
    // Another corner case,
    // when D is 28
    else if ((D == 28) && (E == 6)) {
		easter_month = 4;
		easter_day = 18;
    }
    else {
        // If days > 31, move to April
        // April = 4th Month
        if (days > 31) {
			easter_month = 04;
			easter_day = days-31;
        }
        else {
            // Otherwise, stay on March
            // March = 3rd Month
			easter_month = 03;
        }
    }
	return (moment){0,0, easter_day, easter_month, year};
}

std::string padint(const int input, const int minimum_signs) {
	std::ostringstream output;
	output << std::internal << std::setfill('0') << std::setw(minimum_signs) << input;
	return output.str();
}

timeblock timesplit(timeblock& input_block, const moment splitpoint) {
	// Splits a timeblock at splitpoint.
	// It changes the input_block to start at splitpoint, and returns a new timeblock
	// that lasts from where the input_block used to start, to splitpoint.
	// BASICALLY: input_block becomes first half, output is second half.
	if(splitpoint <= input_block.start || splitpoint >= input_block.end) {
		std::cerr << "ERROR: Splitpoint outside of timeblock!\n";
		std::cerr << "Timeblock: " << timeprint(input_block) << std::endl;
		std::cerr << "Splitpoint: " << timeprint(splitpoint) << std::endl;
	}
	timeblock output{input_block.start, splitpoint};
	output.valuefactor = input_block.valuefactor;
	output.price_reason = input_block.price_reason;
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
std::string timeprint(moment input_moment, bool clockonly) {
	using namespace std;
	string output;
	if(clockonly) {
		output =
			padint(input_moment.hours, 2) + ":"
			+ padint(input_moment.minutes, 2);
	} else {
		output =
			to_string(input_moment.year) + "-"
			+ padint(input_moment.month, 2) + "-"
			+ padint(input_moment.day, 2);
	}
	return output;
}

std::string timeprint(timeblock input_timeblock) {
	std::string output{timeprint(input_timeblock.start) + " --> " + timeprint(input_timeblock.end)};
	return output;
}
std::string timeprint(timeblock input_timeblock, bool clockonly) {
	std::string output{timeprint(input_timeblock.start, clockonly) + " --> " + timeprint(input_timeblock.end, clockonly)};
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
	std::cout << "HH MM (24-hour format, use space)" << std::endl;
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
