#ifndef COUNTDOWN_CLOCK
#define COUNTDOWN_CLOCK

#include "countdown_config.h"

class CountdownClock {
  public:

    CountdownClock();

    void      set_time(int*);
    void      set_time(int days=0,int hours=0, int minutes=0, int seconds=0, int ms=0);
    void      get_time(int*);
    void      set_direction(int);
    int       get_direction(void);

    bool      tick(int);  // could be positive or negative
    bool      done(void);
    void      print(const char* msg=NULL);

  private:
    bool      update(int&, int&, int, int);
    //
    // days, hours, minutes, seconds, tenths
    int       _time[N_ELEMENTS];
    int       _direction;  // < 0 is counting down
};

extern  CountdownClock  gClock;
#endif
