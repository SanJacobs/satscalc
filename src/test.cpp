#include "test.h"
#include "time.h"

void test(){
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
	moment inputmoment2 = timeinput(inputmoment);
	std::cout << "Time reveived: " << timeprint(inputmoment2) << std::endl;
}
