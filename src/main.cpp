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

int main(int argc, char* argv[])
{
	if(argc > 1){
		if(std::string(argv[1])=="test") {
			test(); // Test time-stuff
		}
	} else {
		std::cout << "satscalc (C++ Edition)\n";
		std::cout << "Copyright 2022 Sander J. Skjegstad.\n";
		std::cout << "This is free software with ABSOLUTELY NO WARRENTY.\n";
		std::cout << "It does NOT give financial advice.\n\n";

		std::cout << "-----\nStep 0: Setting the dayrate\n\n";

		int dayrate_input;
		std::cout << "Dayrate: ";
		std::cin >> dayrate_input;
		std::cin.ignore();
		const int dayrate = dayrate_input;
		
		const double hourly_rate = dayrate/7.5;
		std::cout << "Hourly rate: " << hourly_rate << "\n\n";
		
		std::cout << "-----\nStep 1: Adding the days\n\n";
		
		std::cout << "How many days do you want to enter? ";
		int number_of_days;
		std::cin >> number_of_days;
		std::cin.ignore();
		//number_of_days = 1; // Just here for debugging
		delta default_daylength{0, 8, 0};
		std::vector<workday> workdays;
		
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
				std::cout << "\033[2J\033[1;1H";
				std::cout << "\n------------- DAY " << day+1 << " -------------\n";
				std::cout << "[1] Calltime: " << timeprint(calltime) << "\n";
				std::cout << "[2] Wraptime: " << timeprint(wraptime) << "\n";
				std::cout << "[3] Planned wrap: " << timeprint(planned_wraptime) << "\n";
				std::cout << "[4] Lunch: " << timeprint((timeblock){lunch_start, lunch_end}) << "\n" << std::endl;
				// TODO: Add second lunch option
				
				std::cout << "Write the number for what you want to change, or 0 to go to the next day." << std::endl;
				
				int input_number = 8;
				std::cin >> input_number;
				std::cin.ignore();
				
				std::cout << "\n";
				switch (input_number) {
					case 0:
						goto done;
					case 1:
						std::cout << "Changing calltime.\n";
						calltime = timeinput();
						break;
					case 2:
						std::cout << "Changing actual wraptime.\n";
						wraptime = timeinput(calltime);
						break;
					case 3:
						std::cout << "Changing planned wraptime.\n";
						planned_wraptime = timeinput(calltime);
						break;
					case 4:
						std::cout << "Changing lunch-time.\n";
						std::cout << "Lunch start:\n";
						lunch_start = timeinput(calltime);
						std::cout << "Lunch end:\n";
						lunch_end = timeinput(calltime);
						break;
					default:
						std::cout << "ERROR: That's not a valid number, ya doofus. 1, 2, 3, 4 or 0 only." << std::endl;
				}
			}
done:
			
			workdays.push_back({previous_wrap,
					calltime,
					wraptime,
					planned_wraptime});
			
			if(lunch_start != lunch_end) workdays[day].lunch(lunch_start, lunch_end);
			
			workday* current_workday = &workdays[day];
			
			std::cout << "\nCalltime: " << timeprint(current_workday->call) << "\n";
			std::cout << "Wraptime: " << timeprint(current_workday->wrap) << "\n";
			std::cout << "Planned wrap: " << timeprint(current_workday->planned_wrap) << "\n\n";
			
			for(int i=0; i<current_workday->total_timeblocks; i++) {
				std::cout << "Timeblock " << i << ": " << timeprint(current_workday->blocks[i])
					<< ". Total hours: " << current_workday->blocks[i].hourcount()
					<< "\t Valuefactor: " << current_workday->blocks[i].valuefactor << std::endl;
			}
			
			// This assumes that the user is entering things chronologically, which they may not be
			previous_wrap = wraptime;
			std::cout << "\033[2J\033[1;1H";
		}
		std::cout << "\n\n\n -+-+-+-+-+-+-+- CALCULATION -+-+-+-+-+-+-+-\n\n";
		std::cout << "Dayrate: " << dayrate << "\n";
		std::cout << "Hourly rate: " << hourly_rate << "\n";
		
		double total_sum = 0;
		for(int ii=0; ii < number_of_days; ii++) {
			workday& each_day = workdays[ii];
			double day_price = 0;
			std::cout << "\n ----- Day " << ii+1 << ": " << timeprint(each_day.call, false) << " ----- " << "\n";
			
			for(int jj=0; jj < each_day.total_timeblocks; jj++) {
				timeblock& each_block = each_day.blocks[jj];
				double block_price = each_block.hourcount() * hourly_rate * each_block.valuefactor;
				std::cout << timeprint(each_block, true) << ", " << each_block.hourcount() << "h\t+" << (each_block.valuefactor-1)*100 << "%\t= " << block_price << "\n";
				day_price += block_price;
			}
			std::cout << "Price of day " << ii+1 << ": " << day_price << std::endl;
			total_sum += day_price;
		}
		
		std::cout << "\nSUM: " << total_sum << std::endl;
		
		
		return 0;
	}
}

