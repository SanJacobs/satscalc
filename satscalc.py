#!usr/bin/env python3

# satscalc.py - script to help with billing for Norwegian film workers
# Copyright (C) 2021 Sander Skjegstad
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program. If not, see https://www.gnu.org/licenses/

import datetime

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


def hours_between(time1, time2):
    """Returns the difference beween two times."""
    if time2 > time1:
        return (time2 - time1).total_seconds()/3600
    return (time1 - time2).total_seconds()/3600


def calc_overtime_factor(hours, warned):
    """Loops through and sums together hour-pay-factor for every hour of overtime"""
    output = 0
    hours_passed = 1
    
    if warned:
        time_and_half_length = 3
    else:
        time_and_half_length = 1
    
    while hours > 0:
        if hours_passed <= time_and_half_length:
            # 50% extra for the first 3 hours of warned overtime, first 1 hour of unwarned.
            output += 1.5 * min(1, hours)
        elif hours_passed <= 7:
            # 100% extra for all overtime beyond 3 hours.
            output += 2 * min(1, hours)
        else:
            # 200% extra for all overtime beyond 7 hours.
            # FIXME: The actual rule is 200% extra for anything beyond a 14-hour day, but since there is no support for pre-call work yet, this is fine. For now.
            output += 3 * min(1, hours)
        hours -= 1
        hours_passed += 1
    
    return output


def floatify(hour, minutes):
    # Turns hour and minute into a single float that is easier to do math with
    return float(hour+(minutes/60.0))




if __name__ == "__main__":
    
    print("This program comes with ABSOLUTELY NO WARRANTY.")
    print("This is free software, and you are welcome to redistribute it")
    print("under the conditions of the GPL, v3 or later.\n")
    
    baserate = int(input("What's your base dayrate (sats)? "))
    hourly_rate = baserate/7.5

    print("Hourly rate:", str(hourly_rate))
    
    social_costs = not "n" in input("Add 26% social costs? [y/n] ").lower()

    print("\nTime to add your hours of work.\n")

# --- Data gathering loop ---

    workdays = []

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
        print(f"Wraptime:",wraptime)
        print(f"Delta: {hours_worked} hours.\n")

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
            workdays.append({
                "call": calltime,
                "wrap": wraptime,
                "otw": warned_overtime,
                "ot": overtime_hours,
                }) 
        else:
            workdays.append({
                "call": calltime,
                "wrap": wraptime,
                }) 
        
            
        
    # --- Price calculation ---

    # NOTE:
    # First hour of warned overtime are 50%, unwarned is 100%
    # Any work between 22:00 and 06:00 gets at least a 100% bonus
    # If you get warned about some overtime, but they go beyond that, anything beyond that should be treated as thouogh it was unwarned.
    # This means I'll eventually have to implement asking for the specific time things were supposed to be done.
    
    # TODO: Take weekends into account.

    for day_index, each_day in enumerate(workdays):
        
        if day_index > 0:
            previous_day = workdays[day_index-1]
            # TODO: Possibly just put the stuff about 10 hours minimum between workdays here. That's a 200% bonus.
        else:
            previous_day = 0
        
        worktime = hours_between(each_day["call"], each_day["wrap"])
        each_day["hours"] = worktime
        
        if worktime == 8:
            each_day["category"] = "Normal day"
            each_day["price"] = round(baserate, 2)
        elif worktime > 8:
            each_day["category"] = "Overtime"
            overtime = worktime-8
            ot_factor = calc_overtime_factor(overtime, each_day["otw"])
            each_day["price"] = round(baserate + (hourly_rate*ot_factor), 2)
        elif 4 < worktime < 8:
            each_day["category"] = "Short day"
            overtime = worktime-8
            each_day["price"] = round(hourly_rate*worktime, 2)
        elif worktime <= 4:
            each_day["category"] = "4-hour limit"
            each_day["price"] = round(hourly_rate*4, 2)
        else:
            print("This shouldn't be possible, something is very wrong.")
            quit()
            
        

    # --- Printing receipt ---
    
    total = 0
    
    print("Receipt")
    print("-----\n")
    print("\n\nCalculation via satscalc by SanJacobs.")
    print("Please note point 15 (Disclaimer of Warranty) of the GPLv3.")
    for day_index, each_day in enumerate(workdays):
        print("\n---")
        print(f"Day #{day_index+1}: {each_day['call'].date()} - {each_day['category']}\n")
        
        print("Calltime:", str(each_day["call"].time()))
        print("Finished:", str(each_day["wrap"].time()))
        if "ot" in each_day:
            print("Overtime:", each_day["ot"], "hour(s)")
            print("Warned?", "Yes" if each_day["otw"] else "No")
        print("Price:", each_day["price"])
        total += each_day["price"]
    
    print("\n\n-----")
    if social_costs:
        print("Sum:", total)
        print("TOTAL:", round(total*1.26, 2))
    else:
        print("SUM AND TOTAL:", round(total, 2))
