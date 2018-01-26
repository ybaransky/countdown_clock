#ifndef COUNTDOWN_CLOCK
#define COUNTDOWN_CLOCK

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

class CountdownClock {

  struct  SegmentState {
    int16_t _curr = -1;
    int16_t _prev = -1;

    bool    changed()         { return _curr != _prev;}
    bool    sync()            { _prev = _curr;}
    void    value(int16_t v)  { _curr = v; _prev = -1;}
    int16_t value()           { return _curr;}
    void    add(int16_t v)    { _curr += v; }
  };

  struct SegmentDigit {
    Adafruit_7segment*  _segment;
    int                 _index;   // 0,1,3,4
    void                set(Adafruit_7segment* ptr, int index) {_segment=ptr; _index=index;}
  };

  public:
    void  begin(void);
    void      set_time(int*);
    void      get_time(int*);
    void      set_message(const String&);
    String&   get_message(void);

    bool  decrement(uint16_t);
    void  displayClock(void);
    void  displayMessage(void);

    void  clear(void);
    void  writeDisplay(void);

  private:
    Adafruit_7segment   _dddd;
    Adafruit_7segment   _hhmm;
    Adafruit_7segment   _ssuu;
    SegmentDigit        _digits[12];

    SegmentState        _dd;
    SegmentState        _hh;
    SegmentState        _mm;
    SegmentState        _ss;
    SegmentState        _uu;

    String _message; 
};

#endif
