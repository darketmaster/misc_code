/*
  Name:      TILEMAP.H
  Copyright: 
  Author: m   MAURICIO LOPEZ
  Date: 22/07/06 11:48
  Description:  TILEMAP ROUTINES
*/

#ifndef __TILEMAP_H__
#define __TILEMAP_H__

#include <stdlib.h>
#include "allegro.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////
//                           misc functions
///////////////////////////////////////////////////////////////////////////

#define T_NOFLIP 0
#define T_HFLIP  1 
#define T_VFLIP  2
#define T_VHFLIP 4
#define T_TRANS  8
#define T_LIT    16
#define T_CHAR   32

typedef struct TILE{
  int index;
  char prop;      
}TILE;



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

#endif // __TILEMAP_H__
