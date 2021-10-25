#!/usr/bin/env python3
import datetime

# NOTE: Maybe the whole thing should be re-thought to actually prompt for date.
# That way you can easily fill in a calendar of sorts, which is much easier to loop through and calculate what hours were overtime etc.
# This can probably be done with some smarts, which assume that if you start at 08:00, 07:00 doesn't mean the same day.
# Meaning you only need to prompt for the date of the call time, not the date of the wrap time.
# This will also automatically let you know if it was a weekend and stuff like that.

# TODO: Prompt user for base rate, call time, wrap time, whether overtime was warned, and if it was a weekend.
# This should be enough to calculate the pay for a single day's work.

# These accumulate gradually as you enter hours
normal_hours =  0.0
ot1_hours =  0.0
ot2_hours =  0.0
ot3_hours =  0.0

baserate = int(input("What's your base dayrate (sats)? "))
hourly_rate = baserate/7.5

print("Hourly rate:", str(hourly_rate))

print("\nTime to add your hours of work.")

def timeInput(prompt):
    while True:
        i = input(prompt)
        # If empty, return empty string
        if i=="": return ""
        elif len(i)==4: break
        print("Wrong format. Try again.")
        
    t = datetime.datetime.strptime(i,"%H%M")
    
    return t

while True:
    calltime = timeInput("Your call time (24-hour format, hhmm. Leave blank to continue.): ")
    # TODO: Detect turnover problems here, because the previous wraptime is still left over from the previous looparound
    # Though you may still need cache the previous two times before you write those hours to the accumulators. We'll see.
    if calltime == "":
        break
    wraptime = timeInput("Your wrap time (24-hour format, hhmm): ")
    
    # Make sure we're not traveling backwards in time
    if wraptime < calltime:
        wraptime = wraptime.replace(day=wraptime.day+1)
    difference = wraptime-calltime
    hours_worked = difference.total_seconds()/3600
    
    # TODO: This part makes no sense. Don't worry. It'll be made to make sense.
    # TODO: Apparently there is a rule that you always have to invoice for 4 hours or more on ad-shoots. Investigate.
    
    weekend = "y" in input("Was this a weekend? [y/n]: ").lower()
    
    overtime = hours_worked > 8
    if overtime:
        overtime_hours = hours_worked-8
        print("Overtime detected.")
        warned_overtime = "y" in input("Was this overtime warned about before 12 o'clock the day before? [y/n]: ").lower()
    
# --- Price calculation ---

# Notes:
# First two hours of warned overtime are 50%, unwarned is 100%
# Any work after 20:00 is overtime
#

# Extremely naive calculation
sum = 0
if overtime:
    sum += overtime_hours*(hourly_rate*1.5)


print("\nDifference:", difference)
print("Hours worked:", hours_worked)
