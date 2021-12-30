#!usr/bin/env python3

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
    
    baserate = int(input("What's your base dayrate (sats)? "))
    hourly_rate = baserate/7.5

    print("Hourly rate:", str(hourly_rate))
    
    social_costs = not "n" in input("Add 26% social costs? [y/n] ").lower()

    print("\nTime to add your hours of work.")

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
                "ot": overtime,
                }) 
        else:
            workdays.append({
                "call": calltime,
                "wrap": wraptime,
                }) 
        
            
        
    # --- Price calculation ---

    # NOTE:
    # First two hours of warned overtime are 50%, unwarned is 100%
    # Any work after 20:00 is overtime
    # There is a rule that you always have to invoice for 4 hours or more pr day on ad-shoots. Take this into account.
    # If you get warned about some overtime, but they go beyond that, anything beyond that should be treated as thouogh it was unwarned.
    # This means I'll have to ask for the specific time things were supposed to be done.
    
    # TODO: Doesn't take weekends into account yet

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
            each_day["price"] = baserate
        elif worktime > 8:
            each_day["category"] = "Overtime"
            overtime = worktime-8
            ot_factor = calc_overtime_factor(overtime, each_day["otw"])
            each_day["price"] = baserate + (hourly_rate*ot_factor)
        elif 4 < worktime < 8:
            each_day["category"] = "Short day"
            overtime = worktime-8
            each_day["price"] = hourly_rate*worktime
        elif worktime <= 4:
            each_day["category"] = "Sub 4-hour day"
            each_day["price"] = hourly_rate*4
        else:
            print("This shouldn't be possible, something is very wrong.")
            quit()
            
        

    # --- Printing receipt ---
    
    total = 0
    
    print("\n\nCalculation via satscalc by SanJacobs.")
    print("Please note point 15 (Disclaimer of Warranty) of the GPLv3.")
    for day_index, each_day in enumerate(workdays):
        print("\n---")
        print(f"Day #{day_index+1}: {each_day['call'].date()} - {each_day['category']}\n")
        
        print("Calltime: " + str(each_day["call"].time()))
        print("Finished at: " + str(each_day["wrap"].time()))
        if "ot" in each_day:
            print("Overtime: " + each_day["ot"] + " hour(s)")
            print("Warned? " + each_day["otw"])
        print("Price: " + each_day["price"])
        total += each_day["price"]
    
    print("\n\n-----")
    print("TOTAL:", total)
