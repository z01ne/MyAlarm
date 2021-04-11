#include <iostream>
#include "MyAlarm.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// #define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include "doctest.h"

using namespace std;

time_t currentTime;

void setTime(int sec, int min, int hour = -1, int mDay = 1, int month = -1, int year = 2021)
{
    if (month != -1)
        month--;
    auto t = time(nullptr);
    tm now = *localtime(&t);
    now.tm_year = year - 1900;
    now.tm_mon = month == -1 ? 0 : month;
    now.tm_mday = mDay == -1 ? 1 : mDay;
    now.tm_hour = hour == -1 ? 0 : hour;
    now.tm_min = min;
    now.tm_sec = sec;
    //now.tm_isdst = -1;
    currentTime = mktime(&now);
}

time_t tp()
{
    return currentTime;
}

void cb()
{
    printf("Hello world\n");
    printf("id : %d\n", MyAlarm::getNextAlarm().getId());
}

TEST_CASE("alarm creation")
{
    MyAlarm::setTimeProvider([]() { return currentTime; });
    MyAlarm &al = MyAlarm::createHour(10, cb);
    int a = al.countAvailable();
    int id = al.getId();
    CHECK(a == 19);
    CHECK_MESSAGE(id > -1, "first alarm");
    MyAlarm &al2 = MyAlarm::createHour(10, cb);
    a = al2.countAvailable();
    int id2 = al2.getId();
    CHECK(a == 18);

    CHECK_MESSAGE(id2 > -1, "second alarm", id2);
    CHECK_MESSAGE(id != id2, "different ids", id, id2);

    auto &alhour = MyAlarm::createDay(1, 1, cb);
    CHECK(alhour.getId() > -1);

    auto &alweek = MyAlarm::createWeek(MyAlarm::Thursday, 1, 1, cb);
    CHECK(alweek.getId() > -1);

    int8_t dw[]{MyAlarm::Monday, MyAlarm::Thursday, MyAlarm::Tuesday, MyAlarm::Wednesday};
    auto &alweeks = MyAlarm::createWeek(dw, 1, 1, 4, cb);
    CHECK(alweeks.getId() > -1);

    auto &almon = MyAlarm::createMonth(1, 1, 1, cb);
    CHECK(almon.getId() > -1);

    int8_t dm[]{9, 1, 5, 4, 7};
    auto &almons = MyAlarm::createMonth(dm, 1, 1, 5, cb);
    CHECK(almons.getId() > -1);

    MyAlarm::freeAll();
}

TEST_CASE("Hour")
{
    setTime(0, 9); //1609456140
    MyAlarm::setTimeProvider(tp);
    MyAlarm &al = MyAlarm::createHour(10, cb);
    CHECK(al.getNextTrigger() == 1609456200);

    CHECK(MyAlarm::update() == -1);
    setTime(0, 10); //1609456200
    CHECK(MyAlarm::update() == 0);
    setTime(0, 11);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 11, 1);
    CHECK(MyAlarm::update() == 0); // next hour

    auto &al2 = MyAlarm::createHour(11, cb);
    CHECK(MyAlarm::update() == -1);

    setTime(0, 13, 2);
    CHECK(MyAlarm::update() == 0);
    CHECK(MyAlarm::update() == 1);
    CHECK(MyAlarm::update() == -1);

    MyAlarm::freeAll();
}

TEST_CASE("Day")
{
    setTime(0, 0, 1);
    MyAlarm::setTimeProvider(tp);
    MyAlarm &al = MyAlarm::createDay(1, 1, cb);

    CHECK(MyAlarm::update() == -1);
    setTime(0, 1, 1);
    CHECK(MyAlarm::update() == 0);
    setTime(0, 2, 1);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 1, 1, 2);
    CHECK(MyAlarm::update() == 0);

    setTime(0, 2, 1, 2);
    auto &al2 = MyAlarm::createDay(2, 2, cb);
    CHECK(MyAlarm::update() == -1);

    setTime(0, 2, 2, 2);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 3, 2, 2);
    CHECK(MyAlarm::update() == -1);

    setTime(0, 2, 1, 3);
    CHECK(MyAlarm::update() == 0);
    setTime(0, 2, 2, 3);
    CHECK(MyAlarm::update() == 1);

    setTime(0, 3, 2, 3);
    CHECK(MyAlarm::update() == -1);

    MyAlarm::freeAll();
}

TEST_CASE("Week")
{
    setTime(0, 0, 0, 1, 2);
    MyAlarm::setTimeProvider(tp);
    MyAlarm &al = MyAlarm::createWeek(MyAlarm::Thursday, 0, 0, cb);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 0, 0, 4, 2);
    auto t = al.getNextTrigger();
    CHECK(MyAlarm::update() == 0);
    t = al.getNextTrigger();
    setTime(0, 0, 0, 5, 2);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 0, 0, 11, 2);
    CHECK(MyAlarm::update() == 0);
    t = al.getNextTrigger();
    int8_t wd[]{MyAlarm::Thursday, MyAlarm::Wednesday, MyAlarm::Friday};
    MyAlarm &al2 = MyAlarm::createWeek(wd, 1, 0, 3, cb);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 0, 2, 11, 2);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 0, 1, 12, 2);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 0, 1, 17, 2);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 0, 1, 18, 2);
    CHECK(MyAlarm::update() == 0);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 0, 1, 19, 2);
    CHECK(MyAlarm::update() == 1);
    CHECK(MyAlarm::update() == -1);

    MyAlarm::freeAll();
}

TEST_CASE("Month")
{
    setTime(0, 0, 0, 1, 1);
    MyAlarm::setTimeProvider(tp);
    MyAlarm &al = MyAlarm::createMonth(2, 0, 0, cb);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 0, 0, 2, 1);
    CHECK(MyAlarm::update() == 0);
    setTime(0, 1, 0, 2, 1);
    CHECK(MyAlarm::update() == -1);

    int8_t md[]{1, 2, 3, 31};
    MyAlarm &al2 = MyAlarm::createMonth(md, 0, 1, 4, cb);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 1, 0, 3, 1);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 1, 0, 31, 1);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 1, 0, 1, 2);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 1, 0, 2, 2);
    CHECK(MyAlarm::update() == 0);
    CHECK(MyAlarm::update() == 1);

    CHECK(MyAlarm::update() == -1);
    MyAlarm::freeAll();
    // last day of month
    setTime(0, 0, 0, 1, 1);
    MyAlarm &al3 = MyAlarm::createMonth(31, 0, 0, cb);
    setTime(0, 0, 0, 31, 1);
    CHECK(MyAlarm::update() == 0);
    setTime(0, 0, 0, 28, 2);
    CHECK(MyAlarm::update() == 0);
    setTime(0, 0, 0, 31, 3);
    CHECK(MyAlarm::update() == 0);
    setTime(0, 0, 0, 30, 4);
    CHECK(MyAlarm::update() == 0);
    MyAlarm::freeAll();
}

TEST_CASE("Year")
{
    setTime(0, 0, 0, 1, 1, 2021);
    MyAlarm::setTimeProvider(tp);
    MyAlarm &al = MyAlarm::createYear(1, 2, 0, 0, cb);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 0, 0, 2, 1, 2021);
    CHECK(MyAlarm::update() == 0);
    setTime(0, 0, 0, 2, 1, 2022);
    CHECK(MyAlarm::update() == 0);
    setTime(0, 0, 0, 5, 2, 2022);
    CHECK(MyAlarm::update() == -1);

    int8_t md[]{1, 2, 3, 31};
    MyAlarm &al2 = MyAlarm::createYear(1, md, 0, 1, sizeof(md), cb);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 1, 0, 1, 1, 2023);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 1, 0, 2, 1, 2023);
    CHECK(MyAlarm::update() == 0);
    CHECK(MyAlarm::update() == 1);
    CHECK(MyAlarm::update() == -1);
    setTime(0, 1, 0, 3, 1, 2023);
    CHECK(MyAlarm::update() == 1);
    setTime(0, 1, 0, 31, 1, 2023);
    CHECK(MyAlarm::update() == 1);

    setTime(0, 1, 0, 31, 5, 2023);
    CHECK(MyAlarm::update() == -1);
    MyAlarm::freeAll();
}

TEST_CASE("Timer")
{
    setTime(0, 0, 1);
    MyAlarm::setTimeProvider(tp);
    auto &timer = MyAlarm::createTimer(1, 1, 2, cb);
    timer.once();
    MyAlarm::update();
    CHECK(timer.isNull() == false);

    setTime(0, 1, 2);
    MyAlarm::update();
    CHECK(timer.isNull() == false);

    setTime(2, 1, 2);
    MyAlarm::update();
    CHECK(timer.isNull() == true);

    auto &timer2 = MyAlarm::createTimer(1, 1, 2, cb);
    timer2.repeat(3);

    setTime(4, 2, 3);
    CHECK(MyAlarm::update() == 0);

    setTime(6, 3, 4);
    CHECK(MyAlarm::update() == 0);

    setTime(8, 4, 5);
    CHECK(MyAlarm::update() == -1);
    CHECK(timer.isNull() == true);

    auto &timer3 = MyAlarm::createTimer(1, 1, 2, cb);
}