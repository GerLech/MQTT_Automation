#include <MQTT_AutomationConditionSensorX.h>
#include <ArduinoJson.h>

String propertyFormACSX = "["
"{"
"'name':'name',"
"'label':'Name',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'topic1',"
"'label':'Thema1',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'identifier1',"
"'label':'Bezeichner1',"
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
"'name':'topic2',"
"'label':'Thema2',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'identifier2',"
"'label':'Bezeichner2',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'mathOperator',"
"'label':'Operator',"
"'type':'select',"
"'options':["
"{'l':'+','v':'"+String(AUTO_MATH_PLUS)+"'},"
"{'l':'-','v':'"+String(AUTO_MATH_MINUS)+"'},"
"{'l':'*','v':'"+String(AUTO_MATH_MULT)+"'},"
"{'l':'/','v':'"+String(AUTO_MATH_DIV)+"'},"
"{'l':'%','v':'"+String(AUTO_MATH_PERCENT)+"'}"
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


MQTT_AutomationConditionSensorX::MQTT_AutomationConditionSensorX(const char name[]):MQTT_AutomationCondition(name) {
}

void MQTT_AutomationConditionSensorX::update(String data){
  StaticJsonDocument<500> doc;
  deserializeJson(doc,data);
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],15);
  if (doc.containsKey("topic1")) strlcpy(_topic1,doc["topic1"],128);
  if (doc.containsKey("identifier1")) strlcpy(_identifier1,doc["identifier1"],20);
  if (doc.containsKey("topic2")) strlcpy(_topic2,doc["topic2"],128);
  if (doc.containsKey("identifier2")) strlcpy(_identifier2,doc["identifier2"],20);
  if (doc.containsKey("compOperator")) _compOperator = doc["compOperator"];
  if (doc.containsKey("mathOperator")) _mathOperator = doc["mathOperator"];
  if (doc.containsKey("const")) _const = doc["const"];
  if (doc.containsKey("hysteresis")) _hysteresis = doc["hysteresis"];
}

String MQTT_AutomationConditionSensorX::getForm(){
    return propertyFormACSX;
}
String MQTT_AutomationConditionSensorX::getProperties(){
  StaticJsonDocument<300> doc;
  char buf[300];
  doc["name"] = _name;
  doc["type"] = AUTO_TYPE_ACSX;
  doc["topic1"] = _topic1;
  doc["identifier1"] = _identifier1;
  doc["topic2"] = _topic2;
  doc["identifier2"] = _identifier2;
  doc["compOperator"] = String(_compOperator);
  doc["mathOperator"] = String(_mathOperator);
  doc["const"] = _const;
  doc["hysteresis"] = _hysteresis;
  uint16_t n = serializeJson(doc, buf);
  buf[n] = 0;
  return String(buf);
}

uint8_t MQTT_AutomationConditionSensorX::checkCondition(uint16_t sunrise, uint16_t sundown) {
  if (_valid1 && _valid2) {
    float c1 = _value2;
    switch (_mathOperator) {
      case AUTO_MATH_PLUS : c1=_value2+_const; break;
      case AUTO_MATH_MINUS : c1=_value2-_const; break;
      case AUTO_MATH_MULT : c1=_value2*_const; break;
      case AUTO_MATH_DIV : c1=_value2/_const; break;
      case AUTO_MATH_PERCENT : c1=_value2*_const/100; break;
    }
    bool result = true;
    float hys = abs(_value1 * _hysteresis /100);
    switch (_compOperator) {
      case AUTO_COMP_EQUAL: result = (abs(_value1-c1 ) < hys)?AUTO_CONDITION_TRUE:AUTO_CONDITION_FALSE; break;
      case AUTO_COMP_LESS: if(_value1 < (c1 -hys)) result = AUTO_CONDITION_TRUE;
        if((_value1 >= (c1 -hys)) && (_value1 <= (c1 +hys))) result = AUTO_CONDITION_INVALID;
        if(_value1 > (c1 +hys)) result = AUTO_CONDITION_FALSE;
        break;
      case AUTO_COMP_MORE: if(_value1 > (c1 +hys)) result = AUTO_CONDITION_TRUE;
        if((_value1 >= (c1 -hys)) && (_value1 <= (c1 +hys))) result = AUTO_CONDITION_INVALID;
        if(_value1 < (c1 -hys)) result = AUTO_CONDITION_FALSE;
        break;
      case AUTO_COMP_UNEQUAL: result = (abs(_value1-c1)>hys)?AUTO_CONDITION_TRUE:AUTO_CONDITION_FALSE; break;
    }
    return result;
  } else {
    return AUTO_CONDITION_INVALID;
  }
}
void MQTT_AutomationConditionSensorX::updateValue(const char topic [],const char data[]){
  if (strcmp(_topic1,topic)==0){
    StaticJsonDocument<200> doc;
    deserializeJson(doc,data);
    if (doc.containsKey(_identifier1)) {
      _value1 = doc[_identifier1];
      _valid1 = true;
    }
  }
  if (strcmp(_topic2,topic)==0){
    StaticJsonDocument<200> doc;
    deserializeJson(doc,data);
    if (doc.containsKey(_identifier2)) {
      _value2 = doc[_identifier2];
      _valid2 = true;
    }
  }
}
