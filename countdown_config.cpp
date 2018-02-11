#include <FS.h>
#include "countdown_config.h"

static const char* sFilename      = "/countdown.json";

const int   TIME_DEFAULT[]        = {0,30,30,12,48}; // MS,SEC,MIN,HR,DAYS
const bool  VISIBLE_DEFAULT[]     = {true,true,true,true,true};
const char* MSG_START_DEFAULT     = "4Ur1    CLOC"; // 12 chars total
const char* MSG_END_DEFAULT       = "FUC 4OU";

const int   BRIGHTNESS_DEFAULT    = 15;  // 0-15
const int   DIRECTION_DEFAULT     = -1;  // -1,1   countdown
const bool  PERIODIC_SAVE_DEFAULT = false;
const char* ADDRESS_DEFAULT       = "0-1-2";

const char* AP_NAME_DEFAULT       = "Countdown";
const char* AP_PASSWORD_DEFAULT   = "thereisnospoon";

Config::Config() {
  SPIFFS.begin();

  for(int i=0;i<sizeof(_time)/sizeof(int);i++) {
    _time[i] = TIME_DEFAULT[i];
    _visible[i] = VISIBLE_DEFAULT[i];
  }
  strlcpy(_msg_start,   MSG_START_DEFAULT,   sizeof(_msg_start));
  strlcpy(_msg_end,     MSG_END_DEFAULT,     sizeof(_msg_end));

  _brightness    = BRIGHTNESS_DEFAULT;
  _direction     = DIRECTION_DEFAULT;
  _periodic_save = PERIODIC_SAVE_DEFAULT;
  strlcpy(_address,  ADDRESS_DEFAULT,  sizeof(_address));

  strlcpy(_ap_name,     AP_NAME_DEFAULT,     sizeof(_ap_name));
  strlcpy(_ap_password, AP_PASSWORD_DEFAULT, sizeof(_ap_password));
}

void  Config::print(const char* msg) {
  if (strlen(msg))
    Serial.printf("%s\n",msg);
  serialize(Serial);
  Serial.println("");
}

void  Config::save(JsonObject& obj) const {
  JsonArray& time    = obj.createNestedArray("time");
  JsonArray& visible = obj.createNestedArray("visible");
  for (int i=0;i < sizeof(_time)/sizeof(int);i++) {
    time.add( _time[i] );
    visible.add( _visible[i] );
  }
  obj["msg_start"]     = _msg_start;
  obj["msg_end"]       = _msg_end;

  obj["brightness"]    = _brightness;
  obj["direction"]     = _direction;
  obj["address"]       = _address;
  obj["periodic_save"] = _periodic_save;

  obj["ap_name"]       = _ap_name;
  obj["ap_password"]   = _ap_password;
}

void  Config::load(const JsonObject& obj) {
  int i;
  JsonArray::iterator it;
  JsonArray &time = obj["time"];
  for (i=0, it=time.begin(); it != time.end(); ++it) 
    _time[i++] = it->as<int>();

  JsonArray &visible = obj["visible"];
  for (i=0, it=visible.begin(); it != visible.end(); ++it) 
    _visible[i++] = it->as<bool>();

  strlcpy(_msg_start, obj["msg_start"]   | MSG_START_DEFAULT, sizeof(_msg_start));
  strlcpy(_msg_end,   obj["msg_end"]     | MSG_END_DEFAULT,   sizeof(_msg_end));

  _brightness    = obj["brightness"]    | BRIGHTNESS_DEFAULT;
  _direction     = obj["dirction"]      | DIRECTION_DEFAULT;
  _periodic_save = obj["periodic_save"] | PERIODIC_SAVE_DEFAULT;
  strlcpy(_address,   obj["address"]    | ADDRESS_DEFAULT,   sizeof(_address));

  strlcpy(_ap_name,     obj["ap_name"]     | AP_NAME_DEFAULT, sizeof(_ap_name));
  strlcpy(_ap_password, obj["ap_password"] | AP_NAME_DEFAULT, sizeof(_ap_password));
}

bool  Config::serialize(Print& dst) const {
  DynamicJsonBuffer jb(512);
  JsonObject& root = jb.createObject();
  save(root);
//  root.prettyPrintTo(Serial);
  return root.prettyPrintTo(dst);
}

bool  Config::saveFile() const {
  File  file=SPIFFS.open(sFilename, "w");
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
  File  file=SPIFFS.open(sFilename, "r");
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

void Config::set_time(int* time) {
  memcpy(_time, time, N_ELEMENTS*sizeof(int));
}

void Config::set_visible(bool* visible) {
  memcpy(_visible, visible, N_ELEMENTS*sizeof(bool));
}
