#!/usr/bin/env python3
import time

# TODO: Prompt user for base rate, call time, wrap time, whether overtime was warned, and if it was a weekend.
# This should be enough to calculate the pay for a single day's work.

baserate = int(input("What's your base dayrate (sats)? "))
hourly_rate = baserate/7.5

print("Hourly rate:", str(hourly_rate))

# TODO: Investigate if storing multiple calltimes and wraptimes will require a custom datastructure

while True:
    # TODO: Check for valid format
    calltime = input("Your call time (hhmm): ")
    if calltime == "done":
        break
    wraptime = input("Your wrap time (hhmm): ")
    
