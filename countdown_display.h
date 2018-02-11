#ifndef  COUNTDOWN_DISPLAY
#define COUNTDOWN_DISPLAY

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

class CountdownDisplay {

  enum {DDDD=2, HHMM=1, SSUU=0, N_SEGMENTS=3, N_DIGITS=N_SEGMENTS*4};

  struct  Digit {
    int   _value;
    bool  _visible; 
  };

  struct  Buffer {
    void      fill(int*);
    void      fill_xx(int, int, int, bool);
    void      fill_days(int, int, int, int, int);
    void      clear(void);
    void      print(const char* msg=NULL);

    Digit     _digits[N_DIGITS];
  };

  struct SegmentDigit {
    Adafruit_7segment*  _segment; 
    uint8_t             _index;   // 0,1,3,4  0 is leftmost on the hardware
    void                set(Adafruit_7segment* ptr, uint8_t index) {_segment=ptr; _index=index;}
  };

  public:
    CountdownDisplay();

    void      begin(void);
    void      set_time(int*);
    void      get_time(int*);
    void      set_message(const String&);
    String&   get_message(void);

    void      displayTime(int*);
    void      displayBuffer(Buffer*);
    void      displayMessage(void);
    void      displayBlinkMessage(uint32_t);

    bool      changed(void);
    void      clear(bool write=true);
    void      setBrightness(int);
    void      writeDisplay(void);

  private:

    Adafruit_7segment     _segments[N_SEGMENTS];  // 0 is rightmost (SSUU)
    SegmentDigit          _digits[N_DIGITS];      // this is each invidual addressable digit
    Buffer                _buffers[2];  // 
    int                   _curr_idx;

    String                _message; 
};

extern  CountdownDisplay  gDisplay;
#endif
