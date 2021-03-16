#ifndef __CAMERA_H_
#define __CAMERA_H_

#include <stdlib.h>
#include "allegro.h"

#include "3ddef.h"
#include <stdio.h>

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

class CAMERA{
public:

 int viewport_w;  /* Viewport Width  (pixels)  */
 int viewport_h;  /* Viewport Height (pixels)  */
 int fov;         /* Field of view   (degrees) */
 float aspect;    /* Aspect ratio              */
 float per;    
 int xoffset;
 int yoffset;
 
 CVector3 position;
 QUAT orientation;
 
 CAMERA ();
 CAMERA (int vpw_, int vph_, int fov_, float asp_);
 CAMERA (int vpw_, int vph_, int fov_, float asp_, float x, float y, float z, float heading,float  pitch,float  roll);
 ~CAMERA ();
 
 void set_pos(float x, float y, float z);
 void set_ori(float heading,float  pitch,float  roll);
 void set_var(int vpw_, int vph_, int fov_, float asp_);

 void draw_overlay();
 void process_input(void);
 void set_camera();
 
 void set_offsets(int xoff, int yoff){ xoffset = xoff; yoffset = yoff;}
 

};










//////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __CAMERA_H_
