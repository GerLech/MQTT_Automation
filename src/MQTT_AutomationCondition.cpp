#include <MQTT_AutomationCondition.h>

MQTT_AutomationCondition::MQTT_AutomationCondition(const char name[]) {
  strlcpy (_name,name,15);
}
