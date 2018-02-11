#include "countdown_config.h"
#include "countdown_display.h"





uint16_t asci_mask(char c) {
static const uint16_t alphafonttable[] =  {
0b0000000000000001,
0b0000000000000010,
0b0000000000000100,
0b0000000000001000,
0b0000000000010000,
0b0000000000100000,
0b0000000001000000,
0b0000000010000000,
0b0000000100000000,
0b0000001000000000,
0b0000010000000000,
0b0000100000000000,
0b0001000000000000,
0b0010000000000000,
0b0100000000000000,
0b1000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0000000000000000,
0b0001001011001001,
0b0001010111000000,
0b0001001011111001,
0b0000000011100011,
0b0000010100110000,
0b0001001011001000,
0b0011101000000000,
0b0001011100000000,
0b0000000000000000, //  
0b0000000000000110, // !
0b0000001000100000, // "
0b0001001011001110, // #
0b0001001011101101, // $
0b0000110000100100, // %
0b0010001101011101, // &
0b0000010000000000, // '
0b0010010000000000, // (
0b0000100100000000, // )
0b0011111111000000, // *
0b0001001011000000, // +
0b0000100000000000, // ,
0b0000000011000000, // -
0b0000000000000000, // .
0b0000110000000000, // /
0b0000110000111111, // 0
0b0000000000000110, // 1
0b0000000011011011, // 2
0b0000000010001111, // 3
0b0000000011100110, // 4
0b0010000001101001, // 5
0b0000000011111101, // 6
0b0000000000000111, // 7
0b0000000011111111, // 8
0b0000000011101111, // 9
0b0001001000000000, // :
0b0000101000000000, // ;
0b0010010000000000, // <
0b0000000011001000, // =
0b0000100100000000, // >
0b0001000010000011, // ?
0b0000001010111011, // @
0b0000000011110111, // A
0b0001001010001111, // B
0b0000000000111001, // C
0b0001001000001111, // D
0b0000000011111001, // E
0b0000000001110001, // F
0b0000000010111101, // G
0b0000000011110110, // H
0b0001001000000000, // I
0b0000000000011110, // J
0b0010010001110000, // K
0b0000000000111000, // L
0b0000010100110110, // M
0b0010000100110110, // N
0b0000000000111111, // O
0b0000000011110011, // P
0b0010000000111111, // Q
0b0010000011110011, // R
0b0000000011101101, // S
0b0001001000000001, // T
0b0000000000111110, // U
0b0000110000110000, // V
0b0010100000110110, // W
0b0010110100000000, // X
0b0001010100000000, // Y
0b0000110000001001, // Z
0b0000000000111001, // [
0b0010000100000000, // 
0b0000000000001111, // ]
0b0000110000000011, // ^
0b0000000000001000, // _
0b0000000100000000, // `
0b0001000001011000, // a
0b0010000001111000, // b
0b0000000011011000, // c
0b0000100010001110, // d
0b0000100001011000, // e
0b0000000001110001, // f
0b0000010010001110, // g
0b0001000001110000, // h
0b0001000000000000, // i
0b0000000000001110, // j
0b0011011000000000, // k
0b0000000000110000, // l
0b0001000011010100, // m
0b0001000001010000, // n
0b0000000011011100, // o
0b0000000101110000, // p
0b0000010010000110, // q
0b0000000001010000, // r
0b0010000010001000, // s
0b0000000001111000, // t
0b0000000000011100, // u
0b0010000000000100, // v
0b0010100000010100, // w
0b0010100011000000, // x
0b0010000000001100, // y
0b0000100001001000, // z
0b0000100101001001, // {
0b0001001000000000, // |
0b0010010010001001, // }
0b0000010100100000, // ~
0b0011111111111111,
};
  return alphafonttable[uint8_t(c)];
}

static int ctoi(char c) { return c - 48; }

void  CountdownDisplay::Buffer::clear(void) {
  for(int i=0;i<N_DIGITS;i++)
    _digits[i] = {0,false};
}

void  CountdownDisplay::Buffer::fill_days(int lsb, int thousands, int hundreds, int tens, int ones) {
  _digits[lsb+3] = {thousands,bool(thousands)};
  _digits[lsb+2] = {hundreds, bool(thousands) || bool(hundreds)};
  _digits[lsb+1] = {tens,     bool(thousands) || bool(hundreds) || bool(tens)};
  _digits[lsb+0] = {ones, true};
}

void  CountdownDisplay::Buffer::fill_xx(int lsb, int tens, int ones, bool show) {
  _digits[lsb+1] = {tens, show};
  _digits[lsb+0] = {ones, true};
}

void  CountdownDisplay::Buffer::fill(int* time) {
  int   ones,tens,bundreds,thousands;
  clear();
  if (gConfig._visible[MILLIS]) {
    // tenths is always the right most digit
    _digits[0] = {int(time[MILLIS]/100), true};
  }

  if (gConfig._visible[SECONDS]) {
    tens = time[SECONDS]/10;
    ones = time[SECONDS]%10;
    if (gConfig._visible[MILLIS]) {
      // put this in right most segment, next to MILLIS
      fill_xx(1, tens, ones, tens);
    } else {
      // put this in the right most segment, right most
      fill_xx(0, tens, ones, tens);
    }
  }

  if (gConfig._visible[MINUTES]) {
    tens = time[MINUTES]/10;
    ones = time[MINUTES]%10;
    if (gConfig._visible[SECONDS]) {
      // put this in the middle segement, right most
      fill_xx(4, tens, ones, true);
    } else {
      // put this in the right segment, right most
      fill_xx(0, tens, ones, tens);
    }
  }

  if (gConfig._visible[HOURS]) {
    tens = time[HOURS]/10;
    ones = time[HOURS]%10;
    if (gConfig._visible[MINUTES]) {
      if (gConfig._visible[SECONDS]) {
        // put this in the middle segement, but left most
        fill_xx(6, tens, ones, true);
      } else {
        // put thie in the middle segement, but right most
        fill_xx(4, tens, ones, true);
      }
    } else {
      // put this in the middle segement, but right most
      fill_xx(4, tens, ones, tens);
    }
  }
  
  if (gConfig._visible[DAYS]) {
    int thouands = time[DAYS]/1000;
    int hundreds = (time[DAYS]%1000)/100;
    tens = (time[DAYS]%100)/10;
    ones = (time[DAYS]%10);
    // put this in the left segement, always
    fill_days(8,thousands, hundreds, tens, ones);
    /*
    if (gConfig._visible[HOURS]) {
      if (gConfig._visible[MINUTES]) {
        if (gConfig._visible[SECONDS]) {
          // put this in the left segement
          fill_days(8,thousands, hundreds, tens, ones);
        } else {
          // put this in the left segement
          fill_days(8,thousands, hundreds, tens, ones);
        }
      } else {
        // put this in the middle segement
        fill_days(4,thousands, hundreds, tens, ones);
      }
    } else {
      // put this in the right segement
      fill_days(0,thousands, hundreds, tens, ones);
    }
    */
  }
}

void  CountdownDisplay::Buffer::print(const char* msg) {
  if (msg)
    Serial.printf("%s ",msg);

  for(int i=N_DIGITS-1;i>=0; i--) {
    if (_digits[i]._visible)
      Serial.print(_digits[i]._value);
    else
      Serial.print("_");

    if ((i==8) || (i==4))
      Serial.print(" ");

    if (i==6)
      Serial.print(":");

    if ((i==1) && gConfig._visible[MILLIS])
      Serial.print(".");

    if (i==0)
      Serial.println("");
  }
}

////////////////////////////////////////////////////////////////////////
//
CountdownDisplay::CountdownDisplay(void) {
  int i,j;

  for(i=0; i<N_SEGMENTS; i++) {
    _digits[0 + i*4].set(&_segments[i], 4);
    _digits[1 + i*4].set(&_segments[i], 3);
    _digits[2 + i*4].set(&_segments[i], 1);
    _digits[3 + i*4].set(&_segments[i], 0);
  }
  _buffers[0].clear();
  _buffers[1].clear();
  _curr_idx = 0;
}

void CountdownDisplay::begin(void) {
#define WORK_CLOCK_no
#ifdef WORK_CLOCK
  _segments[DDDD].begin(0x72);
  _segments[HHMM].begin(0x70);
  _segments[SSUU].begin(0x71);
#else
  _segments[DDDD].begin(0x72);
  _segments[HHMM].begin(0x71);
  _segments[SSUU].begin(0x70);
#endif
  setBrightness(15);
}

void  CountdownDisplay::displayBuffer(Buffer* buffer) {
  bool  hasDecimal =  gConfig._visible[MILLIS] && 
                      gConfig._visible[SECONDS];
  bool  hasColon   =  gConfig._visible[HOURS]  && 
                      gConfig._visible[MINUTES] && 
                      gConfig._visible[SECONDS];
  for(int i=0;i<N_DIGITS;i++) {
    Adafruit_7segment*  segment = _digits[i]._segment;
    uint8_t             index   = _digits[i]._index;
    uint8_t             value   = buffer->_digits[i]._value;
    bool                visible = buffer->_digits[i]._visible;

    if (visible)
      segment->writeDigitNum(index, value, (i==1) && hasDecimal);
    else
      segment->writeDigitRaw(index, asci_mask(' '));
  }
  _segments[HHMM].drawColon(hasColon);
  writeDisplay();
}

void    CountdownDisplay::displayTime(int* time) {
  // fill in the current buffer
  // the current buffer 
  Buffer* previous = &_buffers[ _curr_idx ? 0 : 1 ];
  previous->fill(time);
  //
  // has this changed from the preious 
  if (changed()) {
    // previous->print("changed: ");
    displayBuffer(previous);
    _curr_idx = _curr_idx ? 0 : 1;
  }
}
  
void    CountdownDisplay::set_message(const String& message) {_message = message;}
String& CountdownDisplay::get_message(void) { return _message;}
bool    CountdownDisplay::changed(void) {
  for(int i=0;i<N_DIGITS;i++)
    if  ( _buffers[0]._digits[i]._value != 
          _buffers[1]._digits[i]._value )
      return true;
  return false;
}

void  CountdownDisplay::clear(bool write) {
  for(int i=0; i<N_SEGMENTS;i++)
    _segments[i].clear();
  if (write)
    writeDisplay();
}

void  CountdownDisplay::writeDisplay(void) {
  for(int i=0; i<N_SEGMENTS;i++)
    _segments[i].writeDisplay();
}

void  CountdownDisplay::setBrightness(int b) {
  static uint8_t prev = 16;
  uint8_t curr;
  if (b <  0) 
    curr = 0;
  else if (b > 15) 
    curr = 15;
  else 
    curr = b;
  if (curr == prev) 
    return;
  prev = curr;
  Serial.printf("setBrightness %d\n",curr);
  for(int i=0; i<N_SEGMENTS;i++)
    _segments[i].setBrightness(curr);
}

void  CountdownDisplay::displayBlinkMessage(uint32_t currtime) {
  static  uint32_t  blinktime = millis();
  static  bool      display = false;
  if (currtime - blinktime > 500) {
    blinktime = currtime;
    display  = !display;
    if (display)
      displayMessage();
    else
      clear();
  }
}

void  CountdownDisplay::displayMessage(void) {
  clear();
  int n = _message.length();
  for(int j=0; j < n; j++) {
    int i = 12-n + j;
    char c = _message.charAt(j);
    if (isDigit(c)) {
      _digits[i]._segment->writeDigitNum( _digits[i]._index, ctoi(c), false);
    }
    else if (isSpace(c)) {
      ; // skip
    }
    else {
      _digits[i]._segment->writeDigitRaw( _digits[i]._index, asci_mask(c));
    }
  }
  Serial.println(_message);
  writeDisplay();
}

