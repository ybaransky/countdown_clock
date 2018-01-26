#include <FS.h>
#include "countdown_config.h"

static const char* filename       = "/countdown.json";

const char* AP_NAME_DEFAULT       = "YurijCountdown";
const char* AP_PASSWORD_DEFAULT   = "thereisnospoon";
const char* MSG_START_DEFAULT     = "4Ur1    CLOC"; // 12 chars total
const char* MSG_END_DEFAULT       = "FUC 4OU";
const int   DURATION_DEFAULT[]    = {64,12,30,30,0};
const bool  PERIODIC_SAVE_DEFAULT = false;

Config::Config() {
  SPIFFS.begin();
  strlcpy(_ap_name,     AP_NAME_DEFAULT,     sizeof(_ap_name));
  strlcpy(_ap_password, AP_PASSWORD_DEFAULT, sizeof(_ap_password));
  strlcpy(_msg_start,   MSG_START_DEFAULT,   sizeof(_msg_start));
  strlcpy(_msg_end,     MSG_END_DEFAULT,     sizeof(_msg_end));
  for(int i=0;i<sizeof(_duration)/sizeof(int);i++)
    _duration[i] = DURATION_DEFAULT[i];
  _periodic_save = PERIODIC_SAVE_DEFAULT;
}

void  Config::print(const char* msg) {
  if (strlen(msg))
    Serial.printf("%s\n",msg);
  Serial.printf("ap_name     = %s\n",_ap_name);
  Serial.printf("ap_password = %s\n",_ap_password);
  Serial.printf("msg_start   = %s\n",_msg_start);
  Serial.printf("msg_end     = %s\n",_msg_end);
  Serial.printf("duration    = [dd=%d hh=%d mm=%d ss=%d uu%d]\n",
      _duration[0],_duration[1],_duration[2],_duration[3],_duration[4]);
  Serial.printf("periodic    = %b\n",_periodic_save);
}

void  Config::save(JsonObject& obj) const {
  obj["ap_name"]     = _ap_name;
  obj["ap_password"] = _ap_password;
  obj["msg_start"]   = _msg_start;
  obj["msg_end"]     = _msg_end;
  JsonArray& array = obj.createNestedArray("duration");
  for (int i=0;i < sizeof(_duration)/sizeof(int);i++)
    array.add( _duration[i] );
  obj["periodic_save"] = _periodic_save;
}

void  Config::load(const JsonObject& obj) {
  int i=0;
  strlcpy(_ap_name,     obj["ap_name"]     | AP_NAME_DEFAULT, sizeof(_ap_name));
  strlcpy(_ap_password, obj["ap_password"] | AP_NAME_DEFAULT, sizeof(_ap_password));
  strlcpy(_msg_start,   obj["msg_start"]   | MSG_START_DEFAULT, sizeof(_msg_start));
  strlcpy(_msg_end,     obj["msg_end"]     | MSG_END_DEFAULT,   sizeof(_msg_end));
  JsonArray &array = obj["duration"];
  for (JsonArray::iterator it=array.begin(); it != array.end(); ++it) 
    _duration[i++] = it->as<int>();
  _periodic_save = obj["periodic_save"] | PERIODIC_SAVE_DEFAULT;
}

bool  Config::serialize(Print& dst) const {
  DynamicJsonBuffer jb(512);
  JsonObject& root = jb.createObject();
  save(root);
  root.prettyPrintTo(Serial);
  return root.prettyPrintTo(dst);
}

bool  Config::saveFile() const {
  File  file=SPIFFS.open(filename, "w");
  if (!file) {
    Serial.println(F("Failed to create configuration file"));
    return false;
  }
  if (!serialize(file)) {
    Serial.println(F("Failed to serialize configuration"));
    return false;
  }
  return true;
}

bool  Config::deserialize(Stream& src) {
  DynamicJsonBuffer jb(512);
  JsonObject& root = jb.parseObject(src);
  if (!root.success())
    return false;
  root.prettyPrintTo(Serial);
  load(root);
  return true;
}
bool  Config::loadFile() {
  SPIFFS.begin();
  File  file=SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println(F("Failed to open configuration file"));
    return false;
  }
  if (!deserialize(file)) {
    Serial.println(F("Failed to deserialize configuration"));
    return false;
  }
  return true;
}

