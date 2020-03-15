#include <MQTT_AutomationActionDimmer.h>
#include <ArduinoJson.h>

String propertyFormAADM = "["
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
"'name':'min',"
"'label':'Minimalwert',"
"'type':'number',"
"'default':'0'"
"},"
"{"
"'name':'max',"
"'label':'Maximalwert',"
"'type':'number',"
"'default':'100'"
"},"
"{"
"'name':'step',"
"'label':'Schrittweite',"
"'type':'number',"
"'default':'10'"
"},"
"{"
"'name':'duration',"
"'label':'Schrittdauer (s)',"
"'type':'number',"
"'default':'5'"
"},"
"{"
"'name':'direction',"
"'label':'Richtung',"
"'type':'select',"
"'options':["
"{'l':'auf und ab','v':'"+String(AUTO_DIM_UP_DOWN)+"'},"
"{'l':'nur auf','v':'"+String(AUTO_DIM_UP)+"'},"
"{'l':'nur ab','v':'"+String(AUTO_DIM_DOWN)+"'}"
"]"
"},"
"{"
"'name':'trueOnly',"
"'label':'Nur einschalten',"
"'type':'check',"
"'default':'0'"
"}"
"]";

MQTT_AutomationActionDimmer::MQTT_AutomationActionDimmer(const char name[]):MQTT_AutomationAction(name) {
}

void MQTT_AutomationActionDimmer::update(String data){
  StaticJsonDocument<500> doc;
  deserializeJson(doc,data);
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],15);
  if (doc.containsKey("topic")) strlcpy(_topic,doc["topic"],128);
  if (doc.containsKey("command")) strlcpy(_command,doc["command"],20);
  if (doc.containsKey("min")) _min = doc["min"];
  if (doc.containsKey("max")) _max = doc["max"];
  if (doc.containsKey("step")) _step = doc["step"];
  if (doc.containsKey("duration")) _duration = doc["duration"];
  if (doc.containsKey("direction")) _direction = doc["direction"];
  if (doc.containsKey("trueOnly")) _trueOnly = (doc["trueOnly"] != 0);
}

String MQTT_AutomationActionDimmer::getForm(){
    return propertyFormAADM;
}
String MQTT_AutomationActionDimmer::getProperties(){
  StaticJsonDocument<400> doc;
  char buf[400];
  doc["name"] = _name;
  doc["type"] = AUTO_TYPE_AADM;
  doc["topic"] = _topic;
  doc["command"] = _command;
  doc["min"] = _min;
  doc["max"] = _max;
  doc["step"] = _step;
  doc["duration"] = _duration;
  doc["direction"] = String(_direction);
  doc["trueOnly"] = _trueOnly?1:0;
  uint16_t n = serializeJson(doc, buf);
  buf[n] = 0;
  return String(buf);
}


void MQTT_AutomationActionDimmer::publish(bool(*onPublish)(const char * topic, const char * message)){
  //on start of a new action the value needs to be reset
  if ((_toPublish == AUTO_PUBLISH_TRUE) && (_lastStatus != AUTO_PUBLISH_TRUE) && (_direction != AUTO_DIM_UP_DOWN)) _value = 0;
  if (_tim == 0) {
    _tim = _duration * 10;
    if (_toPublish == AUTO_PUBLISH_NONE) return;
    strcpy(_tmp,_topic);
    strcat(_tmp,"/");
    strcat(_tmp,_command);
    if (_toPublish == AUTO_PUBLISH_TRUE) {
      if ((_direction == AUTO_DIM_UP)||(_direction==AUTO_DIM_UP_DOWN)) {
        _value = _value+_step;
        Serial.printf("+%i\n",_value);
        if (_value >= _max) {
          _value = _max;
          _tim = 0;
          _toPublish = AUTO_PUBLISH_NONE;
        }
      } else {
        //switch on without dimming
        _value = _max;
        _tim = 0;
        _toPublish = AUTO_PUBLISH_NONE;
      }
    } else {
      if (!_trueOnly) {
        if ((_direction == AUTO_DIM_DOWN)||(_direction==AUTO_DIM_UP_DOWN)) {
          _value = _value-_step;
          Serial.printf("-%i\n",_value);
          if (_value <= _min) {
            _value = _min;
            _tim = 0;
            _toPublish = AUTO_PUBLISH_NONE;
          }
        } else {
          //switch off without dimming
          _value = _min;
          _tim = 0;
          _toPublish = AUTO_PUBLISH_NONE;
        }
      } else {
        _toPublish = AUTO_PUBLISH_NONE;
      }

    }
    sprintf(_vtmp,"%i",_value);
    onPublish(_tmp,_vtmp);
  } else {
    _tim --;
  }
  _lastStatus = _toPublish;
}
