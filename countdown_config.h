#ifndef COUNTDOWN_CONFIG_H
#define COUNTDOWN_CONFIG_H
#define COUNTDOWN_CONFIG_H

#include "ArduinoJson.h"

enum  {
  N_SEGMENTS=3, DDDD=2, HHMM=1, SSUU=0,
  N_ELEMENTS=5, DAYS=4, HOURS=3, MINUTES=2, SECONDS=1, MILLIS=0,
  N_DIGITS=12,   // 0=right-most
  N_SEGMENT_NAME=5,
  };


struct  Segment {
  int     _seg;
  char    _name[N_SEGMENT_NAME];
  int     _address;
  int     _brightness;
  bool    _visible;

  void    save(JsonObject&) const;
  void    load(const JsonObject&);
  void    print(const char* msg=nullptr) const;
};

struct  Config {
  Config();
  void    save(JsonObject&) const;
  void    load(const JsonObject&);

  bool    serialize(Print&) const;
  bool    deserialize(Stream&);
  
  bool    saveFile() const; 
  bool    loadFile();

  void    print(const char* msg="");
  void    set_time(int*);

  int     _time[N_ELEMENTS];    // uu=0,ss,mm,hh,dd
  int     _display_mode;
  char    _msg_start[N_DIGITS+1];
  char    _msg_end[N_DIGITS+1];

  Segment _segments[N_SEGMENTS]; // DDDD=2, HHMM=1, SSUU=0
  int     _direction;
  bool    _periodic_save;
  
  char    _ap_name[32];
  char    _ap_password[64];

  String  _filename;  // includes path
};

extern  Config  gConfig;

#endif
