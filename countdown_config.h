#ifndef COUNTDOWN_CONFIG_H
#define COUNTDOWN_CONFIG_H

#include "ArduinoJson.h"

enum  {DAYS=4, HOURS=3, MINUTES=2, SECONDS=1, MILLIS=0, N_ELEMENTS=5}; 

struct  Config {
  Config();
  void  print(const char* msg="");

  void  save(JsonObject&) const;
  void  load(const JsonObject&);

  bool  serialize(Print&) const;
  bool  deserialize(Stream&);
  
  bool  saveFile() const; 
  bool  loadFile();

  void  set_time(int*);
  void  set_visible(bool*);

  int   _time[N_ELEMENTS];    // dd,hh,mm,ss,uu
  bool  _visible[N_ELEMENTS]; // dd,hh,mm,ss,uu
  char  _msg_start[13];
  char  _msg_end[13];

  int   _brightness;
  int   _direction;
  bool  _periodic_save;
  char  _address[6];    // X-Y-Z
  
  char  _ap_name[32];
  char  _ap_password[64];

};

extern  Config  gConfig;

#endif
