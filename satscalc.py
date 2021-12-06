#!usr/bin/env python3
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

def timeInput(prompt, fulldate=False):
    
    if fulldate:
        formatprompt="YY/MM/DD HrMn"
        strpformat="%y/%m/%d %H%M"
    else:
        formatprompt="HrMn"
        strpformat="%H%M"
        
    while True:
        print(prompt)
        print(formatprompt)
        i = input()
        # If empty, return empty string
        if i=="": return ""
        try:
            t = datetime.datetime.strptime(i, strpformat)
            break
        except ValueError:
            print("Wrong format. Try again.")
    
    return t

def floatify(hour, minutes):
    # Turns hour and minute into a single float that is easier to do math with
    return float(hour+(minutes/60.0))

# --- Data gathering loop ---

while True:
    calltime = timeInput("CALL TIME (24-hour format. Leave blank to continue.)", True)
    if calltime == "":
        break
    wraptime = timeInput("\nWRAP TIME (24-hour format)")
    
    # Giving wrap same date as call
    wraptime = wraptime.replace(year=calltime.year, month=calltime.month, day=calltime.day)
    # If clock nr at wrap is equal to or lower than call, then a day has passed.
    if floatify(wraptime.hour, wraptime.minute) <= floatify(calltime.hour, calltime.minute):
        wraptime += datetime.timedelta(days=1)
    
    difference = wraptime-calltime
    hours_worked = difference.total_seconds()/3600
    
    print("\nCalltime:",calltime)
    print("Wraptime:",wraptime)
    print("Delta:",hours_worked,"hours.\n")

    # This part makes no sense. Don't worry. It'll be made to make sense.
    
    overtime = hours_worked > 8
    if overtime:
        overtime_hours = hours_worked-8
        print("Overtime detected.")
        warned_overtime = "y" in input("Was this overtime warned about before 12 o'clock the day before? [y/n]: ").lower()
        if warned_overtime:
            if "n" in input("Was it warned of entirely? [y/n] ").lower():
                #overtime_warned_until = timeInput("Until what time was it warned?")
                print("Feature not yet implemented.")
        
    
# --- Price calculation ---

# NOTE:
# First two hours of warned overtime are 50%, unwarned is 100%
# Any work after 20:00 is overtime
# There is a rule that you always have to invoice for 4 hours or more pr day on ad-shoots. Take this into account.
#

# Extremely naive calculation
sum = 0
if overtime:
    sum += overtime_hours*(hourly_rate*1.5)


print("\nDifference:", difference)
print("Hours worked:", hours_worked)
