/*
  Name:      SLOAD.H
  Copyright: 
  Author: m   MAURICIO LOPEZ
  Date: 22/07/06 11:48
  Description:  SLOAD ROUTINES
*/

#ifndef __SLOAD_H__
#define __SLOAD_H__

#include <stdlib.h>
#include "allegro.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////
//                           misc functions
///////////////////////////////////////////////////////////////////////////

typedef struct PROG_DATA{
unsigned int ini;
unsigned int fin;
unsigned int cur;
unsigned int step;
}PROG_DATA;

extern BITMAP *_prog_bmp;
extern PROG_DATA *_prog_data;
extern void (*progress_proc)(void);

void ini_progress_proc(BITMAP *dest,PROG_DATA *p_d);

///////////////////////////////////////////////////////////////////////////
//                           misc functions
///////////////////////////////////////////////////////////////////////////

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __SLOAD_H__
