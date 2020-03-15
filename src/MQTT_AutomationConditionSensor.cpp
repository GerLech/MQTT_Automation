#include <MQTT_AutomationConditionSensor.h>
#include <ArduinoJson.h>

String propertyFormACS = "["
"{"
"'name':'name',"
"'label':'Name',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'topic',"
"'label':'Thema',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'identifier',"
"'label':'Bezeichner',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'compOperator',"
"'label':'Vergleich',"
"'type':'select',"
"'options':["
"{'l':'=','v':'"+String(AUTO_COMP_EQUAL)+"'},"
"{'l':'<','v':'"+String(AUTO_COMP_LESS)+"'},"
"{'l':'>','v':'"+String(AUTO_COMP_MORE)+"'},"
"{'l':'<>','v':'"+String(AUTO_COMP_UNEQUAL)+"'}"
"]"
"},"
"{"
"'name':'const',"
"'label':'Konstante',"
"'type':'float',"
"'default':'0'"
"},"
"{"
"'name':'hysteresis',"
"'label':'Hysterese %',"
"'type':'float',"
"'default':'0'"
"}"
"]";


MQTT_AutomationConditionSensor::MQTT_AutomationConditionSensor(const char name[]):MQTT_AutomationCondition(name) {
}

void MQTT_AutomationConditionSensor::update(String data){
  StaticJsonDocument<500> doc;
  deserializeJson(doc,data);
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],15);
  if (doc.containsKey("topic")) strlcpy(_topic,doc["topic"],128);
  if (doc.containsKey("identifier")) strlcpy(_identifier,doc["identifier"],20);
  if (doc.containsKey("compOperator")) _compOperator = doc["compOperator"];
  if (doc.containsKey("const")) _const = doc["const"];
  if (doc.containsKey("hysteresis")) _hysteresis = doc["hysteresis"];
}

String MQTT_AutomationConditionSensor::getForm(){
    return propertyFormACS;
}
String MQTT_AutomationConditionSensor::getProperties(){
  StaticJsonDocument<300> doc;
  char buf[300];
  doc["name"] = _name;
  doc["type"] = AUTO_TYPE_ACS;
  doc["topic"] = _topic;
  doc["identifier"] = _identifier;
  doc["compOperator"] = String(_compOperator);
  doc["const"] = _const;
  doc["hysteresis"] = _hysteresis;
  uint16_t n = serializeJson(doc, buf);
  buf[n] = 0;
  return String(buf);
}

uint8_t MQTT_AutomationConditionSensor::checkCondition() {
  if (!_valid) {
    return AUTO_CONDITION_INVALID;
  } else {
    uint8_t result = AUTO_CONDITION_FALSE;
    float hys = abs(_const * _hysteresis /100);
    switch (_compOperator) {
      case AUTO_COMP_EQUAL: result = (abs(_value-_const ) < hys)?AUTO_CONDITION_TRUE:AUTO_CONDITION_FALSE; break;
      case AUTO_COMP_LESS: if(_value < (_const -hys)) result = AUTO_CONDITION_TRUE;
        if((_value >= (_const -hys)) && (_value <= (_const +hys))) result = AUTO_CONDITION_INVALID;
        if(_value > (_const +hys)) result = AUTO_CONDITION_FALSE;
        break;
      case AUTO_COMP_MORE: if(_value > (_const +hys)) result = AUTO_CONDITION_TRUE;
        if((_value >= (_const -hys)) && (_value <= (_const +hys))) result = AUTO_CONDITION_INVALID;
        if(_value < (_const -hys)) result = AUTO_CONDITION_FALSE;
        break;
      case AUTO_COMP_UNEQUAL: result = (abs(_value-_const)>hys)?AUTO_CONDITION_TRUE:AUTO_CONDITION_FALSE; break;
    }
    return result;
  }
}
void MQTT_AutomationConditionSensor::updateValue(const char topic [],const char data[]){
    if (strcmp(_topic,topic) == 0) {
      StaticJsonDocument<200> doc;
      deserializeJson(doc,data);
      if (doc.containsKey(_identifier)) {
        _value = doc[_identifier];
        _valid = true;
      }
    }
}
