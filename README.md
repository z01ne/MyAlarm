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
 
`timerAlarm.countAvailable()` get the number of alarms or timers that you can create.  
`timerAlarm.freeAll()` free all the alarms and timers and make theme available.   
`timerAlarm.getAlarmById(id)`   
`timerAlarm.recalculateAllTriggers()` 

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
     
