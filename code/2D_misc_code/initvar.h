/*
  Name:      INITVAR.H 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  INITIALIZATION ROUTINES
*/

#ifndef __INITVAR_H_
#define __INITVAR_H_

#include <stdlib.h>
#include "allegro.h"
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
  int mode,w,h,bpp,idig,imid;
  int modeblit;         //double buffer, page fliping
  BITMAP *buffer;         
  BITMAP *page[2];      //buffers for page fliping
  int  active_page;     //current page
  
  INIT_ENVIROMENT(char *filename);
  ~INIT_ENVIROMENT();
  
  void blitting_mode(int type);
  void destroy_video_buffers();
  
  void blit_buffer();
  void page_fliping();
  void update_blit();  
};

/////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __INITVAR_H_
