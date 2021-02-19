#ifndef MQTT_AutomationConditionDate_h
#define MQTT_AutomationConditionDate_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT_AutomationCondition.h>

class MQTT_AutomationConditionDate : public MQTT_AutomationCondition {
  public:
    MQTT_AutomationConditionDate(const char name[]);
    void update(String data);
    String getForm();
    String getProperties();
    uint8_t checkCondition();
  private:
    uint16_t dayOfTheYear(uint16_t y, uint8_t m, uint8_t d);
    boolean leapYear(uint16_t y);
    uint8_t _startDay = 1;
    uint8_t _endDay = 1;
    uint8_t _startMonth = 1;
    uint8_t _endMonth = 1;
    uint16_t _everyNDay = 1;
    uint8_t _everyMDay = 0;
    uint16_t _days = 1;
};

#endif
