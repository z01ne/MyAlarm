#ifndef MyAlarm_h
#define MyAlarm_h
#include <time.h> /* time_t, struct tm, time, localtime */

#include <functional>

#ifndef _NBALARMS
#define _NBALARMS 20
#endif
#define __LONG_MAX 2147483647L

class MyAlarm
{
private:
    /* data */
    static MyAlarm alarms[_NBALARMS];
    static std::function<time_t(void)> timeProv;
    static time_t globalNextTrigger;
    static int8_t idNextAlarm;
    static bool globalActive;
    static MyAlarm dummy;

    std::function<void(void)> callback;

    int8_t sec = -1, min = -1, hour = -1, month = -1;
    int8_t wDay[7];
    int8_t mDay[31];
    time_t nextTrigger = __LONG_MAX;

    int8_t id = -1;
    bool active = false;
    bool onceFreed = true;
    int8_t mode = 0;
    uint8_t repeatn = 0;      // 0:infinit n:times
    uint8_t firedcounter = 0; // count fired 255
    int8_t type = 0;          // 0:alarm  1:timer
    int8_t alarmType = 0;     // 0:hour 1:day 2:week 3:month 4:year

    void calculateNextTrigger()
    {
        if (!active)
            return;

        time_t tnow = timeProv();
        if (type == 1) // timer
            nextTrigger = tnow + sec + min * 60 + hour * 3600;
        else // alarm
        {
            tm tmAl = *localtime(&tnow); // init Now;
            int8_t nbwDay = 0, amon = month;

            for (int8_t i = 0; i < 7; i++)
                if (wDay[i] != -1)
                    nbwDay++;

            tmAl.tm_sec = 0;
            tmAl.tm_min = min > -1 ? min : tmAl.tm_min;
            tmAl.tm_hour = hour > -1 ? hour : tmAl.tm_hour;
            tmAl.tm_mon = month > -1 ? month : tmAl.tm_mon;

            time_t tTrigger = mktime(&tmAl);

            switch (alarmType)
            {
            case 0: // hour
                if (tTrigger > tnow)
                {
                    nextTrigger = tTrigger;
                }
                else // next hour
                {
                    tmAl.tm_hour++;
                    tTrigger = mktime(&tmAl);
                    nextTrigger = tTrigger;
                }
                break;
            case 1: // dayly
                if (tTrigger > tnow)
                {
                    nextTrigger = tTrigger;
                }
                else // next day
                {
                    tmAl.tm_mday++;
                    tTrigger = mktime(&tmAl);
                    nextTrigger = tTrigger;
                }
                break;
            case 2: // weekly
            {
                uint8_t md = tmAl.tm_mday, wd = tmAl.tm_wday, d = tmAl.tm_wday;
                if (nbwDay == 1) // 1 week day
                {
                    int8_t awd = -1; // alarm next week day
                    for (int8_t i = 0; i < 7; i++)
                        if (wDay[i] != -1)
                        {
                            awd = i;
                            break;
                        }

                    int8_t dif = awd - wd;
                    if (dif < 0)
                        dif = -dif + 1;
                    tmAl.tm_mday = md + dif;
                    tTrigger = mktime(&tmAl);
                    if (tTrigger > tnow)
                        nextTrigger = tTrigger;
                    else // when alarm week day == current
                    {
                        tmAl.tm_mday += 7; // next week
                        tTrigger = mktime(&tmAl);
                        nextTrigger = tTrigger;
                    }
                }
                else
                {
                    for (uint8_t i = 0; i < 7; i++) // search all wday set from current
                    {
                        if (wDay[d] != -1)
                        {
                            int8_t dif = wDay[d] - wd;
                            if (dif < 0)
                                dif = -dif + 1;
                            tmAl.tm_mday = md + dif;
                            tTrigger = mktime(&tmAl);
                            if (tTrigger > tnow)
                            {
                                nextTrigger = tTrigger;
                                break;
                            }
                        }
                        d++;
                        if (d >= 7)
                            d = 0;
                    }
                }
                break;
            }
            case 3: // monthly
            case 4: // yearly
            {
                if (amon != -1) // month is set?
                    tmAl.tm_mon = amon;
                int8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                int8_t monthLastDay = monthDays[tmAl.tm_mon];
                for (int8_t i = tmAl.tm_mday - 1; i < 31; i++)
                {
                    if (mDay[i] == -1)
                        continue;
                    // month overflow
                    tmAl.tm_mday = mDay[i] > monthLastDay ? monthLastDay : mDay[i];
                    tTrigger = mktime(&tmAl);
                    if (tTrigger > tnow) // next day
                    {
                        nextTrigger = tTrigger;
                        break;
                    }
                }
                // next month or year
                if (tTrigger <= tnow)
                {
                    int8_t fmd = -1; // first day set
                    for (auto &&d : mDay)
                        if (d != -1)
                        {
                            fmd = d;
                            break;
                        }
                    monthLastDay = monthDays[tmAl.tm_mon == 11 ? 0 : tmAl.tm_mon + 1]; // last day of the next month
                    tmAl.tm_mday = fmd > monthLastDay ? monthLastDay : fmd;
                    if (amon == -1) // next month
                        tmAl.tm_mon++;
                    else // next year
                        tmAl.tm_year++;
                    tTrigger = mktime(&tmAl);
                    nextTrigger = tTrigger;
                }
                break;
            }
            default: // should not
                free();
                break;
            }
        }
    }

    static void calculateNextGlobalTrigger()
    {
        globalNextTrigger = __LONG_MAX;
        idNextAlarm = -1;
        for (auto &&al : alarms)
            if (al.active)
            {
                if (al.nextTrigger < globalNextTrigger)
                {
                    globalNextTrigger = al.nextTrigger;
                    idNextAlarm = al.id;
                }
            }
    }

    static int8_t available()
    {
        for (int8_t i = 0; i < _NBALARMS; i++)
            if (alarms[i].id == -1)
                return i;
        return -1;
    }

    MyAlarm(/* args */)
    {
        for (int8_t i = 0; i < 31; i++)
        {
            if (i < 7)
                wDay[i] = -1;
            mDay[i] = -1;
        }
    }

    static MyAlarm &create(int8_t _sec, int8_t _minute, int8_t _hour, int8_t _daysOfWeek[], int8_t _daysOfMonth[], int8_t _month, int8_t sizeW, int8_t sizeM, int8_t _type, int8_t _alarmType, std::function<void(void)> _callback)
    {
        auto id = available();
        if (id == -1)
            return dummy;

        MyAlarm &al = alarms[id];
        al.id = id;
        al.sec = _sec;
        al.min = _minute;
        al.hour = _hour;
        al.month = _month;
        al.type = _type;
        al.alarmType = _alarmType;
        al.callback = _callback;

        for (int8_t i = 0; i < sizeW; i++)
            al.wDay[_daysOfWeek[i]] = _daysOfWeek[i];

        for (int8_t i = 0; i < sizeM; i++)
            al.mDay[_daysOfMonth[i] - 1] = _daysOfMonth[i];

        al.enable(true);
        return al;
    }

public:
    enum daysOfWeek_t
    {
        Sunday,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday
    };

    void free()
    {
        if (active && id == idNextAlarm)
        {
            active = false;
            calculateNextGlobalTrigger();
        }

        active = false;
        id = -1;
        sec = -1, min = -1, hour = -1, month = -1;
        for (int8_t i = 0; i < 31; i++)
        {
            if (i < 7)
                wDay[i] = -1;
            mDay[i] = -1;
        }
        firedcounter = 0;
        repeatn = 0;
        callback = NULL;
    }

    void disable(bool only = false)
    {
        if (!active)
            return;

        active = false;

        if (!only && id == idNextAlarm)
            calculateNextGlobalTrigger();
    }

    void enable(bool forced = false)
    {
        if (active && !forced)
            return;

        active = true;
        calculateNextTrigger();
        if (forced || nextTrigger <= globalNextTrigger)
            calculateNextGlobalTrigger();
    }

    void once(bool _onceFreed = true)
    {
        onceFreed = _onceFreed;
        repeatn = 1;
    }
    void repeat(uint8_t n = 0, bool _doneFreed = true)
    {
        repeatn = n;
        onceFreed = _doneFreed;
    }
    void resetCounter() { firedcounter = 0; }
    void resetTimer() { calculateNextTrigger(); }
    void setCallback(std::function<void(void)> cb)
    {
        if (cb != NULL)
            callback = cb;
    }

    int8_t getId() { return id; }
    bool isNull() { return id == -1; }
    bool isActive() { return active; }
    bool isTimer() { return type == 1; }
    time_t getNextTrigger() { return nextTrigger; }

    /////////////////////////////// Public static  //////////////////////////////////////////////

    static int8_t update()
    {
        if (!globalActive)
            return -1;

        if (globalNextTrigger <= timeProv())
        {
            auto &al = alarms[idNextAlarm];
            al.firedcounter++;
            al.callback();

            if (al.repeatn == 0)
                al.calculateNextTrigger();
            else if (al.repeatn == al.firedcounter)
            {
                if (al.onceFreed)
                    al.free();
                else
                    al.disable(true);
            }
            calculateNextGlobalTrigger();

            return al.id;
        }
        return -1;
    }

    static MyAlarm &getNextAlarm()
    {
        if (idNextAlarm == -1)
            return dummy;
        else
            return alarms[idNextAlarm];
    }
    static MyAlarm &getAlarmById(int8_t id)
    {
        if (id >= 0 && id <= _NBALARMS)
        {
            MyAlarm &al = alarms[id];
            if (al.id > -1)
                return al;
            else
                return dummy;
        }
        return dummy;
    }

    static void setTimeProvider(std::function<time_t(void)> tp)
    {
        if (tp != NULL)
            timeProv = tp;
        else
            timeProv = []() { return time(nullptr); };
    }

    static void stopService() { globalActive = false; }
    static void startService() { globalActive = true; }
    static int8_t countAvailable()
    {
        int8_t cnt = 0;
        for (auto &&al : alarms)
            cnt += al.id == -1 ? 1 : 0;
        return cnt;
    }
    static time_t getGlobalNextTrigger() { return globalNextTrigger; }

    static void recalculateAllTriggers()
    {
        for (auto &&al : alarms)
            if (al.id != -1 && al.active)
                al.calculateNextTrigger();
        calculateNextGlobalTrigger();
    }

    static void freeAll()
    {
        for (auto &&al : alarms)
            if (al.id != -1)
                al.free();
        globalNextTrigger = __LONG_MAX;
        idNextAlarm = -1;
    }

    static MyAlarm &createHour(int8_t _minute, std::function<void(void)> _callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[1];
        int8_t month = -1;

        return create(0, _minute, -1, _daysOfWeek, _daysOfMonth, month, 0, 0, 0, 0, _callback);
    }
    static MyAlarm &createDay(int8_t _hour, int8_t _minute, std::function<void(void)> _callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[1];
        int8_t month = -1;

        return create(0, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, 0, 0, 1, _callback);
    }
    static MyAlarm &createWeek(daysOfWeek_t _dayOfWeek, int8_t _hour, int8_t _minute, std::function<void(void)> _callback)
    {
        int8_t _daysOfWeek[]{_dayOfWeek};
        int8_t _daysOfMonth[1];
        int8_t month = -1;

        return create(0, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 1, 0, 0, 2, _callback);
    }
    static MyAlarm &createWeek(int8_t _daysOfWeek[], int8_t _hour, int8_t _minute, int8_t size, std::function<void(void)> _callback)
    {
        int8_t _daysOfMonth[1];
        int8_t month = -1;

        return create(0, _minute, _hour, _daysOfWeek, _daysOfMonth, month, size, 0, 0, 2, _callback);
    }
    static MyAlarm &createMonth(int8_t _dayOfMonth, int8_t _hour, int8_t _minute, std::function<void(void)> _callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[]{_dayOfMonth};
        int8_t month = -1;

        return create(0, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, 1, 0, 3, _callback);
    }
    static MyAlarm &createMonth(int8_t _daysOfMonth[], int8_t _hour, int8_t _minute, int8_t size, std::function<void(void)> _callback)
    {
        int8_t _daysOfWeek[1];
        int8_t month = -1;

        return create(0, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, size, 0, 3, _callback);
    }
    static MyAlarm &createYear(int8_t month, int8_t _dayOfMonth, int8_t _hour, int8_t _minute, std::function<void(void)> _callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[]{_dayOfMonth};
        month--;
        return create(0, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, 1, 0, 4, _callback);
    }
    static MyAlarm &createYear(int8_t month, int8_t _daysOfMonth[], int8_t _hour, int8_t _minute, int8_t size, std::function<void(void)> _callback)
    {
        int8_t _daysOfWeek[1];
        month--;
        return create(0, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, size, 0, 4, _callback);
    }
    static MyAlarm &createTimer(int8_t _hour, int8_t _minute, int8_t _sec, std::function<void(void)> _callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[1];
        return create(_sec, _minute, _hour, _daysOfWeek, _daysOfMonth, 0, 0, 0, 1, 0, _callback);
    }
};
MyAlarm MyAlarm::alarms[_NBALARMS];
std::function<time_t(void)> MyAlarm::timeProv = []() { return time(nullptr); };
time_t MyAlarm::globalNextTrigger = __LONG_MAX;
int8_t MyAlarm::idNextAlarm = -1;
bool MyAlarm::globalActive = true;
MyAlarm MyAlarm::dummy;
#endif