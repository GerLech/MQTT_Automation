#ifndef MQTT_AutomationActionDimmer_h
#define MQTT_AutomationActionDimmer_h

#define AUTO_DIM_UP 0
#define AUTO_DIM_DOWN 1
#define AUTO_DIM_UP_DOWN 3

#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT_AutomationAction.h>

class MQTT_AutomationActionDimmer : public MQTT_AutomationAction {
  public:
    MQTT_AutomationActionDimmer(const char name[]);
    void update(String data);
    String getForm();
    String getProperties();
    void publish(bool(*onPublish)(const char * topic, const char * message));
  private:
    char _topic[128] = "";
    char _command[20] = "";
    char _tmp[300];
    char _vtmp[10];
    int16_t _min = 0;
    int16_t _max = 100;
    uint8_t _step = 10;
    uint16_t _duration = 5;
    uint8_t _direction = AUTO_DIM_UP_DOWN;

    int16_t _value = 0;
    uint16_t _tim = 0;
    uint8_t _lastStatus = 0;
};

#endif
