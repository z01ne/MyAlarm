// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2020
// MIT License

#include <iostream>
#include "jsontst.h"

void merge(JsonVariant dst, JsonVariantConst src)
{
    if (src.is<JsonObject>())
    {
        for (auto kvp : src.as<JsonObject>())
        {
            merge(dst.getOrAddMember(kvp.key()), kvp.value());
        }
    }
    else
    {
        dst.set(src);
    }
}

int main()
{
    DynamicJsonDocument doc(1536);

    doc["ssid"] = "12345678912345678912";
    doc["pwd"] = "123456";

    JsonArray alarms = doc.createNestedArray("alarms");

    JsonObject alarms_0 = alarms.createNestedObject();
    alarms_0["title"] = "azerrttty";
    alarms_0["min"] = 1;
    alarms_0["hour"] = 2;

    JsonArray alarms_0_dw = alarms_0.createNestedArray("dw");
    alarms_0_dw.add(0);
    alarms_0_dw.add(1);
    alarms_0_dw.add(2);
    alarms_0_dw.add(3);
    alarms_0_dw.add(4);
    alarms_0_dw.add(5);
    alarms_0_dw.add(6);

    JsonArray alarms_0_dm = alarms_0.createNestedArray("dm");
    alarms_0_dm.add(1);
    alarms_0_dm.add(2);
    alarms_0_dm.add(3);
    alarms_0_dm.add(4);
    alarms_0_dm.add(5);
    alarms_0_dm.add(6);
    alarms_0_dm.add(7);
    alarms_0_dm.add(8);
    alarms_0_dm.add(9);
    alarms_0_dm.add(10);
    alarms_0_dm.add(11);
    alarms_0_dm.add(12);
    alarms_0_dm.add(13);
    alarms_0_dm.add(14);
    alarms_0_dm.add(15);
    alarms_0_dm.add(16);
    alarms_0_dm.add(17);
    alarms_0_dm.add(18);
    alarms_0_dm.add(19);
    alarms_0_dm.add(20);
    alarms_0["enabled"] = false;
    alarms_0["once"] = false;
    alarms_0["id"] = 1;
    alarms_0["device"] = 1;
    alarms_0["alarmType"] = 0;

    JsonObject alarms_1 = alarms.createNestedObject();
    alarms_1["title"] = "azerrttty2";
    alarms_1["min"] = 1;
    alarms_1["hour"] = 2;

    JsonArray alarms_1_dw = alarms_1.createNestedArray("dw");
    alarms_1_dw.add(0);
    alarms_1_dw.add(1);
    alarms_1_dw.add(2);
    alarms_1_dw.add(3);
    alarms_1_dw.add(4);
    alarms_1_dw.add(5);
    alarms_1_dw.add(6);

    JsonArray alarms_1_dm = alarms_1.createNestedArray("dm");
    alarms_1_dm.add(1);
    alarms_1_dm.add(2);
    alarms_1_dm.add(3);
    alarms_1_dm.add(4);
    alarms_1_dm.add(5);
    alarms_1_dm.add(6);
    alarms_1_dm.add(7);
    alarms_1_dm.add(8);
    alarms_1_dm.add(9);
    alarms_1_dm.add(10);
    alarms_1_dm.add(11);
    alarms_1_dm.add(12);
    alarms_1_dm.add(13);
    alarms_1_dm.add(14);
    alarms_1_dm.add(15);
    alarms_1_dm.add(16);
    alarms_1_dm.add(17);
    alarms_1_dm.add(18);
    alarms_1_dm.add(19);
    alarms_1_dm.add(20);
    alarms_1["enabled"] = false;
    alarms_1["once"] = false;
    alarms_1["id"] = 1;
    alarms_1["device"] = 1;
    alarms_1["alarmType"] = 0;

    serializeJsonPretty(doc, std::cout);

    return 0;
}
