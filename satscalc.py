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

# NOTE: This Timesplitters branch is me trying out a completely new method calculating the cost of each day
# No idea if it will work, yet, but that's what we're doing this for.

# Another NOTE for my future self: Remember to check out the "holidays" library.

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
    """Returns the difference beween two times in hours."""
    if time2 > time1:
        return (time2 - time1).total_seconds()/3600
    return (time1 - time2).total_seconds()/3600


def calc_price_of_day():
    """Takes all the info needed about a day, and outputs how much that day ought to cost"""
    pass


def floatify(hour, minutes):
    """Turns hour and minute into a single float that is easier to do math with"""
    return float(hour+(minutes/60.0))




if __name__ == "__main__":
    
    print("This program comes with ABSOLUTELY NO WARRANTY.")
    print("This is free software, and you are welcome to redistribute it")
    print("under the conditions of the GPL, v3 or later.\n")
    
    user_ins = {}
    
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
            user_ins["warned_overtime"] = input("Was this overtime fully or parially warned about before 12 o'clock the day before? [f/p/n]: ").lower()
            if "y" in user_ins["warned_overtime"]:
                warned_overtime = True
            elif "p" in user_ins["warned_overtime"]:
                while True:
                    planned_wrap = timeInput("When were you supposed to be done?")
                    # FIXME: These +8 hours comparisons will probably cause a bug when the workday crosses midnighes midnight
                    if planned_wrap < wraptime and planned_wrap > calltime+datetime.timedelta(hours=8):
                        break
                    elif planned_wrap <= calltime+datetime.timedelta(hours=8):
                        print("That's not overtime.")
                    print("")
                
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
        elif 4 <= worktime < 8:
            each_day["category"] = "Short day"
            each_day["price"] = round(hourly_rate*worktime, 2)
        elif worktime < 4:
            each_day["category"] = "4-hour limit"
            each_day["price"] = round(hourly_rate*4, 2)
        else:
            print("This shouldn't be possible, something is very wrong.")
            quit()
            
        

    # --- Printing receipt ---
    
    total = 0
    
    print("\n\n\nReceipt")
    print("-----")
    print("\nCalculation via satscalc by SanJacobs.")
    print("Please note point 15 of the GPLv3. (Disclaimer of Warranty)")
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
