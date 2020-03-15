#include <MQTT_AutomationActionDigital.h>
#include <ArduinoJson.h>

String propertyFormAAD = "["
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
"'name':'constOn',"
"'label':'Ein',"
"'type':'text',"
"'default':'ON'"
"},"
"{"
"'name':'constOff',"
"'label':'Aus',"
"'type':'text',"
"'default':'OFF'"
"},"
"{"
"'name':'trueOnly',"
"'label':'Nur einschalten',"
"'type':'check',"
"'default':'0'"
"}"
"]";


MQTT_AutomationActionDigital::MQTT_AutomationActionDigital(const char name[]):MQTT_AutomationAction(name) {
}

void MQTT_AutomationActionDigital::update(String data){
  StaticJsonDocument<500> doc;
  deserializeJson(doc,data);
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],15);
  if (doc.containsKey("topic")) strlcpy(_topic,doc["topic"],128);
  if (doc.containsKey("command")) strlcpy(_command,doc["command"],20);
  if (doc.containsKey("constOn")) strlcpy(_constOn,doc["constOn"],10);
  if (doc.containsKey("constOff")) strlcpy(_constOff,doc["constOff"],10);
  if (doc.containsKey("trueOnly")) _trueOnly = (doc["trueOnly"] != 0);
}

String MQTT_AutomationActionDigital::getForm(){
    return propertyFormAAD;
}
String MQTT_AutomationActionDigital::getProperties(){
  StaticJsonDocument<300> doc;
  char buf[300];
  doc["name"] = _name;
  doc["type"] = AUTO_TYPE_AAD;
  doc["topic"] = _topic;
  doc["command"] = _command;
  doc["constOn"] = _constOn;
  doc["constOff"] = _constOff;
  doc["trueOnly"] = _trueOnly?1:0;

  uint16_t n = serializeJson(doc, buf);
  buf[n] = 0;
  return String(buf);
}

void MQTT_AutomationActionDigital::publish(bool(*onPublish)(const char * topic, const char * message)){
  bool res = false;
  if (_toPublish == AUTO_PUBLISH_NONE) return;
  strcpy(_tmp,_topic);
  strcat(_tmp,"/");
  strcat(_tmp,_command);
  if (_toPublish == AUTO_PUBLISH_TRUE) {
    res=onPublish(_tmp,_constOn);
  } else {
    if (!_trueOnly) {
      res=onPublish(_tmp,_constOff);
    } else {
      _toPublish = AUTO_PUBLISH_NONE;
    }
  }
  if (res) _toPublish = AUTO_PUBLISH_NONE;

}
