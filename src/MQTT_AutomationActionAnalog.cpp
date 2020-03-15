#include <MQTT_AutomationActionAnalog.h>
#include <ArduinoJson.h>

String propertyFormAAA = "["
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
"'name':'command',"
"'label':'Komando',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'sensTopic',"
"'label':'Sensor Thema',"
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
"'name':'mathOperator1',"
"'label':'Operator 1',"
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
"'name':'const1',"
"'label':'Konstante 1',"
"'type':'float',"
"'default':'0'"
"},"
"{"
"'name':'mathOperator2',"
"'label':'Operator 2',"
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
"'name':'const2',"
"'label':'Konstante 2',"
"'type':'float',"
"'default':'0'"
"},"
"{"
"'name':'min',"
"'label':'Minimalwert',"
"'type':'float',"
"'default':'0'"
"},"
"{"
"'name':'max',"
"'label':'Maximalwert',"
"'type':'float',"
"'default':'100'"
"}"
"]";


MQTT_AutomationActionAnalog::MQTT_AutomationActionAnalog(const char name[]):MQTT_AutomationAction(name) {
}

void MQTT_AutomationActionAnalog::update(String data){
  StaticJsonDocument<500> doc;
  deserializeJson(doc,data);
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],15);
  if (doc.containsKey("topic")) strlcpy(_topic,doc["topic"],128);
  if (doc.containsKey("command")) strlcpy(_command,doc["command"],20);
  if (doc.containsKey("sensTopic")) strlcpy(_sensTopic,doc["sensTopic"],128);
  if (doc.containsKey("identifier")) strlcpy(_identifier,doc["identifier"],20);
  if (doc.containsKey("mathOperator1")) _mathOperator1 = doc["mathOperator1"];
  if (doc.containsKey("const1")) _const1 = doc["const1"];
  if (doc.containsKey("mathOperator2")) _mathOperator2 = doc["mathOperator2"];
  if (doc.containsKey("const2")) _const2 = doc["const2"];
  if (doc.containsKey("min")) _min = doc["min"];
  if (doc.containsKey("max")) _max = doc["max"];
}


String MQTT_AutomationActionAnalog::getForm(){
    return propertyFormAAA;
}
String MQTT_AutomationActionAnalog::getProperties(){
  StaticJsonDocument<300> doc;
  char buf[300];
  doc["name"] = _name;
  doc["type"] = AUTO_TYPE_AAA;
  doc["topic"] = _topic;
  doc["command"] = _command;
  doc["sensTopic"] = _sensTopic;
  doc["identifier"] = _identifier;
  doc["mathOperator1"] = String(_mathOperator1);
  doc["const1"] = _const1;
  doc["mathOperator2"] = String(_mathOperator2);
  doc["const2"] = _const2;
  doc["min"] = _min;
  doc["max"] = _max;
  uint16_t n = serializeJson(doc, buf);
  buf[n] = 0;
  return String(buf);
}

void MQTT_AutomationActionAnalog::updateValue(const char topic [],const char data[]) {
  if (strcmp(_sensTopic,topic) == 0) {
    StaticJsonDocument<200> doc;
    deserializeJson(doc,data);
    if (doc.containsKey(_identifier)) {
      _senseValue = doc[_identifier];
      _senseValid = true;
    }
  }

}

void MQTT_AutomationActionAnalog::publish(bool(*onPublish)(const char * topic, const char * message)){
  bool res = false;
  if (_toPublish == AUTO_PUBLISH_NONE) return;
  strcpy(_tmp,_topic);
  strcat(_tmp,"/");
  strcat(_tmp,_command);
  if (_senseValid){
    float c1 = _senseValue;
    Serial.printf("Sensor: %f\n",c1 );
    switch (_mathOperator1) {
      case AUTO_MATH_PLUS : c1=c1+_const1; break;
      case AUTO_MATH_MINUS : c1=c1-_const1; break;
      case AUTO_MATH_MULT : c1=c1*_const1; break;
      case AUTO_MATH_DIV : c1=c1/_const1; break;
      case AUTO_MATH_PERCENT : c1=c1*_const1/100; break;
    }
    Serial.printf("F1 %f %i \n",c1,_const1);
    switch (_mathOperator2) {
      case AUTO_MATH_PLUS : c1=c1+_const2; break;
      case AUTO_MATH_MINUS : c1=c1-_const2; break;
      case AUTO_MATH_MULT : c1=c1*_const2; break;
      case AUTO_MATH_DIV : c1=c1/_const2; break;
      case AUTO_MATH_PERCENT : c1=c1*_const2/100; break;
    }
    Serial.printf("F1 %f %i \n",c1,_const2);
    if (c1 > _max) c1 = _max;
    if (c1 < _min) c1 = _min;
    Serial.printf("Ausgabewert %.0f\n",c1);
    sprintf(_vtmp,"%.0f",c1);
    if (_toPublish == AUTO_PUBLISH_TRUE) {
      res=onPublish(_tmp,_vtmp);
    } else {
      _toPublish = AUTO_PUBLISH_NONE;
    }
    if (res) _toPublish = AUTO_PUBLISH_NONE;
  }

}
