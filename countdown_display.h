#ifndef  COUNTDOWN_DISPLAY
#define COUNTDOWN_DISPLAY

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "countdown_config.h"

class CountdownDisplay {

  struct  Segment {
    struct  Previous {
      void  clear();
      bool  changed(bool);
      bool  changed(int, char, bool);

      int   _brightness;
      char  _char[4];
      bool  _dot[4];
      bool  _colon;
    };
    Adafruit_7segment&  device(void)  {return _device;} 
    Previous&           prev(void)    {return _prev;}
    bool                _colon = false;
    bool                _visible = true;  
    bool                _changed;

    Previous            _prev;
    Adafruit_7segment   _device; 
  };

  public:
    CountdownDisplay();

    void      begin(void);
    void      set_time(int*);
    void      get_time(int*);
    void      set_message(const String&);
    String&   get_message(void);
    void      set_brightness(void);

    void      displayTime(int*);
    void      displayMessage(void);
    void      displayBlinkMessage(void);

    void      clear(bool write=true);
    void      print(const char* msg=NULL, bool text=false);
    void      writeDisplay(bool force=false);

  private:
    void      draw_blank(int);
    void      draw_dddd(int, int, int, int);
    void      draw_hhmm(int, int, int, int, bool);
    void      draw_ssuu(int, int, int, int, int, bool, bool);
    void      draw_c(int, int, char, bool);
    void      draw_x(int, int, int, bool, bool decimal=false);
    void      draw_colon(int, bool);

    Segment   _segments[N_SEGMENTS];  // 0 is rightmost (SSUU)
    int       _imap[4];
    char      _buffer[N_DIGITS+1];    // for diagonstic
    bool      _colons[N_SEGMENTS];   // for diagnostic

    String    _message; 
};

extern  CountdownDisplay  gDisplay;
#endif
