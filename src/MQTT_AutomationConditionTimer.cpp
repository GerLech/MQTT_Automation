#include <MQTT_AutomationConditionTimer.h>
#include <ArduinoJson.h>

String propertyFormACT = "["
  "{"
  "'name':'name',"
  "'label':'Name',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'hour',"
  "'label':'Stunde',"
  "'type':'number',"
  "'min':0,'max':23,"
  "'default':'0'"
  "},"
  "{"
  "'name':'minute',"
  "'label':'Minute',"
  "'type':'number',"
  "'min':0,'max':59,"
  "'default':'0'"
  "},"
  "{"
  "'name':'repeat',"
  "'label':'Wiederhole',"
  "'type':'number',"
  "'min':0,'max':1440,"
  "'default':'0'"
  "},"
  "{"
  "'name':'period',"
  "'label':'Periode[min]',"
  "'type':'number',"
  "'min':2,'max':720,"
  "'default':'2'"
  "},"
  "{"
  "'name':'duration',"
  "'label':'Dauer[min]',"
  "'type':'number',"
  "'min':0,'max':1440,"
  "'default':'1'"
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


MQTT_AutomationConditionTimer::MQTT_AutomationConditionTimer(const char name[]):MQTT_AutomationCondition(name) {
}

void MQTT_AutomationConditionTimer::update(String data){
  Serial.println(data);
  StaticJsonDocument<300> doc;
  deserializeJson(doc,data);
  uint16_t h = 0;
  uint16_t m = 0;
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],15);
  if (doc.containsKey("hour")) h = doc["hour"];
  if (doc.containsKey("minute")) m = doc["minute"];
  if (doc.containsKey("repeat")) _repeat = doc["repeat"];
  if (doc.containsKey("period")) _period = doc["period"];
  if (doc.containsKey("duration")) _duration = doc["duration"];
  _beginMin = h*60 + m;
  if ((_duration == 0) || (_repeat == 0)) {
    _endMin = 1440;
  } else {
    _endMin = _beginMin + _period * _repeat;
  }
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

String MQTT_AutomationConditionTimer::getForm(){
    return propertyFormACT;
}

String MQTT_AutomationConditionTimer::getProperties(){
  StaticJsonDocument<300> doc;
  char buf[300];
  doc["name"] = _name;
  doc["type"] = AUTO_TYPE_ACT;
  doc["hour"] = _beginMin / 60;
  doc["minute"] = _beginMin % 60;
  doc["repeat"] = _repeat;
  doc["period"] = _period;
  doc["duration"] = _duration;
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
  return String(buf);
}

uint8_t MQTT_AutomationConditionTimer::checkCondition() {
  struct tm ti;
  uint8_t result = AUTO_CONDITION_INVALID;
  if(getLocalTime(&ti)){
    uint16_t minutes = ti.tm_hour * 60 + ti.tm_min;
    uint8_t mask = 1 << ti.tm_wday;
    if (((_weekday & mask) != 0) && (minutes >= _beginMin) && (minutes < _endMin)) {
      //we are in the time window
      uint16_t y = (minutes -_beginMin) / _period;
      uint16_t b = _beginMin+y*_period;
      uint16_t e = b+_duration;
      result = ((minutes >= b) && (minutes <e))?AUTO_CONDITION_TRUE:AUTO_CONDITION_FALSE;
    }
  }
  return result;
}
