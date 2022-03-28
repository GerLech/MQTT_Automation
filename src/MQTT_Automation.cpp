//version 1.6
#include <MQTT_Automation.h>
#include <AutomationLayout.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include <MQTT_AutomationActionAnalog.h>
#include <MQTT_AutomationActionDigital.h>
#include <MQTT_AutomationActionDimmer.h>
#include <MQTT_AutomationConditionSensor.h>
#include <MQTT_AutomationConditionSensorX.h>
#include <MQTT_AutomationConditionTimer.h>
#include <MQTT_AutomationConditionDate.h>
#include <MQTT_AutomationConditionSunTime.h>

#include <stdlib.h>
//Formulare

String ruleForm = "["
  "{"
  "'name':'name',"
  "'label':'Name',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'disabled',"
  "'label':'Inaktiv',"
  "'type':'check',"
  "'default':'0'"
  "}"
  "]";


String newEntryForm = "["
"{"
"'name':'name',"
"'label':'Name',"
"'type':'text',"
"'default':''"
"},"
"{"
"'name':'type',"
"'label':'Art',"
"'type':'select',"
"'default':'0',"
"'options':["
"{'v':'"+String(AUTO_TYPE_AAD)+"','l':'Digital-Aktion'},"
"{'v':'"+String(AUTO_TYPE_AAA)+"','l':'Analog-Aktion'},"
"{'v':'"+String(AUTO_TYPE_AADM)+"','l':'Dimmer-Aktion'},"
"{'v':'"+String(AUTO_TYPE_ACT)+"','l':'Timer-Bedingung'},"
"{'v':'"+String(AUTO_TYPE_ACST)+"','l':'Sonnen-Bedingung'},"
"{'v':'"+String(AUTO_TYPE_ACD)+"','l':'Datum-Bedingung'},"
"{'v':'"+String(AUTO_TYPE_ACS)+"','l':'Sensor Bedingung'},"
"{'v':'"+String(AUTO_TYPE_ACSX)+"','l':'erw. Sensor-Bedingung'}"
"]"
"}"
"]";


MQTT_Automation::MQTT_Automation(Adafruit_ILI9341 * tft, TFTForm * conf, const GFXfont * font) {
  _tft = tft;
  _conf = conf;
  _font = font;
}

void MQTT_Automation::init() {
  _rulecount = 0;
  _configActive = false;
  readRules();
}

void MQTT_Automation::setCoordinates(double lon, double lat){
  _lon = lon;
  _lat = lat;
  _hasCoordinates = true;
  calculateSun(true);
}

void MQTT_Automation::showConfig(){
  _curPage = AUTO_PAGE_MAIN;
  _ruleoffset = 0;
  _configActive = true;
  showPage();
}

void MQTT_Automation::handleClick(int16_t x, int16_t y) {
  if (x<0) x=0; if (x>239) x = 239;
  if (y<0) y=0; if (y>319) x = 319;
  uint8_t line = y/20;
  Serial.printf("Zeile %i Position %i Seite %i \n",line,x,_curPage);
  if (_curPage == AUTO_PAGE_MAIN) {
    if (line == 0) {
    } else if (line == 15) {
      uint8_t col = x/60;
      switch (col) {
        case 0: if ((_rulecount > 14) && (_ruleoffset == 0)) _ruleoffset = 14;
          showPage();
          break;
        case 1: if (_onDone) _onDone();
          _configActive = false;
          saveRules();
          break;
        case 2:
          if (_rulecount < AUTO_MAX_RULES) {
            _curForm = AUTO_FORM_RULE;
            _curRule = -1;
            _conf->setDescription(ruleForm);
            _conf->showForm();
          }
          break;
        case 3: if (_ruleoffset > 0) _ruleoffset = 0;
          showPage();
          break;
      }
    } else {
      //rule list line beetween 1 and 14
      uint8_t ix = line-1+_ruleoffset;
      if (ix < _rulecount) {
        _curPage = AUTO_PAGE_RULE;
        _curRule = ix;
        showPage();
      }
    }
  } else {
    //Rule Page
    if (line==0) {
      _conf->setDescription(ruleForm);
      _conf->setValues(getRuleProperties(_curRule));
      _curForm = AUTO_FORM_RULE;
      _conf->showForm();
    } else if (line == 15){
      if (x<80) {
        _curPage = AUTO_PAGE_MAIN;
        showPage();
      } else if (x<160){
        deleteRule(_curRule);
        _curPage = AUTO_PAGE_MAIN;
        showPage();
      } else {
        _conf->setDescription(newEntryForm);
        _curForm = AUTO_FORM_NEW_ENTRY;
        _conf->showForm(false);
      }
    } else if ((line >1) && (line < 15)) {
      if (x<120) {
        if ((line-2) < _rules[_curRule].conditionCnt) {
          _conf->setDescription(_rules[_curRule].conditions[line-2]->getForm());
          _conf->setValues(_rules[_curRule].conditions[line-2]->getProperties());
          _curForm = AUTO_FORM_CONDITION;
          _curCondition = line-2;
          _conf->showForm(true);
        }
      } else {
        if ((line-2) < _rules[_curRule].actionCnt) {
          _conf->setDescription(_rules[_curRule].actions[line-2]->getForm());
          _conf->setValues(_rules[_curRule].actions[line-2]->getProperties());
          _curForm = AUTO_FORM_ACTION;
          _curAction = line-2;
          _conf->showForm(true);
        }
      }
    }
  }
}

void MQTT_Automation::registerOnDone(void (*callback)()){
  _onDone = callback;
}
void MQTT_Automation::registerOnPublish(bool (*callback)(const char * topic, const char * message)){
  _onPublish = callback;
}

void MQTT_Automation::endListe(){
  _curForm = AUTO_FORM_NONE;
  showPage();
}

void MQTT_Automation::endForm(String data) {
  if (data != ""){
    StaticJsonDocument<200> doc;
    deserializeJson(doc, data);
    switch (_curForm) {
      case AUTO_FORM_RULE : if (_curRule < 0) {
          _curRule = _rulecount;
          _rulecount++;
          _rules[_curRule].conditionCnt = 0;
          _rules[_curRule].actionCnt = 0;
          _rules[_curRule].status = AUTO_RULE_INVALID;
          _rules[_curRule].disabled = false;
        };
        if (doc.containsKey("name")) strlcpy(_rules[_curRule].name,doc["name"],15);
        if (doc.containsKey("disabled")) _rules[_curRule].disabled = (doc["disabled"] != 0);
        break;
      case AUTO_FORM_NEW_ENTRY: char name[16];
        if (doc.containsKey("name")) strlcpy(name,doc["name"],15);
        if (doc.containsKey("type")) {
          uint8_t type = doc["type"];
          switch (type) {
            case AUTO_TYPE_AAA:
            case AUTO_TYPE_AAD:
            case AUTO_TYPE_AADM: addAction(type,name,&_rules[_curRule]); break;
            case AUTO_TYPE_ACT:
            case AUTO_TYPE_ACST:
            case AUTO_TYPE_ACD:
            case AUTO_TYPE_ACS:
            case AUTO_TYPE_ACSX: addCondition(type,name,&_rules[_curRule]); break;
          }
        }
        break;
      case AUTO_FORM_CONDITION:
        _rules[_curRule].conditions[_curCondition]->update(data);
        break;
      case AUTO_FORM_ACTION:
        _rules[_curRule].actions[_curAction]->update(data);
        break;
    }
  }
  _curForm = AUTO_FORM_NONE;
  showPage();
}

void MQTT_Automation::updateTopic(char topic[], const char data[]){
  uint8_t j;
  AUTO_RULE_STRUCT * rp;
  char tp[128];
  strlcpy(tp,topic,128);
  char * pch = strrchr(tp,'/');
  if ((pch != NULL) && (strcmp(pch+1,"RESULT")==0)) * pch = 0;
  for (uint8_t i = 0; i<_rulecount; i++){
    rp = &_rules[i];
    for (j=0; j<rp->conditionCnt;j++) {
      rp->conditions[j]->updateValue(tp,data);
    }
    for (j=0; j<rp->actionCnt;j++) {
      rp->actions[j]->updateValue(tp,data);
    }
  }
}

void MQTT_Automation::refresh(){
  if ((millis()-_lts)>100) {
    calculateSun(false); // calculate sunrise and sundown if required
    _lts = millis();
    uint8_t j;
    uint8_t res;
    bool valid;
    bool on;
    AUTO_RULE_STRUCT * rp;
    for (uint8_t i = 0; i<_rulecount; i++){
      rp = &_rules[i];
      if (!rp->disabled) {
        j=0; valid=true; on=true;
        while ((j<rp->conditionCnt) && valid && on){
          res = rp->conditions[j]->checkCondition(_sunrise,_sundown);
          if (res==AUTO_CONDITION_INVALID) valid = false;
          if (res==AUTO_CONDITION_FALSE) on=false;
          j++;
        }
        if (valid) {
          if (on != (rp->status == AUTO_RULE_ON)) {
            for (j=0; j<rp->actionCnt; j++) {
              if (on) {
                rp->status = AUTO_RULE_ON;
                rp->actions[j]->setPublishMode(AUTO_PUBLISH_TRUE);
              }else {
                rp->status = AUTO_RULE_OFF;
                rp->actions[j]->setPublishMode(AUTO_PUBLISH_FALSE);
              }
            }
          }
        } else {
          if (rp->status != AUTO_RULE_INVALID) {
            for (j=0; j<rp->actionCnt; j++) {
              rp->status = AUTO_RULE_INVALID;
              rp->actions[j]->setPublishMode(AUTO_PUBLISH_NONE);
            }
          }
        }
      }
    }
    //execute actions if not already done
    if (_onPublish) {
      for (uint8_t i = 0; i<_rulecount; i++){
        rp = &_rules[i];
        if (!rp->disabled) {
          for (j=0; j<rp->actionCnt; j++) {
            rp->actions[j]->publish(_onPublish);
          }
        }
      }
    }
  }
}


bool MQTT_Automation::saveRules(){
  File f = SPIFFS.open(AUTO_RULES_FILENAME,"w");
  if (f) {
    DynamicJsonDocument doc(32768);
    Serial.println("Start rules");
    for (uint8_t i = 0; i<_rulecount; i++) {
      Serial.printf("rule %i\n",i);
      JsonObject rule = doc.createNestedObject();
      ruleJSON(&_rules[i],rule);
    }
    uint16_t n = serializeJson(doc,f);
    f.close();
    return true;
  } else {
    Serial.println("Cannot write outputs");
    return false;
  }
}

bool MQTT_Automation::readRules() {
  clearRules();
  if (!SPIFFS.exists(AUTO_RULES_FILENAME)) {
    //if configfile does not exist write default values
    saveRules();
  }
  Serial.println("READ");
  File f = SPIFFS.open(AUTO_RULES_FILENAME,"r");
  if (f) {
    AUTO_RULE_STRUCT * rp;
    Serial.println("Read rules");
    uint16_t sz = f.size() * 4;
    if (sz < 1000) sz = 1000;
    Serial.println(sz);
    DynamicJsonDocument doc(sz);
    deserializeJson(doc,f);
    f.close();
    JsonArray rules = doc.as<JsonArray>();
    for (JsonVariant v : rules) {
      JsonObject r = v.as<JsonObject>();
      rp = &_rules[_rulecount];
      _rulecount++;
      updateRule(rp,r);
    }
    return true;
  } else {
    Serial.println("Cannot read inputs");
    return false;
  }

}

void MQTT_Automation::updateRule(AUTO_RULE_STRUCT * rp, JsonObject r) {
  StaticJsonDocument<500> tmp;
  char buf[500];
  if (r.containsKey("name")) strlcpy(rp->name,r["name"],15);
  if (r.containsKey("disabled")) rp->disabled = (r["disabled"] != 0);
  if (r.containsKey("conditions")) {
    JsonArray conds = r["conditions"].as<JsonArray>();
    for (JsonVariant c : conds) {
      JsonObject co = c.as<JsonObject>();
      tmp.clear();
      for (JsonPair kv : co) {
        tmp[kv.key()] = kv.value();
      }
      serializeJson(tmp,buf);
      uint8_t ix = rp->conditionCnt;
      if (tmp.containsKey("type") && (tmp["type"] < MAX_CONDITION_TYPES)) {
        addCondition(tmp["type"],tmp["name"],rp);
        rp->conditions[ix]->update(String(buf));
      }
    }
  }
  if (r.containsKey("actions")) {
    Serial.println("Load Actions");
    JsonArray acts = r["actions"].as<JsonArray>();
    for (JsonVariant a : acts) {
      JsonObject ao = a.as<JsonObject>();
      tmp.clear();
      for (JsonPair kv : ao) {
        tmp[kv.key()] = kv.value();
      }
      serializeJson(tmp,buf);
      uint8_t ix = rp->actionCnt;
      if (tmp.containsKey("type") && (tmp["type"] < MAX_ACTION_TYPES)) {
        addAction(tmp["type"],tmp["name"],rp);
        rp->actions[ix]->update(String(buf));
      }
    }
  }
}

bool MQTT_Automation::deleteEntry(uint8_t index){
  if (_curPage == AUTO_PAGE_RULE) {
    AUTO_RULE_STRUCT * r = &_rules[_curRule];
    if (_curForm == AUTO_FORM_CONDITION) {
      if (_curCondition < r->conditionCnt) {
        delete(r->conditions[_curCondition]);
        r->conditionCnt--;
        for (uint8_t i = _curCondition; i<r->conditionCnt; i++) r->conditions[i] = r->conditions[i+1];
      }
    }
    if (_curForm == AUTO_FORM_ACTION) {
      if (_curAction < r->actionCnt) {
        delete(r->actions[_curAction]);
        r->actionCnt--;
        for (uint8_t i = _curAction; i<r->actionCnt; i++) r->actions[i] = r->actions[i+1];
      }
    }
  }
  _curForm = AUTO_FORM_NONE;
  showPage();

}

void MQTT_Automation::mqttConfig(char cmd[], int16_t ruleId, int16_t elementId, const char data[] ){
  Serial.printf ("Config cmd = %s rule = %i  element = %i  data = %s\n",cmd,ruleId,elementId,data);
  if ((strcmp(cmd,"getrulelist")==0) && _onPublish ) {publishRuleList(); return;}
  if ((strcmp(cmd,"getrule")==0) && _onPublish ) {publishRule(data); return;}
  if (_configActive) {
    if (_onPublish) _onPublish("confs/%s/error","Command not allowed, configuration is active");
  } else {
    if (strcmp(cmd,"updaterule")==0) updateRuleConf(ruleId,data);
    if (strcmp(cmd,"deleterule")==0) deleteRuleConf(data);
  }
}

void MQTT_Automation::updateRuleConf(int16_t ruleId, const char data[]) {
  AUTO_RULE_STRUCT * rp;
  if ((ruleId >= 0) && (ruleId < AUTO_MAX_RULES)) {
    DynamicJsonDocument doc(32768);
    DeserializationError   error = deserializeJson(doc,data);
    if (error ) {
      Serial.println("JSON update rule: ");
      Serial.println(error.c_str());
    }

    JsonObject r = doc.as<JsonObject>();
    int16_t ccnt = -1;
    if (ruleId >= _rulecount) {
      rp = &_rules[_rulecount];
      _rulecount++;
      Serial.println("Rule added!");
    } else {
      rp = &_rules[ruleId];
      clearActions(ruleId);
      clearConditions(ruleId);
    }
    updateRule(rp,r);
    saveRules();
  }
}

void MQTT_Automation::deleteRuleConf(const char rulename[]) {
   int8_t index = findRule(rulename);
   if (index >= 0) {
     clearActions(index);
     clearConditions(index);
     if (index < _rulecount) {
       _rulecount--;
       for (uint8_t i = index; i<_rulecount; i++) _rules[i] = _rules[i+1];
     }
     saveRules();
   }
}

int8_t MQTT_Automation::findRule(const char rulename[])
{
  int8_t i = _rulecount;
  while ((i >= 0) && (strcmp(_rules[i].name,rulename) != 0)) i--;
  return i;
}

String MQTT_Automation::getRuleJSON(const char rulename[]) {
  int8_t i = findRule(rulename);
  if (i < 0) {
    return "";
  } else {
    DynamicJsonDocument doc(32768);
    JsonObject rule = doc.to<JsonObject>();
    ruleJSON(&_rules[i],rule);
    String result = "";
    uint16_t n=serializeJson(doc,result);
    return result;
  }
}

uint16_t MQTT_Automation::getSunrise(){
  return _sunrise;
}

uint16_t MQTT_Automation::getSundown(){
  return _sundown;
}

void MQTT_Automation::calculateSun(boolean force) {
  time_t now;
  tm tl,tu;
  time(&now);
  localtime_r(&now, &tl);
  if (tl.tm_year == 70) return; //no valid date timeEvent
  double jul = getJulianDate(tl.tm_year+1900, tl.tm_mon+1, tl.tm_mday);
  //sunrise and sundown will be calculated if not exists or a new day has started
  if (((jul > _julianDate) || (_sunrise == 0) || force) && _hasCoordinates) {
    _julianDate = jul;
    double jd2000 = 2451545.0;
    gmtime_r(&now, &tu);
    double julDay= (jul - jd2000) / 36525.0;
    double dk;
    double h = -50.0 / 60.0 * _rad;
    double b = _lat * _rad;
    double zz = (tl.tm_hour * 60.0 + tl.tm_min - tu.tm_hour * 60.0 - tu.tm_min) /60.0;
    double te = timeEquation(dk, julDay);
    double timeDiff = 12.0 * acos((sin(h) - sin(b) * sin(dk)) /(cos(b) * cos(dk))) / _pi;
    double riseOT = 12.0 - timeDiff - te;
    double downOT = 12.0 + timeDiff - te;
    double riseWT = riseOT - _lon / 15.0;
    double downWT = downOT - _lon / 15.0;
    double sr = riseWT + zz;
    if (sr < 0.0) sr += 24.0;
    if (sr > 24.0) sr -= 24.0;
    double sd = downWT + zz;
    if (sd < 0.0) sd += 24.0;
    if (sd > 24.0) sd -= 24.0;
    _sunrise = round(sr * 60.0);
    _sundown = round(sd * 60.0);
  }
}


//******************************** private **********************

double MQTT_Automation::getJulianDate(uint16_t year, uint8_t month, uint8_t day){
  int   g;
  if (month <= 2)  {
    month += 12;
    year -= 1;
  }
  g = (year / 400) - (year / 100) + (year / 4); //honor leap years
  return 2400000.5 + 365.0 * year - 679004.0 + g
         + int(30.6001 * (month + 1)) + day + 0.5;
}

double MQTT_Automation::timeEquation(double &dk, double jDay){
  double ra_avg = 18.71506921 + 2400.0513369 * jDay + (2.5862e-5 - 1.72e-9 * jDay) * jDay * jDay;
  double m  = inPi(_pi2 * (0.993133 + 99.997361 * jDay));
  double l  = inPi(_pi2 * (  0.7859453 + m / _pi2 + (6893.0 * sin(m) + 72.0 * sin(2.0 * m) + 6191.2 * jDay) / 1296.0e3));
  double e = getInclination(jDay);
  double ra = atan(tan(l) * cos(e));
  if (ra < 0.0) ra += _pi;
  if (l > _pi) ra += _pi;
  ra = 24.0 * ra / _pi2;
  dk = asin(sin(e) * sin(l));
  // 0<=ra_avg<24
  ra_avg = 24.0 * inPi(_pi2 * ra_avg / 24.0) / _pi2;
  double dra = ra_avg - ra;
  if (dra < -12.0) dra += 24.0;
  if (dra > 12.0) dra -= 24.0;
  dra = dra * 1.0027379;
  return dra ;
}

double MQTT_Automation::inPi(double x){
  int n = (int)(x / _pi2);
  x = x - n * _pi2;
  if (x < 0) x += _pi2;
  return x;
}

double MQTT_Automation::getInclination(double jDay){
  return _rad * (23.43929111 + (-46.8150 * jDay - 0.00059 * jDay * jDay + 0.001813 * jDay * jDay * jDay) / 3600.0);
}


void MQTT_Automation::ruleJSON(AUTO_RULE_STRUCT * r, JsonObject rule) {
  StaticJsonDocument<300> tmp;
  rule["name"] = r->name;
  rule["disabled"] = r->disabled?1:0;
  rule["conditionCnt"] = r->conditionCnt;
  rule["actionCnt"] = r->actionCnt;
  r->status = AUTO_RULE_INVALID;
  JsonArray conds = rule.createNestedArray("conditions");
  Serial.println("conditions");
  for (uint8_t j = 0; j<r->conditionCnt; j++) {
    deserializeJson(tmp,r->conditions[j]->getProperties());
    Serial.printf("condition %i\n",j);
    JsonObject tmpo = tmp.as<JsonObject>();
    JsonObject c = conds.createNestedObject();
    for (JsonPair kv : tmpo){
      c[kv.key()] = kv.value();
    }
  }
  JsonArray actions = rule.createNestedArray("actions");
  Serial.println("actions");
  for (uint8_t j = 0; j<r->actionCnt; j++) {
    deserializeJson(tmp,r->actions[j]->getProperties());
    Serial.printf("action %i\n",j);
    JsonObject tmpo = tmp.as<JsonObject>();
    JsonObject c = actions.createNestedObject();
    for (JsonPair kv : tmpo){
      c[kv.key()] = kv.value();
    }
  }
}

void MQTT_Automation::publishRule(const char rulename[]) {
   String tmp = getRuleJSON(rulename);
   if (tmp != "") _onPublish("confs/%s/rule",tmp.c_str());
}

void MQTT_Automation::publishRuleList() {
  char buf[1000];
  DynamicJsonDocument doc(1000);
  JsonArray array = doc.to<JsonArray>();
  for (uint8_t i = 0; i < _rulecount; i++) array.add(_rules[i].name);
  serializeJson(doc,buf);
  Serial.println(buf);
  _onPublish("confs/%s/rulelist",buf);
}

void MQTT_Automation::showPage() {
  _tft->fillScreen(ILI9341_WHITE);
  if (_curPage == AUTO_PAGE_MAIN) {
    showBox(0,0,240,20,"REGELN",false,auto_styles[AUTOSTYLETOPBAR]);
    _tft->fillTriangle(15,304,45,304,30,316,ILI9341_BLUE);
    _tft->fillTriangle(195,316,225,316,210,304,ILI9341_BLUE);
    showRoundedBox(62,301,56,18,"Fertig",auto_styles[AUTOSTYLEBUTTON]);
    showRoundedBox(122,301,56,18,"Neu",auto_styles[AUTOSTYLEBUTTON]);
    String label;
    uint8_t i = 0;
    uint8_t ix =  i+_ruleoffset;
    do {
      ix = i+_ruleoffset;
      if (ix < _rulecount) {
        label = _rules[ix].name;
        showBox(0,i*20+20,240,20,label,false,auto_styles[AUTOSTYLEOPTIONS]);
        if (i<13) _tft->drawLine(0,i*20+39,240,i*20+39,ILI9341_BLACK);
      }
      i++;
    } while ((ix<_rulecount) && (i < 14));
  } else {
    showBox(0,0,240,20,_rules[_curRule].name,false,auto_styles[AUTOSTYLETOPBAR]);
    showBox(0,20,120,20,"Bedingungen",false,auto_styles[AUTOSTYLESUBBAR]);
    showBox(120,20,120,20,"Aktionen",false,auto_styles[AUTOSTYLESUBBAR]);
    showRoundedBox(2,301,76,18,"Fertig",auto_styles[AUTOSTYLEBUTTON]);
    showRoundedBox(82,301,76,18,"Löschen",auto_styles[AUTOSTYLEBUTTON]);
    showRoundedBox(162,301,76,18,"Neu",auto_styles[AUTOSTYLEBUTTON]);
    _tft->drawLine(120,40,120,300,ILI9341_BLUE);
    String label;
    for (uint8_t i = 0; i<_rules[_curRule].conditionCnt;i++) {
      label = _rules[_curRule].conditions[i]->getName();
      showBox(0,i*20+40,120,20,label,false,auto_styles[AUTOSTYLEOPTIONS]);
      _tft->drawLine(0,i*20+59,120,i*20+59,ILI9341_BLACK);
    }
    for (uint8_t i = 0; i<_rules[_curRule].actionCnt;i++) {
      label = _rules[_curRule].actions[i]->getName();
      showBox(121,i*20+40,119,20,label,false,auto_styles[AUTOSTYLEOPTIONS]);
      _tft->drawLine(121,i*20+59,239,i*20+59,ILI9341_BLACK);
    }
  }
}

void MQTT_Automation::alignText(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alignment, String text, bool intFont) {
  uint16_t w1,h1,xx;
  int16_t x1,y1,hx;
  _tft->getTextBounds(text,x,y,&x1,&y1,&w1,&h1);
  hx = y-y1;
  xx = 0;
  switch (alignment) {
    case EDT_ALIGNCENTER : xx = x+(w-w1)/2; break;
    case EDT_ALIGNLEFT : xx = x+4; break;
    case EDT_ALIGNRIGHT : xx = x + w - w1-4;
  }
  if (intFont) {
    if (h > 0) {
      _tft->setCursor(xx,y+(h-8)/2);
    } else {
      _tft->setCursor(xx,y);
    }
  } else {
    if (h > 0) {
      _tft->setCursor(xx,y+(h-hx)/2+hx);
    } else {
      _tft->setCursor(xx,y);
    }
  }
  _tft->print(text);
}


//show a rounded box with centered text with given style
void MQTT_Automation::showRoundedBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text,AUTOSTYLE style){
  if (style.font) _tft->setFont(_font); else _tft->setFont(NULL);
  _tft->setTextColor(style.color);
  _tft->fillRoundRect(x,y,w,h,6,style.fill);
  _tft->drawRoundRect(x,y,w,h,6,style.border);
  _tft->drawRoundRect(x+1,y+1,w-2,h-2,5,style.border);
  alignText(x,y,w,h,style.alignment,encodeUnicode(text,style.font==NULL),(style.font==NULL));
}

void MQTT_Automation::showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text, bool withoutBox, AUTOSTYLE style){
  if (style.font) _tft->setFont(_font); else _tft->setFont(NULL);
  _tft->setTextColor(style.color);
  if (!withoutBox) {
    _tft->fillRect(x,y,w,h,style.fill);
    _tft->drawRect(x,y,w,h,style.border);
  }
  alignText(x,y,w,h,style.alignment,encodeUnicode(text,style.font==NULL),(style.font==NULL));
}

//encode extra character from unicode for display
String MQTT_Automation::encodeUnicode(String text, bool intern){
  String res = "";
  uint8_t i = 0;
  char c;
  while (i<text.length()) {
    c=text[i];
    if (c==195) { //UTF8 characters German Umlaute
      i++;
      if (intern) {
        switch (text[i]) {
          case 164: c=132; break; //ä
          case 182: c=148; break; //ö
          case 188: c=129; break; //ü
          case 159: c=224; break; //ß
          case 132: c=142; break; //Ä
          case 150: c=153; break; //Ö
          case 156: c=154; break; //Ü
          default: c=0;
        }
      } else {
        switch (text[i]) {
          case 164: c=130; break; //ä
          case 182: c=131; break; //ö
          case 188: c=132; break; //ü
          case 159: c=133; break; //ß
          case 132: c=127; break; //Ä
          case 150: c=128; break; //Ö
          case 156: c=129; break; //Ü
          default: c=0;
        }
      }
    } else if (c == 194) { //UTF8 char for Degrees symbol
      i++;
      if (_font == NULL){
        if (text[i] == 176) c=247; else c=0;
      } else {
        if (text[i] == 176) c=134; else c=0;
      }
    } else if (c > 128) { //normal characters unchanged
      c=0;
    }
    if (c>0) res.concat(c);
    i++;
  }
  return res;
}

String MQTT_Automation::getRuleProperties(uint8_t index){
  StaticJsonDocument<200> doc;
  doc["name"]=_rules[index].name;
  doc["disabled"]=_rules[index].disabled?1:0;
  char buf[100];
  uint16_t n=serializeJson(doc,buf);
  buf[n]=0;
  return String(buf);
}


void MQTT_Automation::addAction(uint8_t type, const char name[],AUTO_RULE_STRUCT * rule){
  if (rule->actionCnt < AUTO_MAX_ACTIONS) {
    uint8_t ix = rule->actionCnt;
    rule->actionCnt++;
    switch (type) {
      case AUTO_TYPE_AAA: rule->actions[ix]=new MQTT_AutomationActionAnalog(name);
        break;
      case AUTO_TYPE_AAD: rule->actions[ix]=new MQTT_AutomationActionDigital(name);
        break;
      case AUTO_TYPE_AADM: rule->actions[ix]=new MQTT_AutomationActionDimmer(name);
        break;
      default: rule->actionCnt--;
    }
  }
}
void MQTT_Automation::addCondition(uint8_t type, const char name[],AUTO_RULE_STRUCT * rule){
  if (rule->conditionCnt < AUTO_MAX_CONDITIONS) {
    uint8_t ix = rule->conditionCnt;
    rule->conditionCnt++;
    Serial.printf("Add type %i with name %s\n",type,name);
    switch (type) {
      case AUTO_TYPE_ACS: rule->conditions[ix] = new MQTT_AutomationConditionSensor(name);
        break;
      case AUTO_TYPE_ACSX: rule->conditions[ix]=new MQTT_AutomationConditionSensorX(name);
        break;
      case AUTO_TYPE_ACT: rule->conditions[ix]=new MQTT_AutomationConditionTimer(name);
        break;
      case AUTO_TYPE_ACD: rule->conditions[ix]=new MQTT_AutomationConditionDate(name);
        break;
      case AUTO_TYPE_ACST: rule->conditions[ix]=new MQTT_AutomationConditionSunTime(name);
        break;
      default: rule->conditionCnt--;
    }
  }
}

void MQTT_Automation::clearConditions(uint8_t rule){
  for (uint8_t i = 0; i<_rules[rule].conditionCnt; i++) {
    delete(_rules[rule].conditions[i]);
  }
  _rules[rule].conditionCnt = 0;
}

void MQTT_Automation::clearActions(uint8_t rule){
  for (uint8_t i = 0; i<_rules[rule].actionCnt; i++) {
    delete(_rules[rule].actions[i]);
  }
  _rules[rule].actionCnt = 0;
}

void MQTT_Automation::clearRules() {
  for (uint8_t i = 0; i<_rulecount;i++){
    clearActions(i);
    clearConditions(i);
    _rulecount = 0;
  }
}


void MQTT_Automation::deleteRule(uint8_t index){
  if (index < _rulecount) {
    clearActions(index);
    clearConditions(index);
    _rulecount--;
    for (uint8_t i = index; i<_rulecount; i++) _rules[i] = _rules[i+1];
  }
  showPage();
}
