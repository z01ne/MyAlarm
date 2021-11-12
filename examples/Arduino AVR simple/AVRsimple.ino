#define _NBALARMS 10
#include <MyAlarm.h>

time_t prevMillis = 0;
// time keeping using millis()
void updateTime()
{
  time_t curtime;
  time(&curtime);
  time_t elapsed = millis() - prevMillis;
  if (elapsed >= 1000)
  {
    curtime++;
    prevMillis += elapsed;
    set_system_time(curtime);
  }
}

void nowTimer()
{
  time_t curtime;
  time(&curtime);
  tm *tminfo = localtime(&curtime);
  Serial.print("The current date/time is: ");
  Serial.println(asctime(tminfo));
}

void blinkTimer()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void helloTimer()
{
  Serial.print("Hello ");
  Serial.println(timerAlarm.getNextAlarm().getCounter());
  timerAlarm.createTimer(0, 0, 1, blinkTimer).run(3);
}

void crazyTimer()
{
  uint8_t seconds = random(1, 20);

  Serial.print("crazy timer after ");
  Serial.print(seconds);
  Serial.println(" seconds");

  timerAlarm.createTimer(0, 0, seconds, crazyTimer).runOnce();
}

void morningAlarm()
{
  Serial.println("wake up! it's morning");
}

void weekendAlarm()
{
  Serial.println("the weeeeekend !");
}

void ofTheMonth()
{
  Serial.println("of the month");
}

void endAlarm()
{
  Serial.println("it's the END");
  timerAlarm.createTimer(0, 0, 1, ofTheMonth).run(5);
}

void firstWeekAlarm()
{
  Serial.print("it's day ");
  Serial.println(timerAlarm.getNextAlarm().getCounter());
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  // init time
  tm cDateTime;
  cDateTime.tm_year = 2021 - 1900;
  cDateTime.tm_mon = 8;
  cDateTime.tm_mday = 8;
  cDateTime.tm_hour = 1;
  cDateTime.tm_min = 0;
  cDateTime.tm_sec = 0;
  cDateTime.tm_isdst = 0;
  set_system_time(mktime(&cDateTime));
  ///////////////////////////////

  timerAlarm.createTimer(0, 0, 5, helloTimer);
  timerAlarm.createTimer(0, 0, 1, nowTimer);
  timerAlarm.createTimer(0, 0, 10, crazyTimer).runOnce();
  timerAlarm.createDay(7, 30, 0, morningAlarm);
  int8_t weekEnd[] = {timerAlarm.dw_Saturday, timerAlarm.dw_Sunday};
  timerAlarm.createWeek(weekEnd, 10, 0, 0, 2, weekendAlarm);
  timerAlarm.createMonth(31, 8, 0, 0, endAlarm);
  int8_t firstWeek[] = {1, 2, 3, 4, 5, 6, 7};
  timerAlarm.createMonth(firstWeek, 10, 0, 0, 7, firstWeekAlarm);
}

void loop()
{
  updateTime();
  timerAlarm.update();
}