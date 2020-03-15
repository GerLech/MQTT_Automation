#ifndef MQTT_AutomationActionAnalog_h
#define MQTT_AutomationActionAnalog_h


#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT_AutomationAction.h>

class MQTT_AutomationActionAnalog : public MQTT_AutomationAction  {
  public:
    MQTT_AutomationActionAnalog(const char name[]);
    void update(String data);
    String getForm();
    String getProperties();
    virtual void updateValue(const char topic [],const char data[]);
    void publish(bool(*onPublish)(const char * topic, const char * message));
  private:
    char _topic[128] = "";
    char _command[20] = "";
    char _sensTopic[128] = "";
    char _identifier[20] = "";
    float _min = 0;
    float _max = 100;
    char _tmp[300];
    char _vtmp[10];
    uint8_t _mathOperator1 = 0;
    float _const1 = 0;
    uint8_t _mathOperator2 = 0;
    float _const2 = 0;
    float _senseValue = 0;
    bool _senseValid = false;

    uint8_t status = 0;
};

#endif
