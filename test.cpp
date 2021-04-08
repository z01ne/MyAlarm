//#include <iostream>
#include <stdio.h> /* puts, printf */
#include <time.h>  /* time_t, struct tm, time, localtime */
#include <string>
#include <iostream>
#include "jsontst.h"

using namespace std;

void testTime()
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current local time and date: %s", asctime(timeinfo));

    tm t = *timeinfo;
    printf("h %d ,a %d \n", t.tm_hour, t.tm_year + 1900);

    time_t tim;
    tm ptr;

    int y = 2017, m = 4, d = 20;
    char weekday[7][20] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    time(&tim);
    ptr = *localtime(&tim);
    time_t t2 = mktime(&ptr);
    printf("test %d - %d\n", tim, t2);
    // tm_year is time since 1900
    // ptr.tm_year = y - 1900;
    // ptr.tm_mon = m - 1;
    // ptr.tm_mday = d;
    // ptr.tm_hour = 0;
    // ptr.tm_min = 0;
    // ptr.tm_sec = 0;

    printf("%d\n", mktime(&ptr));
    printf("April 20, 2017 was %s", weekday[ptr.tm_wday]);
}

struct AlarmDesc
{
    char title[30];
    bool once;
    bool enabled;
    int8_t min;
    int8_t hour;
    int8_t daysOfWeek[7];
    int8_t daysOfMonth[31];
    int8_t month;
    int8_t id = -1;
    int8_t device = -1;
    int8_t alarmType; //0 off 1 on
};
struct config
{
    char ssid[20];
    char pwd[20];
    AlarmDesc alarmsArr[10];
} cfg;

void alarmStrctToJson(AlarmDesc &sAlrm, JsonObject &jAlrm)
{
    JsonArray dw = jAlrm.createNestedArray("dw");
    JsonArray dm = jAlrm.createNestedArray("dm");

    for (auto &&d : sAlrm.daysOfWeek)
        dw.add(d);
    for (auto &&d : sAlrm.daysOfMonth)
        dm.add(d);

    jAlrm["alarmType"] = sAlrm.alarmType;
    jAlrm["min"] = sAlrm.min;
    jAlrm["hour"] = sAlrm.hour;
    jAlrm["month"] = sAlrm.month;
    jAlrm["device"] = sAlrm.device;
    jAlrm["enabled"] = sAlrm.enabled;
    jAlrm["id"] = sAlrm.id;
    jAlrm["once"] = sAlrm.once;
    jAlrm["title"] = sAlrm.title;
}
void alarmJsonToStrct(JsonObject &jAlrm, AlarmDesc &sAlrm)
{
    JsonArray dw = jAlrm["dw"];
    JsonArray dm = jAlrm["dm"];

    for (int8_t i = 0; i < 31; i++)
    {
        sAlrm.daysOfMonth[i] = -1;
        if (i < 7)
            sAlrm.daysOfWeek[i] = -1;
    }

    for (auto &&v : dw)
    {
        int8_t d = v;
        sAlrm.daysOfWeek[d] = d;
    }
    for (auto &&v : dm)
    {
        int8_t d = v;
        sAlrm.daysOfMonth[d - 1] = d;
    }

    strcpy(sAlrm.title, jAlrm["title"]);
    sAlrm.min = jAlrm["min"];
    sAlrm.hour = jAlrm["hour"];
    sAlrm.month = jAlrm["month"];
    sAlrm.enabled = jAlrm["enabled"];
    sAlrm.once = jAlrm["once"];
    sAlrm.id = jAlrm["id"];
    sAlrm.device = jAlrm["device"];
    sAlrm.alarmType = jAlrm["alarmType"];
}
AlarmDesc *findAlrm(int8_t id)
{
    for (AlarmDesc &elm : cfg.alarmsArr)
        if (elm.id == id)
            return &elm;
    return nullptr;
}

int main()
{
    //testTime();
    DynamicJsonDocument doc(3072); //2048

    auto file = R"(
        {
            "ssid": "12345678912345678912",
            "pwd": "123456",
            "alarms": [
                {
                "title": "azerrttty",
                "min": 1,
                "hour":2,
                "dw":[0,1,2,3,4,5,6],
                "dm":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20],
                "enabled": false,
                "once": false,
                "id": 1,
                "device": 1,
                "alarmType": 0
                },
                {
                "title": "azerrttty2",
                "min": 1,
                "hour":2,
                "dw":[0,1,2,3,4,5,6],
                "dm":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20],
                "enabled": false,
                "once": false,
                "id": 1,
                "device": 1,
                "alarmType": 0
                }
            ]
        })";

    DeserializationError error = deserializeJson(doc, file);
    printf("cap %d\n", doc.size());
    if (error)
    {
        printf("deserializeJson() failed: \n");
        printf(error.c_str());
        return false;
    }

    strcpy(cfg.ssid, doc["ssid"]);
    strcpy(cfg.pwd, doc["pwd"]);
    int i = 0;
    for (JsonObject &&elem : doc["alarms"].as<JsonArray>())
    {
        alarmJsonToStrct(elem, cfg.alarmsArr[i]);
        i++;
    }
    printf("//////////////////////////////\n");

    strcpy(cfg.alarmsArr[0].title, "test1");
    strcpy(cfg.alarmsArr[1].title, "test2");

    DynamicJsonDocument doc2(1536);
    doc2["ssid"] = cfg.ssid;
    doc2["pwd"] = cfg.pwd;

    JsonArray alarms = doc2.createNestedArray("alarms");

    for (int i = 0; i < 10; i++)
    {
        if (cfg.alarmsArr[i].id < 0)
            continue;

        JsonObject jo = alarms.createNestedObject();
        alarmStrctToJson(cfg.alarmsArr[i], jo);
    }

    serializeJsonPretty(doc2, std::cout);

    return 0;
}