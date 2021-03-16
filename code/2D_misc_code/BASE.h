/*
  Name:      BASE.H 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  SAMUS ROUTINES
*/

#ifndef __BASE_H_
#define __BASE_H_

#include <stdlib.h>
#include "allegro.h"

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

///////////////////// rutines//////////////////////////////////////////////////////////
#define MAXNAMESIZE 64
#define P_NORMAL 0
#define P_HFLIP  1
#define P_VFLIP  2
#define P_HVFILP 3

class _move{
public:
       char name[MAXNAMESIZE];
       char prop;
       _move();
       ~_move();
       void ini(char *cad);
};

#define M_DEFAULT  0
#define M_STAND_F  1
#define M_STAND_B  2
#define M_STAND_L  3
#define M_STAND_R  4
#define M_CROUCH_F 5
#define M_CROUCH_B 6
#define M_CROUCH_L 7
#define M_CROUCH_R 8


//////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __BASE_H_
