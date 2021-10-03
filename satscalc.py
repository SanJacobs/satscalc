#!/usr/bin/env python3
import datetime

# TODO: Prompt user for base rate, call time, wrap time, whether overtime was warned, and if it was a weekend.
# This should be enough to calculate the pay for a single day's work.

baserate = int(input("What's your base dayrate (sats)? "))
hourly_rate = baserate/7.5

print("Hourly rate:", str(hourly_rate))

print("\nTime to add your hours of work.")

def timeInput(prompt):
    while True:
        i = input(prompt)
        if i=="": return ""
        elif len(i)==4: break
        print("Wrong format. Try again.")
        
    t = datetime.datetime.strptime(i,"%H%M")
    
    return t

while True:
    calltime = timeInput("Your call time (24-hour format, hhmm. Leave blank to continue.): ")
    if calltime == "":
        break
    wraptime = timeInput("Your wrap time (24-hour format, hhmm): ")
    
    # Make sure we're not traveling backwards in time
    if wraptime < calltime:
        wraptime = wraptime.replace(day=wraptime.day+1)
    difference = wraptime-calltime
    hours_worked = difference.total_seconds()/3600
    
    # TODO: This part makes no sense. Don't worry. It'll be made to make sense.
    
    input("Was this a weekend? [y/n]: ")
    
    if hours_worked > 8:
        print("Overtime detected.")
        input("Was this overtime warned about before 12 o'clock the day before? [y/n]: ")

print("\nDifference:",difference)
print("Hours worked:",hours_worked)
