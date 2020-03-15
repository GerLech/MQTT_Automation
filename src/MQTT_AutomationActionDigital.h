#ifndef MQTT_AutomationActionDigital_h
#define MQTT_AutomationActionDigital_h


#include <Arduino.h>
#include <ArduinoJson.h>
#include <MQTT_AutomationAction.h>

class MQTT_AutomationActionDigital  : public MQTT_AutomationAction  {
  public:
    MQTT_AutomationActionDigital(const char name[]);
    void update(String data);
    String getForm();
    String getProperties();
    void publish(bool(*onPublish)(const char * topic, const char * message));
  private:
    char _topic[128] = "";
    char _command[20] = "";
    char _constOn[25] = "ON";
    char _constOff[25] = "OFF";
    char _tmp[300];

    uint8_t status = 0;
};

#endif
