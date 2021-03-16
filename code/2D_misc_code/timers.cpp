/*
  Name:      TIMER.H 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  TIMER ROUTINES
*/

#include "timers.h"
#include "allegro.h"

int _Timevel = 60;
volatile int t;
volatile long int decs;
int frame;
float fps;


///////////////////////////Principal timer func//////////////////////////////////////////
void tick(void)
{
   t++;
   decs++;
}
END_OF_FUNCTION(tick);


/////////////////////////Install timer func////////////////////////////////////////////
void install_tick()
{
   LOCK_VARIABLE(decs);
   LOCK_VARIABLE(t);
   LOCK_FUNCTION(tick);
   install_int_ex(tick, BPS_TO_TIMER(_Timevel));
}

///////////////////////Remove Timer func///////////////////////////////////////////////
void remove_tick()
{
 remove_int(tick);
}

///////////////////////check the fps////////////////////////////////////////////////////
void chkfps()
{
 frame++;
 if (t > _Timevel) {
     fps = (_Timevel * frame) / t;
     t = 0;
     frame = 0;
 }
}
