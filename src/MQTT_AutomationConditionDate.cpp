#include <MQTT_AutomationConditionDate.h>
#include <ArduinoJson.h>

String propertyFormACD = "["
  "{"
  "'name':'name',"
  "'label':'Name',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'startday',"
  "'label':'Starttag',"
  "'type':'number',"
  "'min':1,'max':31,"
  "'default':'1'"
  "},"
  "{"
  "'name':'startmonth',"
  "'label':'Startmonat',"
  "'type':'number',"
  "'min':1,'max':12,"
  "'default':'1'"
  "},"
  "{"
  "'name':'endday',"
  "'label':'Endtag',"
  "'type':'number',"
  "'min':1,'max':31,"
  "'default':'1'"
  "},"
  "{"
  "'name':'endmonth',"
  "'label':'Endmonat',"
  "'type':'number',"
  "'min':1,'max':12,"
  "'default':'1'"
  "},"
  "{"
  "'name':'everynday',"
  "'label':'alle n Tage',"
  "'type':'number',"
  "'min':1,'max':365,"
  "'default':'1'"
  "},"
  "{"
  "'name':'days',"
  "'label':'Tage',"
  "'type':'number',"
  "'min':1,'max':365,"
  "'default':'1'"
  "},"
  "{"
  "'name':'everymday',"
  "'label':'monatlich jeden',"
  "'type':'number',"
  "'min':0,'max':32,"
  "'default':'0'"
  "}"
  "]";

uint8_t mdays[] = {31,28,31,30,31,30,31,31,30,31,30,31};

MQTT_AutomationConditionDate::MQTT_AutomationConditionDate(const char name[]):MQTT_AutomationCondition(name) {
}

void MQTT_AutomationConditionDate::update(String data){
  Serial.println(data);
  StaticJsonDocument<300> doc;
  deserializeJson(doc,data);
  if (doc.containsKey("name")) strlcpy(_name,doc["name"],15);
  if (doc.containsKey("startday")) _startDay = doc["startday"];
  if (doc.containsKey("endday")) _endDay = doc["endday"];
  if (doc.containsKey("startmonth")) _startMonth = doc["startmonth"];
  if (doc.containsKey("endmonth")) _endMonth = doc["endmonth"];
  if (doc.containsKey("everynday")) _everyNDay = doc["everynday"];
  if (doc.containsKey("everymday")) _everyMDay = doc["everymday"];
  if (doc.containsKey("days")) _days = doc["days"];
}

String MQTT_AutomationConditionDate::getForm(){
    return propertyFormACD;
}

String MQTT_AutomationConditionDate::getProperties(){
  StaticJsonDocument<300> doc;
  char buf[300];
  doc["name"] = _name;
  doc["type"] = AUTO_TYPE_ACD;
  doc["startday"] = _startDay;
  doc["endday"] = _endDay;
  doc["startmonth"] = _startMonth;
  doc["endmonth"] = _endMonth;
  doc["everynday"] = _everyNDay;
  doc["everymday"] = _everyMDay;
  doc["days"] = _days;
  uint16_t n = serializeJson(doc, buf);
  buf[n] = 0;
  return String(buf);
}

uint8_t MQTT_AutomationConditionDate::checkCondition() {
  struct tm ti;
  uint8_t result = AUTO_CONDITION_FALSE;
  if(getLocalTime(&ti)){
    uint8_t day = 0;
    uint8_t mon = ti.tm_mon+1;
    boolean valid = false;
    Serial.printf("Heute %i.%i Beginn %i.%i Ende %i.%i \n",ti.tm_mday,mon,_startDay,_startMonth,_endDay,_endMonth);
    if (_startMonth < _endMonth) {
      if ((mon > _startMonth) && (mon < _endMonth)) {
        valid = true;
        Serial.println("Monat gültig");
      }
    } else {
      if ((mon < _endMonth) || (mon > _startMonth)) {
        valid = true;
        Serial.println("Monat gültig");
      }
    }
    if ((mon == _startMonth) && (ti.tm_mday >= _startDay) && (ti.tm_mday <= _endDay)) {
      valid = true;
      Serial.println("Starttag gültig");
    }
    if ((mon == _endMonth) &&  (ti.tm_mday >= _startDay) && (ti.tm_mday <= _endDay)) {
      valid = true;
      Serial.println("Endtag gültig");
    }
    if (valid) {
      if (_everyMDay > 0) { //the n-th day in a month should be used
        Serial.println("Tag im Monat");
        if (_everyMDay == 32) { //last day in the month
          uint8_t day = mdays[ti.tm_mon];
          if (leapYear(ti.tm_year)) day++;
          Serial.println("Letzter gültig");
        } else {
          day=_everyMDay;
        }
        if (ti.tm_mday == day) result = AUTO_CONDITION_TRUE;
      } else {
        Serial.println("Tag im Jahr");
        //we use every n-th day
        uint16_t sd = (_startMonth > mon)?dayOfTheYear(ti.tm_year-1,_startMonth,_startDay):dayOfTheYear(ti.tm_year,_startMonth,_startDay);
        int16_t delta = ti.tm_yday - sd;
        if (delta < 0) delta = (leapYear(ti.tm_year-1))?delta+366:delta+365;
        if ((delta % _everyNDay) < _days) result = AUTO_CONDITION_TRUE;
      }
    }
  }
  return result;
}

uint16_t MQTT_AutomationConditionDate::dayOfTheYear(uint16_t y, uint8_t m, uint8_t d) {
  uint16_t res = d;
  if (m < 3) {
    res = (m==2)?d+31:d;
  } else {
    res = d+(153 * m -162)/5;
    if (leapYear(y)) res++;
  }
  return res;
}

boolean MQTT_AutomationConditionDate::leapYear(uint16_t y){
  boolean leapYear = false;
  if ((y % 4) == 0 ) leapYear = true;
  if ((y % 100) == 0 ) leapYear = false;
  if ((y % 400) == 0 ) leapYear = true;
  return leapYear;
}
