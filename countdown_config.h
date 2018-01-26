#ifndef COUNTDOWN_CONFIG_H
#define COUNTDOWN_CONFIG_H

#include "ArduinoJson.h"

struct  Config {
  Config();
  void  print(const char* msg="");

  void  save(JsonObject&) const;
  void  load(const JsonObject&);

  bool  serialize(Print&) const;
  bool  deserialize(Stream&);
  
  bool  saveFile() const; 
  bool  loadFile();

  char  _ap_name[32];
  char  _ap_password[64];

  char  _msg_start[13];
  char  _msg_end[13];

  int   _duration[5]; // dd,hh,mm,ss,uu
  bool  _periodic_save;
  
};


#endif
