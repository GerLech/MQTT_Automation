#ifndef MQTT_AutomationAction_h
#define MQTT_AutomationAction_h


#include <Arduino.h>
#include <ArduinoJson.h>

#define MAX_ACTION_TYPES 3
#define AUTO_TYPE_AAA 0
#define AUTO_TYPE_AAD 1
#define AUTO_TYPE_AADM 2


#define AUTO_MATH_PLUS 0
#define AUTO_MATH_MINUS 1
#define AUTO_MATH_MULT 2
#define AUTO_MATH_DIV 3
#define AUTO_MATH_PERCENT 4

#define AUTO_PUBLISH_NONE 0
#define AUTO_PUBLISH_TRUE 1
#define AUTO_PUBLISH_FALSE 2



class MQTT_AutomationAction {
  protected:
    char _name[15];
    bool _trueOnly = 0;
    uint8_t _toPublish = AUTO_PUBLISH_NONE;
  public:
    MQTT_AutomationAction(const char name[]);
    virtual void update(String data) = 0;
    virtual String getForm() = 0;
    virtual String getProperties() = 0;
    virtual String getName() {return String(_name);};
    virtual void updateValue(const char topic [],const char data[]) {};
    virtual void publish(bool(*onPublish)(const char * topic, const char * message)) {};
    void setPublishMode(uint8_t mode);
};

#endif
