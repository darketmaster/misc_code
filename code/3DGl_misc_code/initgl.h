#ifndef __INITVAR_H_
#define __INITVAR_H_

#include <stdlib.h>
#include "allegro.h"
#include "alleggl.h"
#include <stdio.h>

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

//init routines 
 
#define MODE_DOUBLEBUFFER    0
#define MODE_PAGEFLIPING    1
#define BUFFER_VIDEO        2
 
/////////////////////////////////////////////////////////////////////////////
class INIT_ENVIROMENT{
public:
  int mode,w,h,bpp,idig,imid,zbpp;
  int ffullscreen;
  
  INIT_ENVIROMENT(char *filename);
  ~INIT_ENVIROMENT();

  void update_blit();  
};

/////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __INITVAR_H_
