#ifndef MyAlarm_h
#define MyAlarm_h
#include <time.h> /* time_t, struct tm, time, localtime */

#ifdef __AVR
#ifndef _NBALARMS
#define _NBALARMS 5
#endif
typedef void (*cb_t)();
typedef time_t (*cbProv_t)();
#else
#ifndef _NBALARMS
#define _NBALARMS 20
#endif
#include <functional>
typedef std::function<time_t(void)> cbProv_t;
typedef std::function<void(void)> cb_t;
#endif

#define __LONG_MAX 2147483647L

class MyAlarm
{
private:
    /* data */
    static MyAlarm alarms[_NBALARMS];
    static cbProv_t timeProv;
    static time_t globalNextTrigger;
    static int8_t idNextAlarm;
    static bool globalActive;
    // static MyAlarm dummy;

    cb_t callback;

    int8_t sec = -1, min = -1, hour = -1, month = -1;
    int8_t wDay[7];
    int8_t mDay[31];
    time_t nextTrigger = __LONG_MAX;

    int8_t id = -1;
    bool active = false;
    bool onceFreed = true;
    int8_t mode = 0;
    uint8_t nbRun = 0;        // 0:infinit n:times
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

            tmAl.tm_sec = sec > -1 ? sec : tmAl.tm_sec;
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

    static MyAlarm &create(int8_t _sec, int8_t _minute, int8_t _hour, int8_t _daysOfWeek[], int8_t _daysOfMonth[], int8_t _month, int8_t sizeW, int8_t sizeM, int8_t _type, int8_t _alarmType, const cb_t &_callback)
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
    static MyAlarm dummy;
    enum daysOfWeek_t : uint8_t
    {
        dw_Sunday,
        dw_Monday,
        dw_Tuesday,
        dw_Wednesday,
        dw_Thursday,
        dw_Friday,
        dw_Saturday
    };
    enum monthsOfYear_t : uint8_t
    {
        m_January = 1,
        m_February,
        m_March,
        m_April,
        m_May,
        m_June,
        m_july,
        m_August,
        m_September,
        m_October,
        m_November,
        m_December
    };

    void free()
    {
        if (isFree())
            return;

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
        nbRun = 0;
        callback = NULL;
        nextTrigger = __LONG_MAX;
    }

    void disable(bool only = false)
    {

        if (isFree() || !active)
            return;

        active = false;

        if (!only && id == idNextAlarm)
            calculateNextGlobalTrigger();
    }

    void enable(bool forced = false)
    {
        if (isFree())
            return;
        if (active && !forced)
            return;

        active = true;
        calculateNextTrigger();
        if (forced || nextTrigger <= globalNextTrigger)
            calculateNextGlobalTrigger();
    }

    [[deprecated("Use runOnce(bool).")]] void once(bool _onceFreed = true)
    {
        onceFreed = _onceFreed;
        nbRun = 1;
    }
    [[deprecated("Use run(int,bool).")]] void repeat(uint8_t n = 0, bool _doneFreed = true)
    {
        nbRun = n;
        onceFreed = _doneFreed;
    }
    void run(uint8_t n = 0, bool _doneFreed = true)
    {
        nbRun = n;
        onceFreed = _doneFreed;
    }
    void runOnce(bool _onceFreed = true)
    {
        run(1, _onceFreed);
    }
    uint8_t getnbRun() { return nbRun; }
    void resetCounter() { firedcounter = 0; }
    uint8_t getCounter() { return firedcounter; }
    void resetTimer()
    {
        if (type == 1)
            calculateNextTrigger();
    }
    void setCallback(const cb_t &cb)
    {
        if (cb != NULL)
            callback = cb;
    }

    int8_t getId() { return id; }
    [[deprecated("Use isFree().")]] bool isNull() { return id == -1; }
    bool isFree() { return id == -1; }
    bool isActive() { return active; }
    bool isTimer() { return type == 1; }
    time_t getNextTrigger() { return nextTrigger; }

    /////////////////////////////// Public static  //////////////////////////////////////////////

    static int8_t update()
    {
        if (!globalActive)
            return -2;

        int8_t idCurrent = idNextAlarm;

        if (globalNextTrigger <= timeProv())
        {
            auto &al = alarms[idNextAlarm];
            al.firedcounter++;

            if (al.callback != NULL)
                al.callback();

            // printf("DEBUG: Alarm %d Fired %d\n", al.id, al.firedcounter);
            if (!al.isFree()) // freed in callback
            {
                if (al.nbRun == 0 || al.firedcounter < al.nbRun)
                    al.calculateNextTrigger();
                else if (al.nbRun == al.firedcounter)
                {
                    if (al.onceFreed)
                        al.free();
                    else
                        al.disable(true);
                }
            }

            calculateNextGlobalTrigger();

            return idCurrent;
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
        if (id >= 0 && id < _NBALARMS)
        {
            MyAlarm &al = alarms[id];
            if (al.id > -1)
                return al;
            else
                return dummy;
        }
        return dummy;
    }

    static void setTimeProvider(const cbProv_t &tp)
    {
        if (tp != NULL)
            timeProv = tp;
        // else
        //     timeProv = []()
        //     { return time(nullptr); };
    }

    static void stopService() { globalActive = false; }
    static void startService(bool reset = true)
    {
        if (!globalActive)
        {
            globalActive = true;
            if (reset)
                recalculateAllTriggers();
        }
    }
    static bool isServicing() { return globalActive == true; }

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

    static MyAlarm &createHour(int8_t _minute, int8_t _second, const cb_t &_callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[1];
        int8_t month = -1;

        return create(_second, _minute, -1, _daysOfWeek, _daysOfMonth, month, 0, 0, 0, 0, _callback);
    }
    static MyAlarm &createDay(int8_t _hour, int8_t _minute, int8_t _second, const cb_t &_callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[1];
        int8_t month = -1;

        return create(_second, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, 0, 0, 1, _callback);
    }
    static MyAlarm &createWeek(daysOfWeek_t _dayOfWeek, int8_t _hour, int8_t _minute, int8_t _second, const cb_t &_callback)
    {
        int8_t _daysOfWeek[]{(int8_t)_dayOfWeek};
        int8_t _daysOfMonth[1];
        int8_t month = -1;

        return create(_second, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 1, 0, 0, 2, _callback);
    }
    static MyAlarm &createWeek(int8_t _daysOfWeek[], int8_t _hour, int8_t _minute, int8_t _second, int8_t size, const cb_t &_callback)
    {
        int8_t _daysOfMonth[1];
        int8_t month = -1;

        return create(_second, _minute, _hour, _daysOfWeek, _daysOfMonth, month, size, 0, 0, 2, _callback);
    }
    static MyAlarm &createMonth(int8_t _dayOfMonth, int8_t _hour, int8_t _minute, int8_t _second, const cb_t &_callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[]{_dayOfMonth};
        int8_t month = -1;

        return create(_second, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, 1, 0, 3, _callback);
    }
    static MyAlarm &createMonth(int8_t _daysOfMonth[], int8_t _hour, int8_t _minute, int8_t _second, int8_t size, const cb_t &_callback)
    {
        int8_t _daysOfWeek[1];
        int8_t month = -1;

        return create(_second, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, size, 0, 3, _callback);
    }
    static MyAlarm &createYear(int8_t month, int8_t _dayOfMonth, int8_t _hour, int8_t _minute, int8_t _second, const cb_t &_callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[]{_dayOfMonth};
        month--;
        return create(_second, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, 1, 0, 4, _callback);
    }
    static MyAlarm &createYear(int8_t month, int8_t _daysOfMonth[], int8_t _hour, int8_t _minute, int8_t _second, int8_t size, const cb_t &_callback)
    {
        int8_t _daysOfWeek[1];
        month--;
        return create(_second, _minute, _hour, _daysOfWeek, _daysOfMonth, month, 0, size, 0, 4, _callback);
    }
    static MyAlarm &createTimer(int8_t _hour, int8_t _minute, int8_t _sec, const cb_t &_callback)
    {
        int8_t _daysOfWeek[1];
        int8_t _daysOfMonth[1];
        return create(_sec, _minute, _hour, _daysOfWeek, _daysOfMonth, 0, 0, 0, 1, 0, _callback);
    }
};
MyAlarm MyAlarm::alarms[_NBALARMS];
#ifdef _STM32_DEF_ // bug in time(nullptr)
cbProv_t MyAlarm::timeProv = []()
{ return __LONG_MAX; };
#else
cbProv_t MyAlarm::timeProv = []()
{ return time(nullptr); };
#endif
time_t MyAlarm::globalNextTrigger = __LONG_MAX;
int8_t MyAlarm::idNextAlarm = -1;
bool MyAlarm::globalActive = true;
MyAlarm MyAlarm::dummy;
MyAlarm &timerAlarm = MyAlarm::dummy;
#endif