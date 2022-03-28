#ifndef MQTT_AutomationConditionTimer_h
#define MQTT_AutomationConditionTimer_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT_AutomationCondition.h>

class MQTT_AutomationConditionTimer : public MQTT_AutomationCondition {
  public:
    MQTT_AutomationConditionTimer(const char name[]);
    void update(String data);
    String getForm();
    String getProperties();
    uint8_t checkCondition(uint16_t sunrise, uint16_t sundown);
  private:
    uint16_t _beginMin = 0;
    uint16_t _endMin = 0;
    uint16_t _repeat = 1;
    uint16_t _period = 0;
    uint16_t _duration = 1;
    uint8_t _weekday = 0;
    uint16_t _day = 0;
};

#endif
