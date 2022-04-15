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

// TODO: Make a system that effeciently stores a range of time, and lets you split it up neatly
// The slicing function could use a pointer to output the posterior half of the time range into
// The slicing process should figure out how many slices will need to be made before doing the slices, so a correctly sized array can be allocated on the stack instead of using a vec on the heap

// TODO: Make the system that determines the price of each of those slices of time

// TODO: Test the boost time date system, see if it accounts for leap years and DST.

#include <iostream>
#include "time.h"
//#include <boost/date_time/time_duration.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>

int main()
{
	//using boost::posix_time::to_simple_string;
	//tm test_moment_one{0, 30, 14, 27, 11, 2010};
	moment calltime{30, 8, 27, 11, 2010};
	moment wraptime{30, 16, 27, 11, 2010};
	timeblock workday{calltime, wraptime};
	
	std::cout << "\n\n --- TIME UNITS TEST ---\n\n";
	std::cout << "Calltime: " << timeprint(calltime) << std::endl;
	std::cout << "Wraptime: " << timeprint(wraptime) << std::endl;
	
	std::cout << "\nWorkday:\n";
	std::cout << timeprint(workday.start) << " --> " << timeprint(workday.end) << std::endl;
	
	std::cout << "\nSplitting workday into workday and second_half...\n";
	moment splitpoint{0, 12, 27, 11, 2010};
	timeblock second_half{timesplit(workday, splitpoint)};
	
	std::cout << "\nSplitpoint: " << timeprint(splitpoint) << std::endl;
	std::cout << "\nWorkday:\n";
	std::cout << timeprint(workday.start) << " --> " << timeprint(workday.end) << std::endl;
	std::cout << "\nSecond_half:\n";
	std::cout << timeprint(second_half.start) << " --> " << timeprint(second_half.end) << std::endl;
	
	std::cout << "\n\n --- TIME MATH TEST ---\n\n";
	
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
	
	return 0;
}

