#include <Arduino.h>
#include "countdown_config.h"
#include "countdown_clock.h"

div_t div(int a, int b) {
  div_t r;
  r.quot = int(a / b);
  r.rem  = a % b;
  return r;
}

CountdownClock::CountdownClock(void) {}

void  CountdownClock::set_time(int days, int hours, int minutes, int seconds, int ms) {
  _time[DAYS]    = days;
  _time[HOURS]   = hours;
  _time[MINUTES] = minutes;
  _time[SECONDS] = seconds;
  _time[MILLIS]  = ms;
}
void  CountdownClock::set_time(int* time) { memcpy(_time, time, N_ELEMENTS*sizeof(int)); }
void  CountdownClock::get_time(int* time) { memcpy(time, _time, N_ELEMENTS*sizeof(int)); }
bool  CountdownClock::done(void) { return 
  !(_time[MILLIS] || _time[SECONDS] || _time[HOURS] || _time[DAYS]);
}

bool  CountdownClock::update(int& borrow, int& target, int amount, int modulo) {
  int prev = borrow;
  div_t r = div( target + amount, modulo );
  target = r.rem;
  if (target < 0) {
    target += modulo;
    borrow--;
  } else if (target >= modulo) {
    target -= modulo;
    borrow++;
  }
  borrow += r.quot;
  return prev != borrow;  // did we change the borrow
}

bool  CountdownClock::tick(int delta) {
// returns true if something changed
  if (done())
    return false;

  if (update( _time[SECONDS], _time[MILLIS], delta, 1000))
    if (update( _time[MINUTES], _time[SECONDS], 0, 60))
      if (update( _time[HOURS], _time[MINUTES], 0, 60))
        if (update( _time[DAYS], _time[HOURS], 0, 24))
          // check to see if we are out of time0
          if (_time[DAYS] < 0)
            set_time(0);
  return true;
}

void  CountdownClock::print(const char* msg) {
  if (msg)
    Serial.printf("%s ",msg);
  Serial.printf("%d %02d:%02d %02d.%03d\n",_time[DAYS],_time[HOURS],_time[MINUTES],_time[SECONDS],_time[MILLIS]);
}
