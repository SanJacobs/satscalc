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
		
		while(1) {
			std::cout << "Filling in a test-day, and that's it.\n";
			std::cout << "Storing multiple workdays in an efficient way will be figured out later." << std::endl;
			
			workday test_day({0, 0, 1, 1, 1000},
					{0, 8, 20, 11, 2022},
					{0, 21, 20, 11, 2022},
					{0, 18, 20, 11, 2022});
			
			std::cout << "\nCalltime: " << timeprint(test_day.call) << "\n";
			std::cout << "Wraptime: " << timeprint(test_day.wrap) << "\n";
			std::cout << "Planned wrap: " << timeprint(test_day.planned_wrap) << "\n\n";
			
			for(int i=0; i<test_day.total_timeblocks; i++) {
				std::cout << "Segment " << i << ": " << timeprint(test_day.blocks[i]) << std::endl;
			}
			
			break;
		}
		
		return 0;
	}
}

