#include <FS.h>
#include "countdown_config.h"

static  const char* sFilename     = "/countdown.json";

static  const int     TIME_DEFAULT[]     = {0,30,30,12,48}; // MS,SEC,MIN,HR,DAYS
static  const int     DISPLAY_MODE       = 0;
static  const char*   MSG_START_DEFAULT  = "Yuricloc"; // 12 chars total
static  const char*   MSG_END_DEFAULT    = "Fuc You";

static  const Segment SEGMENT_DEFAULT[]  = { 
  {SSUU, "SSUU", 0, 15, true}, 
  {HHMM, "HHHM", 1, 15, true}, 
  {DDDD, "DDDD", 2, 15, true} 
};
static  const int     DIRECTION_DEFAULT     = -1;  // -1,1   countdown
static  const bool    PERIODIC_SAVE_DEFAULT = true;

static  const char*   AP_NAME_DEFAULT       = "Countdown";
static  const char*   AP_PASSWORD_DEFAULT   = "thereisnospoon";

/*
 * ***********************************************************************
 */

void  Segment::print(const char* msg) const {
  if (msg)
    Serial.printf("%s\n",msg);
  Serial.printf(" seg: %d\n", _seg);
  Serial.printf("name: %4s\n", _name);
  Serial.printf("addr: %d\n", _address);
  Serial.printf("brit: %d\n", _brightness);
  Serial.printf("show: %d\n", _visible);
}

void  Segment::save(JsonObject& obj) const {
  obj["name"]       = _name;
  obj["address"]    = _address;
  obj["brightness"] = _brightness;
  obj["visible"]    = _visible;
}

void  Segment::load(const JsonObject& obj) {
  strlcpy(_name, obj["name"]      | SEGMENT_DEFAULT[_seg]._name, N_SEGMENT_NAME);
  _address    = obj["address"]    | SEGMENT_DEFAULT[_seg]._address; 
  _brightness = obj["brightness"] | SEGMENT_DEFAULT[_seg]._brightness;
  _visible    = obj["visible"]    | SEGMENT_DEFAULT[_seg]._visible;
}

/*
 * ***********************************************************************
 */

Config::Config() : _filename(sFilename) {
  SPIFFS.begin();

  for(int i=0;i<sizeof(_time)/sizeof(int);i++) {
    _time[i] = TIME_DEFAULT[i];
  }
  _display_mode = DISPLAY_MODE;

  strlcpy(_msg_start,   MSG_START_DEFAULT,   sizeof(_msg_start));
  strlcpy(_msg_end,     MSG_END_DEFAULT,     sizeof(_msg_end));

  _segments[DDDD] = SEGMENT_DEFAULT[DDDD];
  _segments[HHMM] = SEGMENT_DEFAULT[HHMM];
  _segments[SSUU] = SEGMENT_DEFAULT[SSUU];

  _direction      = DIRECTION_DEFAULT;
  _periodic_save  = PERIODIC_SAVE_DEFAULT;

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
  for (int i=0;i < N_ELEMENTS; i++) {
    time.add( _time[i] );
  }
  obj["display_mode"]  = _display_mode;
  obj["msg_start"]     = _msg_start;
  obj["msg_end"]       = _msg_end;

  JsonArray& segments = obj.createNestedArray("segments");
  for (int i=0;i < N_SEGMENTS;i++) {
    _segments[i].save(segments.createNestedObject());
  }

  obj["direction"]     = _direction;
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
  _display_mode = obj["display_mode"] | DISPLAY_MODE;

  strlcpy(_msg_start, obj["msg_start"]   | MSG_START_DEFAULT, sizeof(_msg_start));
  strlcpy(_msg_end,   obj["msg_end"]     | MSG_END_DEFAULT,   sizeof(_msg_end));

  JsonArray &segments = obj["segments"];
  for (i=0, it=segments.begin(); it != segments.end(); ++it) {
    _segments[i++].load(*it);
  }

  _direction     = obj["dirction"]      | DIRECTION_DEFAULT;
  _periodic_save = obj["periodic_save"] | PERIODIC_SAVE_DEFAULT;

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
