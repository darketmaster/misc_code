#ifndef __GAME_H__
#define __GAME_H__

#include <stdlib.h>
#include "allegro.h"
#include "alleggl.h"
#include <GL/glu.h>

#include "gamevar.h"

///////////////////////////////////////////////////////////////////////////
//                           LIST_MODEL_NAMES
///////////////////////////////////////////////////////////////////////////

class LIST_MODEL_NAMES{
public:

 Gamepaths paths;
 int n;
 
 FLISTDATA flist;

};




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

#endif // __GAME_H__
