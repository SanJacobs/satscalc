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

int main(int argc, char* argv[])
{
	if(argc > 1){
		if(std::string(argv[1])=="test") {
			moment calltime{30, 8, 27, 11, 2010};
			moment wraptime{30, 16, 27, 11, 2010};
			timeblock workday{calltime, wraptime};

			std::cout << "\n\n --- TIME UNITS TEST ---\n\n";
			std::cout << "Calltime: " << timeprint(calltime) << std::endl;
			std::cout << "Wraptime: " << timeprint(wraptime) << std::endl;

			std::cout << "\nWorkday:\n";
			std::cout << timeprint(workday) << std::endl;

			
			
			std::cout << "\n\n --- TIME MATH TEST ---\n\n";
			
			std::cout << "Difference between calltime and wraptime:\n";
			std::cout << (calltime-wraptime) << std::endl;
			std::cout << "Other way around, should show same result:\n";
			std::cout << (calltime-wraptime) << std::endl;
			std::cout << "As method on workday:\n";
			std::cout << (workday.hourcount()) << std::endl;

			std::cout << "\nSplitting workday into workday and second_half...\n";
			moment splitpoint{0, 12, 27, 11, 2010};
			timeblock first_half{timesplit(workday, splitpoint)};

			std::cout << "\nSplitpoint: " << timeprint(splitpoint) << std::endl;
			std::cout << "\nfirst_half:\n";
			std::cout << timeprint(first_half) << std::endl;
			std::cout << "\nWorkday:\n";
			std::cout << timeprint(workday) << std::endl;

			std::cout << "\nSplitting second_half at erronious point...\n";
			moment erronious_splitpoint{0, 14, 27, 11, 2010};
			std::cout << timeprint(timesplit(first_half, erronious_splitpoint)) << "\n";

			
			
			std::cout << "\n\n --- TIME WINDING TEST ---\n\n";
			
			moment testtime{30, 8, 25, 2, 2012};
			std::cout << "Testtime: " << timeprint(testtime) << std::endl;


			std::cout << "\nForwarding 45 minutes...\n";
			wind(testtime, 45, 0, 0);
			std::cout << "Testtime: " << timeprint(testtime) << std::endl;

			std::cout << "\nRewinding 45 minutes...\n";
			wind(testtime, -45, 0, 0);
			std::cout << "Testtime: " << timeprint(testtime) << std::endl;


			std::cout << "\nForwarding 20 hours...\n";
			wind(testtime, 0, 20, 0);
			std::cout << "Testtime: " << timeprint(testtime) << std::endl;

			std::cout << "\nRewinding 20 hours...\n";
			wind(testtime, 0, -20, 0);
			std::cout << "Testtime: " << timeprint(testtime) << std::endl;


			std::cout << "\nForwarding 10 days...\n";
			wind(testtime, 0, 0, 10);
			std::cout << "Testtime: " << timeprint(testtime) << std::endl;

			std::cout << "\nRewinding 10 days...\n";
			wind(testtime, 0, 0, -10);
			std::cout << "Testtime: " << timeprint(testtime) << std::endl;
			
			
			
			std::cout << "\n\n --- TIME INPUT TEST ---\n\n";
			
			moment inputmoment = timeinput();
			std::cout << "Time reveived: " << timeprint(inputmoment) << std::endl;
			moment inputmoment2 = timeinput(2012, 11, 26);
			std::cout << "Time reveived: " << timeprint(inputmoment2) << std::endl;
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

