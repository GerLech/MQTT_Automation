#ifndef MQTT_AutomationCondition_h
#define MQTT_AutomationCondition_h

#include <Arduino.h>
#include <ArduinoJson.h>

#define MAX_CONDITION_TYPES 13
#define AUTO_TYPE_ACS 10
#define AUTO_TYPE_ACSX 11
#define AUTO_TYPE_ACT 12


#define AUTO_COMP_EQUAL 0
#define AUTO_COMP_LESS 1
#define AUTO_COMP_MORE 2
#define AUTO_COMP_UNEQUAL 3

#define AUTO_MATH_PLUS 0
#define AUTO_MATH_MINUS 1
#define AUTO_MATH_MULT 2
#define AUTO_MATH_DIV 3
#define AUTO_MATH_PERCENT 4

#define AUTO_CONDITION_INVALID 0
#define AUTO_CONDITION_FALSE 1
#define AUTO_CONDITION_TRUE 2

class MQTT_AutomationCondition {
  protected:
    char _name[15];
  public:
    MQTT_AutomationCondition(const char name[]);
    virtual void update(String data) = 0;
    virtual String getForm() = 0;
    virtual String getProperties() = 0;
    virtual String getName() {return String(_name);};
    virtual uint8_t checkCondition() {return AUTO_CONDITION_INVALID;};
    virtual void updateValue(const char topic [],const char data[]) {};
};

#endif
