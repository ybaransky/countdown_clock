#include <Arduino.h>
#include "countdown_clock.h"


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



void CountdownClock::begin(void) {
  _dddd.begin(0x70);
  _hhmm.begin(0x71);
  _ssuu.begin(0x72);
}

void CountdownClock::set_time(int* time) {
  _dd.value( time[0] );
  _hh.value( time[1] );
  _mm.value( time[2] );
  _ss.value( time[3] );
  _uu.value( time[4] );
  clear();
}

void CountdownClock::get_time(int* time) {
  time[0] = _dd.value();
  time[1] = _hh.value();
  time[2] = _mm.value();
  time[3] = _ss.value();
  time[4] = _uu.value();
}

bool CountdownClock::decrement(uint16_t decrement_amount) {
  // are we at zero already so we can't decrement
  if (_dd.value() | _hh.value() | _mm.value() | _ss.value() | _uu.value() ) {
    _uu.add(-decrement_amount);
    if (_uu.value() < 0) {
      _uu.add(1000);
      _ss.add(-1);
      if (_ss.value() < 0) {
        _ss.add(60);
        _mm.add(-1);
        if (_mm.value() < 0) {
          _mm.add(60);
          _hh.add(-1);
          if (_hh.value() < 0) {
            _hh.add(24);
            if (_dd.value() == 0) {
              // we can out of carry, set to zero
              int time[] = {0,0,0,0,0};
              set_time(time);
              return false;
            }
            else
              _dd.add(-1);
          }
        }
      }
    }
    return true;
  }
  return false;
}

void CountdownClock::print(void) {
  _dddd.writeDisplay();
  _hhmm.writeDisplay();
  _ssuu.writeDisplay();
}

void CountdownClock::display(void) {
  int   value;
  bool  changed;

  // days
  if (_dd.changed()) {
    _dd.sync();
    int days = _dd.value();
    // if its 0 days, display 1 zero
    if (days) 
      _dddd.println(days);
    else
      _dddd.writeDigitNum(4, 0, false);
    _dddd.writeDisplay();
  }

  // hhmm
  changed = false;
  if (_hh.changed()) {
    changed = true;
    _hh.sync();
  }
  if (_mm.changed()) {
    changed = true;
    _mm.sync();
  }
  if (changed) {
    int hours = _hh.value();
    if (hours < 10)
      _hhmm.clear();
    else
      _hhmm.writeDigitNum(0, int(hours/10), false);
    _hhmm.writeDigitNum(1, hours % 10   , false);
    int minutes = _mm.value();
    _hhmm.writeDigitNum(3, int(minutes/10), false);
    _hhmm.writeDigitNum(4, minutes % 10   , false);
    _hhmm.drawColon(true);
    _hhmm.writeDisplay();
  }
  
  // ssuu
  changed = false;
  if (_ss.changed()) {
    changed = true;
    _ss.sync();
    int seconds = _ss.value();
    if (seconds < 10)
      _ssuu.clear(); 
    else
      _ssuu.writeDigitNum(1, int(seconds/10), false);
    _ssuu.writeDigitNum(3, seconds % 10   , true);

  }
  if (_uu.changed()) {
    changed = true;
    _uu.sync();
    _ssuu.writeDigitNum(4, int(_uu.value()/100), false);
  }
  if (changed) {
    _ssuu.drawColon(false);
    _ssuu.writeDisplay();
  }
}

void  CountdownClock::clear(void) {
  _dddd.clear();
  _hhmm.clear();
  _ssuu.clear();
}

void  CountdownClock::done_message(bool display) {
  _dddd.clear();
  _hhmm.clear();
  _ssuu.clear();
  _dddd.writeDisplay();
  _hhmm.writeDisplay();
  _ssuu.writeDisplay();

  if (display) {
//    _dddd.writeDigitNum(1, 8, false);
//    _dddd.writeDigitNum(3, 4, false);
//    _dddd.writeDigitRaw(1, asci_mask('B'));
//    _dddd.writeDigitRaw(3, asci_mask('Y'));
//    _dddd.writeDigitRaw(4, asci_mask('E'));
    _hhmm.writeDigitRaw(1, asci_mask('O'));
    _hhmm.writeDigitRaw(3, asci_mask('L'));
    _hhmm.writeDigitNum(4, 1, false);

    _ssuu.writeDigitRaw(0, asci_mask('U'));
    _ssuu.writeDigitNum(1, 1, false);
    _ssuu.writeDigitRaw(3, asci_mask('A'));
    /*
    _hhmm.writeDigitRaw(1, asci_mask('F'));
    _hhmm.writeDigitRaw(3, asci_mask('U'));
    _hhmm.writeDigitRaw(4, asci_mask('C'));

    _ssuu.writeDigitNum(1, 4, false);
//    _ssuu.writeDigitRaw(1, asci_mask('Y'));
    _ssuu.writeDigitRaw(3, asci_mask('O'));
    _ssuu.writeDigitRaw(4, asci_mask('U'));
    */

  }

  _dddd.writeDisplay();
  _hhmm.writeDisplay();
  _ssuu.writeDisplay();
}

