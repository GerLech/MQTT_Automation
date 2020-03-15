#include <MQTT_AutomationAction.h>

MQTT_AutomationAction::MQTT_AutomationAction(const char name[]) {
  strlcpy (_name,name,15);
}

void MQTT_AutomationAction::setPublishMode(uint8_t mode) {
  _toPublish = mode;
}
