#include <MQTT_AutomationConditionSunTime.h>
#include <ArduinoJson.h>

String propertyFormACST = "["
  "{"
  "'name':'name',"
  "'label':'Name',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'from',"
  "'label':'x',"
  "'type':'select',"
  "'options':["
  "{'l':'von','v':'1'},"
  "{'l':'bis','v':'0'}"
  "]"
  "},"
  "{"
  "'name':'rise',"
  "'label':'Sonne',"
  "'type':'select',"
  "'options':["
  "{'l':'Aufgang','v':'1'},"
  "{'l':'Untergang','v':'0'}"
  "]"
  "},"
  "{"
  "'name':'duration',"
  "'label':'Dauer[min]',"
  "'type':'number',"
  "'min':0,'max':1440,"
  "'default':'0'"
  "},"
  "{"
  "'name':'offset',"
  "'label':'Versatz[min]',"
  "'type':'number',"
  "'min':-1440,'max':1440,"
  "'default':'0'"
  "},"
  "{"
  "'name':'day0',"
  "'label':'Sonntag',"
  "'type':'check',"
  "'default':'0'"
  "},"
  "{"
  "'name':'day1',"
  "'label':'Montag',"
  "'type':'check',"
  "'default':'0'"
  "},"
  "{"
  "'name':'day2',"
  "'label':'Dienstag',"
  "'type':'check',"
  "'default':'0'"
  "},"
  "{"
  "'name':'day3',"
  "'label':'Mittwoch',"
  "'type':'check',"
  "'default':'0'"
  "},"
  "{"
  "'name':'day4',"
  "'label':'Donnerstag',"
  "'type':'check',"
  "'default':'0'"
  "},"
  "{"
  "'name':'day5',"
  "'label':'Freitag',"
  "'type':'check',"
  "'default':'0'"
  "},"
  "{"
  "'name':'day6',"
  "'label':'Samstag',"
  "'type':'check',"
  "'default':'0'"
  "}"
  "]";


MQTT_AutomationConditionSunTime::MQTT_AutomationConditionSunTime(const char name[]):MQTT_AutomationCondition(name) {
}

void MQTT_AutomationConditionSunTime::update(String data){
  Serial.println(data);
  StaticJsonDocument<300> doc;
  deserializeJson(doc,data);
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],15);
  if (doc.containsKey("from")) _from = (doc["from"] == "1");
  if (doc.containsKey("rise")) _rise = (doc["rise"] == "1");
  if (doc.containsKey("duration")) _duration = doc["duration"];
  if (doc.containsKey("offset")) _offset = doc["offset"];
  _weekday = 0;
  uint8_t val = 1;
  uint8_t ck = 0;
  char key[6];
  for (uint8_t j=0; j<7; j++){
    sprintf(key,"day%i",j);
    if (doc.containsKey(key)) ck = doc[key];
    Serial.printf("CB %s = %i val = %i, wd = %i\n",key,ck,val,_weekday);
    if (ck!=0) _weekday+=val;
    val = val * 2;
  }
  uint8_t wd = doc["weekday"];
  if (doc.containsKey("weekday")) _weekday = doc["weekday"];
  Serial.println(wd);
}

String MQTT_AutomationConditionSunTime::getForm(){
  Serial.println(propertyFormACST);
    return propertyFormACST;
}

String MQTT_AutomationConditionSunTime::getProperties(){
  StaticJsonDocument<300> doc;
  String x;
  char buf[300];
  doc["name"] = _name;
  doc["type"] = AUTO_TYPE_ACST;
  x = _from?"1":"0";
  doc["from"] = x;
  x = _rise?"1":"0";
  doc["rise"] = x;
  doc["duration"] = _duration;
  doc["offset"] = _offset;
  doc["weekday"] = _weekday;
  uint8_t mask = 1;
  char key[6];
  for (uint8_t j=0; j<7; j++){
    sprintf(key,"day%i",j);
    doc[key] = (mask & _weekday)==0?0:1;
    mask = mask * 2;
  }
  uint16_t n = serializeJson(doc, buf);
  buf[n] = 0;
  Serial.println(buf);
  return String(buf);
}

uint8_t MQTT_AutomationConditionSunTime::checkCondition(uint16_t sunrise, uint16_t sundown) {
  struct tm ti;
  uint8_t result = AUTO_CONDITION_INVALID;
  if(getLocalTime(&ti)){
    uint16_t minutes = ti.tm_hour * 60 + ti.tm_min;
    uint8_t mask = 1 << ti.tm_wday;
    uint16_t dur = _duration;
    if (dur == 0) dur = 24 * 60;
    if ((_weekday & mask) != 0) {
      int16_t onTime = _rise?sunrise:sundown;
      onTime += _offset;
      int16_t offTime = onTime;
      if (_from) offTime += _duration; else onTime -= dur;
      if (onTime < 0) onTime = 0;
      result = ((minutes >= onTime) && (minutes < offTime))?AUTO_CONDITION_TRUE:AUTO_CONDITION_FALSE;
    } else {
      result = AUTO_CONDITION_FALSE;
    }
  }
  return result;
}
