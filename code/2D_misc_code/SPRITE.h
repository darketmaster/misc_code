/*
  Name:      SPRITE.H
  Copyright: 
  Author: m   MAURICIO LOPEZ
  Date: 22/07/06 11:48
  Description:  SPRITES ROUTINES
*/

#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <stdlib.h>
#include "allegro.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////
//                           SPRITES CLASS
///////////////////////////////////////////////////////////////////////////

#define MAXPALS 32

class SPRITES{
public:
       int nsprites;       //sprites number = w*h
       int npal;    //number of palette
       int nc, nf;    //size of the original bitmap in sprites numbers
       BITMAP **sprites;   //pointer to bitmaps array
       RGB **pal;          //pointer to palette array
       
       SPRITES();
       SPRITES(char *filename, int nc, int nf);
       ~SPRITES();
       void clear();       
       void destroy();
       int load(char *filename, int nc, int nf);
       int load_palette(char *filename);
       void draw(BITMAP *dest, int ns, int x, int y, int flip);             
};




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

#endif // __SPRITE_H__
