# MyAlarm

makes it easy to create tasks that run at a specific time or periodically after an interval.

Tasks scheduled at a particular time of day are called Alarms.  
Tasks scheduled after an interval of time has elapsed are called Timers.  
These tasks can be created to continuously repeat or repeat n times or to occur once only.  
## API
`timerAlarm.update()`  
should be called frequently or placed in the loop, it's where the alarms and timers are serviced.   

`timerAlarm.createHour(min, sec, hourAlarm)`  
Create an alarm that fire every hour at the specified minute and second.  

`timerAlarm.createDay(hour,min, sec, dayAlarm)`  
Create an alarm that fire every day at the specified time.  

`timerAlarm.createWeek(dayOfWeek,hour,min, sec, weekAlarm)`  
`timerAlarm.createWeek(daysOfweekArray,hour,min, sec, arrSize, weekAlarm)`  
Create an alarm that fire every week at the specified time and days of week.  

`timerAlarm.createMonth(dayOfMonth,hour,min, sec, monthAlarm)`  
`timerAlarm.createMonth(daysOfMonthArray,hour,min, sec, arrSize, monthAlarm)`  
Create an alarm that fire every month at the specified time and days of month.  

`timerAlarm.createYear(month,dayOfMonth,hour,min, sec, monthAlarm)`  
`timerAlarm.createYear(month,daysOfMonthArray,hour,min, sec, arrSize, monthAlarm)`  
Create an alarm that fire every year at the specified time, month and days of month.  

`timerAlarm.createTimer(hour, min, sec, myTimer)`  
Create a timer that fire every time the duration specified elapsed.  

If you want to create a timer or alarm that trigger once or n times chain 'once()' or 'repeat(n)' with create  
Ex : `timerAlarm.createHour(min, sec, hourAlarm).once()`   
     `timerAlarm.createHour(min, sec, hourAlarm).repeat(n)` if n equal to 0, it will repeat indefinitly   
 
 ### Other functions
 
 `timerAlarm.setTimeProvider(cb_Provider)`  
 `cb_Provider` is a function that return the current unix time stamp.  
 
 `timerAlarm.stopService()`  
 `timerAlarm.startService()`   
 Start and stop servicing alarms and timers.
 
 `timerAlarm.getNextAlarm()`
 Return the alarm or timer that will trigger next.   
 If used inside the callback will return the current alarm or timer.   
 you can use it to get :`timerAlarm.getNextAlarm().getCounter()` return how many times fired and other information (see instance functions).  
 
`timerAlarm.countAvailable()` get the number of alarms or timers that you can use.  
`timerAlarm.freeAll()` free all the alarms and timers and make theme available.   
`timerAlarm.getAlarmById(id)`   
`timerAlarm.recalculateAllTriggers()`  
`timerAlarm.getGlobalNextTrigger()` get the unix time stamp of the next trigger of all alarms and timers.   

### Instance functions
`getCounter()`   
`disable()`   
`enable()`   
`free()` clear the object and make it available for reuse   
`getId()`   
`getNextTrigger()` return the unix time stamp of the next trigger   
`isActive()`   
`isNull()`  
`isTimer()`   
`resetTimer()`   
`resetCounter()`  
`setCallback(cb)`   
`repeat(n)`   
`once()`   
 
 
FAQ
---
_Q: What hardware and software is needed to use this library?_

A: This library requires an SDK with a ctime implementation. No internal or external hardware is used by the Alarm library.

_Q: Are there any restrictions on the code in a task handler function?_

A: No. The scheduler does not use interrupts so your task handling function is no different from other functions you create in your sketch. 

(If you need timer intervals shorter than 1 second then you should look for a different library)

_Q: How are scheduled tasks affected if the system time is changed?_

A: Tasks are scheduled for specific times designated by the system clock. If the system time is reset to a later time (for example one hour ahead) then all alarms will occur one hour later.

If the system time is set backwards (for example one hour back) then the alarms will occur an hour earlier.

If the time is reset before the time a task was scheduled, then the task will be triggered on the next service (the next call to Cron.delay).
This is  the expected behaviour for Alarms (tasks scheduled for a specific time of day will trigger at that time), but the affect on task for subfractions may not be intuitive. If a timer is scheduled to trigger in 5 minutes time and the clock is set ahead by one hour, that timer will not trigger until one hour and 5 minutes has elapsed.

_Q: How many alarms can be created?_

A: It depends on the system. Up to five alarms can be scheduled in Arduino AVR based boards and 20 for other boards.
The number of alarms can be changed by defining `_NBALARMS` to the desired number befor including the library.   
exemple:  
`#define _NBALARMS 10`   
`#include <MyAlarm.hpp>`

once Alarms or Timers are freed when they are triggered so another once alarm or timer can be set to trigger again.
