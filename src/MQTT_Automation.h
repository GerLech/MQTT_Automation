//Version 1.6
#ifndef MQTT_Automation_h
#define MQTT_Automation_h

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <TFTForm.h>
#include <MQTT_AutomationAction.h>
#include <MQTT_AutomationCondition.h>

#define AUTO_MAX_RULES 30
#define AUTO_MAX_CONDITIONS 10
#define AUTO_MAX_ACTIONS 10

#define AUTO_ALIGNCENTER 0
#define AUTO_ALIGNLEFT 1
#define AUTO_ALIGNRIGHT 2

#define AUTO_PAGE_MAIN 0
#define AUTO_PAGE_RULE 1

#define AUTO_FORM_NONE 0
#define AUTO_FORM_RULE 1
#define AUTO_FORM_CONDITION 2
#define AUTO_FORM_ACTION 3
#define AUTO_FORM_NEW_ENTRY 4

#define AUTO_INPUT_FILENAME "/automation/inputs.json"
#define AUTO_OUTPUT_FILENAME "/automation/outputs.json"
#define AUTO_TIMER_FILENAME "/automation/timers.json"
#define AUTO_RULES_FILENAME "/automation/rules.json"

#define AUTO_RULE_INVALID 0
#define AUTO_RULE_ON 1
#define AUTO_RULE_OFF 2



typedef
struct {
  uint16_t fill;
  uint16_t border;
  uint16_t color;
  uint8_t alignment;
  bool font;
} AUTOSTYLE;

typedef
struct {
  char name[15];
  uint8_t status;
  bool disabled;
  uint8_t conditionCnt;
  uint8_t actionCnt;
  MQTT_AutomationCondition * conditions[AUTO_MAX_CONDITIONS];
  MQTT_AutomationAction * actions[AUTO_MAX_ACTIONS];
} AUTO_RULE_STRUCT;

class MQTT_Automation {
  public:
    MQTT_Automation(Adafruit_ILI9341 * tft, TFTForm * conf, const GFXfont * font);
    void init();
    void setCoordinates(double lon, double lat);
    void showConfig();
    void handleClick(int16_t x, int16_t y);
    void registerOnDone(void (*callback)());
    void registerOnPublish(bool (*callback)(const char * topic, const char * message));
    void endListe();
    void updateTopic(char topic[], const char data[]);
    void refresh();
    void endForm(String data);
    bool saveRules();
    bool readRules();
    bool deleteEntry(uint8_t index);
    int8_t findRule(const char rulename[]);
    String getRuleJSON(const char rulename[]);
    void mqttConfig(char cmd[], int16_t ruleId, int16_t elementId, const char data[]);
    void calculateSun(boolean force);
    uint16_t getSunrise();
    uint16_t getSundown();
  private:
    void showPage();
    double getJulianDate(uint16_t year, uint8_t month, uint8_t day);

    void alignText(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alignment, String text, bool intFont);
    double timeEquation(double &dk, double jDay);
    double inPi(double x);
    double getInclination(double jDay);
    void showRoundedBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text,AUTOSTYLE style);
    void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text, bool withoutBox, AUTOSTYLE style);
    String encodeUnicode(String text, bool intern);
    void addAction(uint8_t type, const char name[],AUTO_RULE_STRUCT * rule);
    void addCondition(uint8_t type, const char name[],AUTO_RULE_STRUCT * rule);
    void clearConditions(uint8_t rule);
    void clearActions(uint8_t rule);
    void clearRules();
    void deleteRule(uint8_t index);
    void publishRuleList();
    void publishRule(const char rulename[]);
    String getRuleProperties(uint8_t index);
    void ruleJSON(AUTO_RULE_STRUCT * r, JsonObject rule);
    void updateRule(AUTO_RULE_STRUCT * rp, JsonObject r);
    void updateRuleConf(int16_t ruleId, const char data[]);
    void deleteRuleConf(const char rulename[]);
    AUTO_RULE_STRUCT _rules[AUTO_MAX_RULES];
    boolean _configActive;
    uint8_t _curPage;
    int8_t _curRule;
    uint8_t _curForm;
    uint8_t _curAction;
    uint8_t _curCondition;
    uint8_t _ruleoffset;
    uint8_t _rulecount;
    uint32_t _lts = 0;
    double _lon = 0; //longitude of the Location
    double _lat = 0; //latidude of ther location
    boolean _hasCoordinates = false;
    double _julianDate = 0; //the julian date for the current sun Values
    uint16_t _sunrise = 0; //minutes for sunrise
    uint16_t _sundown = 0; //minutes for sundown
    Adafruit_ILI9341 * _tft;
    TFTForm * _conf;
    const GFXfont * _font;
    void(*_onDone)() = NULL;
    bool(*_onPublish)(const char * topic, const char * message) = NULL;
    //high precise constants
    const double _pi2 = 6.283185307179586476925286766559;
    const double _pi = 3.1415926535897932384626433832795;
    const double _rad = 0.017453292519943295769236907684886;

};
#endif'
