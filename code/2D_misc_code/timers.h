/*
  Name:      TIMERS.H 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  TIMER ROUTINES
*/

#ifndef __TIMERS_H_
#define __TIMERS_H_

#include <stdlib.h>
#include "allegro.h"

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

/////////////////////timer rutines//////////////////////////////////////////////////////////

extern int _Timevel;            //timer velocity per seconds
extern volatile int t;          //aux counter
extern int frame;               //frame counter
extern float fps;               //fps
extern volatile long int decs;  //counter

//timer interrupt handler /
void tick(void);
void install_tick();
void remove_tick();
void chkfps();


//////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __TIMERS_H_
