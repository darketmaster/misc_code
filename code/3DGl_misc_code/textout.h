#ifndef __TEXTOUT_H__
#define __TEXTOUT_H__

#include <stdlib.h>
#include "allegro.h"
#include "alleggl.h"
#include <stdio.h>
/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////

///////////////////



GLuint init_cfont(float _xoff);
void destroy_cfont(GLuint base);
void print_text(GLuint base, int TEXT_HEIGHT,float x, float y, const char *fmt, ...);
void OrthoMode(int left, int top, int right, int bottom);
void PerspectiveMode();


/////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __TEXTOUT_H__
