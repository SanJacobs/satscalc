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

/* TODO: Make a system that effeciently stores a range of time, and lets you split it up neatly */
/* TODO: Make the system that determines the price of each of those slices of time */

#include <iostream>
#include <boost/date_time/time_duration.hpp>

int main()
{
	tm test_moment_one{0, 30, 14, 27, 11, 2010};
	std::cout << "Testing 123" << std::endl;
	return 0;
}

