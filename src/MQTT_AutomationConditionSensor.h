#ifndef MQTT_AutomationConditionSensor_h
#define MQTT_AutomationConditionSensor_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT_AutomationCondition.h>

class MQTT_AutomationConditionSensor  : public MQTT_AutomationCondition {
  public:
    MQTT_AutomationConditionSensor(const char name[]);
    void update(String data);
    String getForm();
    String getProperties();
    uint8_t checkCondition(uint16_t sunrise, uint16_t sundown);
    void updateValue(const char topic [],const char data[]);
  private:
    char _topic[128] = "";
    char _identifier[20] = "";
    uint8_t _compOperator = 0;
    float _const = 0;
    float _hysteresis = 2;

    float _value = 0;
    bool _valid = false;
};

#endif
