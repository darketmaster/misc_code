#ifndef __FAST_MATH2D_
#define __FAST_MATH2D_

#include<stdlib.h>
#include "allegro.h"

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

/*------------var-----------------------------------------------------------------*/
#define floatcos(x)     Costab[(int)((x)*10.0)]
#define floatsin(x)     Sintab[(int)((x)*10.0)]
#define floattan(x)     Tantab[(int)((x)*10.0)]

#ifndef M_PI
   #define PI 3.14159265358979323846 // in math.h       180/3.14159
#endif
#define RAD2DEG(x)  (x)*57.2957732 //180/PI      //igual a x*180/PI  de radianes a deg
#define DEG2RAD(x)  (x)*0.0174532944//PI/180      //igual a x*PI/180  de deg a radianes
#define RAD2FIX(x)  (x)*40.74366094//128/PI      //igual a x*128/PI  de fix a radianes
#define FIX2RAD(x)  (x)*0.0245436953//PI/128      //igual a x*PI/128  de radianes a fix

#define DEG2FIX(x)  (x)*0.7111111 //igual a x*128/180
#define FIX2DEG(x)  (x)*1.40625   //igual a x*180/128

void correct_angle(float *ang);
float get_angle_xz(float x,float z);

extern float *Sintab;
extern float *Costab;
extern float *Tantab;
extern float *aTantab;

void math_exit(void);
int math_ini(void);


/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __FAST_MATH2D_
