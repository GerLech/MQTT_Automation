#ifndef MQTT_AutomationConditionSensorX_h
#define MQTT_AutomationConditionSensorX_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT_AutomationCondition.h>

class MQTT_AutomationConditionSensorX : public MQTT_AutomationCondition {
  public:
    MQTT_AutomationConditionSensorX(const char name[]);
    void update(String data);
    String getForm();
    String getProperties();
    uint8_t checkCondition(uint16_t sunrise, uint16_t sundown);
    void updateValue(const char topic [],const char data[]);
  private:
    char _topic1[128] = "";
    char _identifier1[20] = "";
    char _topic2[128] = "";
    char _identifier2[20] = "";
    uint8_t _compOperator = 0;
    uint8_t _mathOperator = 0;
    float _const = 0;
    float _hysteresis = 2;

    float _value1 = 0;
    float _value2 = 0;
    bool _valid1 = false;
    bool _valid2 = false;
};

#endif
