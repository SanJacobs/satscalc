/*
satscalc - software to help with billing for Norwegian film workers
Copyright (C) 2022 Sander Skjegstad

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see https://www.gnu.org/licenses/
*/

// TODO: Add ruleset selector (That only allows you to pick the advert ruleset at first)

#include <iostream>
#include "time.h"
#include "test.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #define CLEAR std::system("cls")
#else
    #define CLEAR std::cout << "\033[2J\033[1;1H"
#endif

int main(int argc, char* argv[])
{
	if(argc > 1){
		if(std::string(argv[1])=="test") {
			test(); // Test time-stuff
			return 0;
		}}
	std::cout << "satscalc (C++ Edition)\n";
	std::cout << "Copyright 2022 Sander J. Skjegstad.\n";
	std::cout << "This is free software with ABSOLUTELY NO WARRENTY.\n";
	std::cout << "It does NOT give financial advice.\n\n";

	std::cout << "-----\nStep 0: Setting the dayrate\n\n";

	int dayrate;
	std::cout << "Dayrate: ";
	std::cin >> dayrate;
	std::cin.ignore();
	
	long double hourly_rate = dayrate/7.5;
	std::cout << "Hourly rate: " << hourly_rate << "\n\n";
	
	std::cout << "How many days do you want to enter? ";
	int number_of_days;
	std::cin >> number_of_days;
	std::cin.ignore();
	delta default_daylength{0, 8, 0};
	long double social_costs = 1.26;
	
	while(1) {
		CLEAR;
		std::cout << "\n------------- Setup and defaults -------------\n";
		std::cout << "[d] Dayrate: " << dayrate << " (" << hourly_rate << " hourly)\n";
		std::cout << "[w] Workdays to input: " << number_of_days << "\n";
		std::cout << "[l] Default length of workday: " << default_daylength.hours << "h\n";
		std::cout << "[s] Social costs: " << (social_costs-1)*100 << "%\n";
		//std::cout << "[l] Lunch: " << timeprint((timeblock){lunch_start, lunch_end}) << "\n" << std::endl;
		
		std::cout << "\nWrite the letter for what you want to change, or 0 to continue." << std::endl;
		
		char input_char = '5';
		std::cin >> input_char;
		std::cin.ignore();
		
		std::cout << "\n";
		switch (input_char) {
			case '0':
				goto setup_done;
			case 'd':
				std::cout << "Changing dayrate.\nDayrate: ";
				std::cin >> dayrate;
				std::cin.ignore();
				hourly_rate = dayrate/7.5;
				break;
			case 'w':
				std::cout << "Changing number of workdays.\nDays: ";
				std::cin >> number_of_days;
				std::cin.ignore();
				break;
			case 'l':
				std::cout << "Changing default workday length:\nHours: ";
				std::cin >> default_daylength.hours;
				std::cin.ignore();
				break;
			case 's':
				{
				std::cout << "Changing social cost percentage:\nSocial cost %";
				int social_input = 0;
				std::cin >> social_input;
				std::cin.ignore();
				social_costs = (social_input+100.0f)/100.0f;
				break;
				}
			default:
				std::cout << "ERROR: That's not a valid char, ya doofus." << std::endl;
		}
	}
setup_done:
	CLEAR;
	
	//number_of_days = 1; // Just here for debugging
	workday workdays[number_of_days];
	
	moment previous_wrap{0, 16, 20, 11, 1000}; // Set to a long time ago
	
	for(int day=0; day<number_of_days; day++) {
		
		// TODO: Inputing the dates should be done with a custom function and layout,
		// not std::cin, because it openly allows for invalid input
		std::cout << "\n - DAY " << day+1 << "-\nCalltime:\n";
		moment calltime = timeinput();
		
		moment wraptime = calltime+default_daylength;
		moment planned_wraptime = wraptime;
		// TODO: Maybe lunch should be 0000-00-00 00:00 if day is short.
		moment lunch_start = calltime+(delta){0,4,0}; 
		moment lunch_end = lunch_start+(delta){30,0,0};
		
		while(1) {
			CLEAR;
			std::cout << "\n------------- DAY " << day+1 << " -------------\n";
			std::cout << "[c] Calltime: " << timeprint(calltime) << "\n";
			std::cout << "[w] Wraptime: " << timeprint(wraptime) << "\n";
			std::cout << "[p] Planned wrap: " << timeprint(planned_wraptime) << "\n";
			std::cout << "[l] Lunch: " << timeprint((timeblock){lunch_start, lunch_end}) << "\n";
			// TODO: Add second lunch option
			
			std::cout << "\nWrite the letter for what you want to change, or 0 to go to the next day." << std::endl;
			
			char input_char = '5';
			std::cin >> input_char;
			std::cin.ignore();
			
			std::cout << "\n";
			switch (input_char) {
				case '0':
					goto done;
				case 'c':
					std::cout << "Changing calltime.\n";
					calltime = timeinput();
					break;
				case 'w':
					std::cout << "Changing actual wraptime.\n";
					wraptime = timeinput(calltime);
					break;
				case 'p':
					std::cout << "Changing planned wraptime.\n";
					planned_wraptime = timeinput(calltime);
					break;
				case 'l':
					std::cout << "Changing lunch-time.\n";
					std::cout << "Lunch start:\n";
					lunch_start = timeinput(calltime);
					std::cout << "Lunch end:\n";
					lunch_end = timeinput(calltime);
					break;
				default:
					std::cout << "ERROR: That's not a valid number, ya doofus." << std::endl;
			}
		}
done:
		
		workdays[day] = {previous_wrap,
				calltime,
				wraptime,
				planned_wraptime};
		
		if(lunch_start != lunch_end) workdays[day].lunch(lunch_start, lunch_end);
		
		std::cout << "\nCalltime: " << timeprint(workdays[day].call) << "\n";
		std::cout << "Wraptime: " << timeprint(workdays[day].wrap) << "\n";
		std::cout << "Planned wrap: " << timeprint(workdays[day].planned_wrap) << "\n\n";
		
		for(int i=0; i<workdays[day].total_timeblocks; i++) {
			std::cout << "Timeblock " << i << ": " << timeprint(workdays[day].blocks[i])
				<< ". Total hours: " << workdays[day].blocks[i].hourcount()
				<< "\t Valuefactor: " << workdays[day].blocks[i].valuefactor << std::endl;
		}
		
		// This assumes that the user is entering things chronologically, which they may not be
		previous_wrap = wraptime;
		CLEAR;
	}
	std::cout << "\n\n\n -+-+-+-+-+-+-+- CALCULATION -+-+-+-+-+-+-+-\n\n";
	std::cout << "Dayrate: " << dayrate << "\n";
	std::cout << "Hourly rate: " << hourly_rate << "\n";
	std::cout << "Social costs: " << (social_costs-1)*100 << "%\n";
	
	double total_sum = 0;
	for(int ii=0; ii < number_of_days; ii++) {
		workday& each_day = workdays[ii];
		double day_price = 0;
		std::cout << "\n ----- Day " << ii+1 << ": " << timeprint(each_day.call, false) << " ----- " << "\n";
		
		for(int jj=0; jj < each_day.total_timeblocks; jj++) {
			timeblock& each_block = each_day.blocks[jj];
			double block_price = each_block.hourcount() * hourly_rate * each_block.valuefactor;
			
			std::cout << timeprint(each_block, true) << ", "
					<< each_block.hourcount() << "h\t+"
					<< (each_block.valuefactor-1)*100 << "%\t= "
					<< block_price;
			//if(each_block.price_reason != "Norm") {
				std::cout << "  \t[" << each_block.price_reason << "]";
			//}
			std::cout << "\n";
			
			day_price += block_price;
		}
		
		double day_length = (timeblock){each_day.call, each_day.wrap}.hourcount();
		
		if((each_day.call.hours < 6 || each_day.call.hours >= 22) &&
				(each_day.wrap.hours < 6 || each_day.wrap.hours >= 22) &&
				day_length < 9)
		{
			std::cout << "Day set entirely between 22:00 and 06:00.\nPrice is dayrate plus 100% for hours worked. [§6.12B]\n";
			day_price = dayrate + (day_length * hourly_rate);
			
		} else if((each_day.call.hours <  6  || each_day.call.hours >= 10) &&
				(each_day.wrap.hours > 22 || each_day.wrap.hours <= 10)){
				if(day_price < dayrate){
					day_price = dayrate;
					
					std::cout << "Offset day at least partially set between 22:00 and 06:00.\nMinimum price is full dayrate. [§6.12A]\n";
				}
		}
		std::cout << "Price of day " << ii+1 << ": " << day_price << std::endl;
		total_sum += day_price;
	}
	
	std::cout << "\nSum: " << total_sum;
	std::cout << "\nTotal: " << total_sum*social_costs << std::endl;
	
	
	return 0;
}

