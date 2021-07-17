//Version 1.1
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
    void mqttConfig(char cmd[], int16_t ruleId, int16_t elementId, char data[] );
  private:
    void showPage();
    void alignText(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alignment, String text, bool intFont);
    void showRoundedBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text,AUTOSTYLE style);
    void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text, bool withoutBox, AUTOSTYLE style);
    String encodeUnicode(String text, bool intern);
    String getRuleProperties();
    void addAction(uint8_t type, const char name[],AUTO_RULE_STRUCT * rule);
    void addCondition(uint8_t type, const char name[],AUTO_RULE_STRUCT * rule);
    void clearConditions(uint8_t rule);
    void clearActions(uint8_t rule);
    void clearRules();
    void deleteRule(uint8_t index);
    void publishRuleList();
    AUTO_RULE_STRUCT _rules[AUTO_MAX_RULES];
    uint8_t _curPage;
    int8_t _curRule;
    uint8_t _curForm;
    uint8_t _curAction;
    uint8_t _curCondition;
    uint8_t _ruleoffset;
    uint8_t _rulecount;
    uint32_t _lts = 0;
    Adafruit_ILI9341 * _tft;
    TFTForm * _conf;
    const GFXfont * _font;
    void(*_onDone)() = NULL;
    bool(*_onPublish)(const char * topic, const char * message) = NULL;
};
#endif'
