#ifndef MQTT_AutomationConditionSunTime_h
#define MQTT_AutomationConditionSunTime_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT_AutomationCondition.h>

class MQTT_AutomationConditionSunTime : public MQTT_AutomationCondition {
  public:
    MQTT_AutomationConditionSunTime(const char name[]);
    void update(String data);
    String getForm();
    String getProperties();
    uint8_t checkCondition(uint16_t sunrise, uint16_t sundown);
  private:
    boolean _from = true;
    boolean _rise = true;
    uint16_t _duration = 1;
    int16_t _offset = 0;
    uint8_t _weekday = 0;
};

#endif
