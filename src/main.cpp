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

// TODO: Write function/method/constructor that slices a workday up into the differently priced segments
// TODO: Make the system that determines the price of each of those slices of time
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

		//int dayrate_input;
		//std::cout << "What is your dayrate? ";
		//std::cin >> dayrate_input;
		const int dayrate = 3338;
		std::cout << "Dayrate: " << dayrate << "\n";
		const double hourly_rate = dayrate/7.5;
		std::cout << "Hourly rate: " << hourly_rate << "\n\n";
		
		std::cout << "-----\nStep 1: Adding the days\n\n";
		
		std::cout << "How many days do you want to submit?" << std::endl;
		int number_of_days;
		// std::cin >> number_of_days;
		number_of_days = 3; // Just here for debugging
		std::vector<workday> workdays;
		
		moment previous_wrap{0, 16, 20, 11, 1000}; // Set to a long time ago
		
		for(int day=0; day<number_of_days; day++) {
			
			std::cout << "\n - DAY " << day+1 << "-\nCalltime:\n";
			moment calltime = timeinput();
			std::cout << "\nWraptime:\n";
			moment wraptime = timeinput();
			std::cout << "\nPlanned wraptime:\n";
			moment planned_wraptime = timeinput();
			
			workdays.push_back({previous_wrap,
					calltime,
					wraptime,
					planned_wraptime});
			
			workday* current_workday = &workdays[day];
			
			std::cout << "\nCalltime: " << timeprint(current_workday->call) << "\n";
			std::cout << "Wraptime: " << timeprint(current_workday->wrap) << "\n";
			std::cout << "Planned wrap: " << timeprint(current_workday->planned_wrap) << "\n\n";
			
			for(int i=0; i<current_workday->total_timeblocks; i++) {
				std::cout << "Timeblock " << i << ": " << timeprint(current_workday->blocks[i])
					<< ". Total hours: " << current_workday->blocks[i].hourcount() << std::endl;
			}
			previous_wrap = wraptime;
		}
		
		return 0;
	}
}

