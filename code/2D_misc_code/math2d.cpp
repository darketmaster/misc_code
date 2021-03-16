#define ALLEGRO_INCLUDE_MATH_H

#include "allegro.h"
#include "math2d.h"
#include "math.h"

#ifndef M_PI
   #define M_PI   3.14159265358979323846
#endif

void correct_angle(float *ang)
{
  if(*ang >= 360.0) while(*ang >= 360.0) *ang -= 360.0;
  else
  if(*ang <   0.0) while(*ang < 0.0) *ang += 360.0;
}

float get_angle_xz(float x,float z)
{
 float ang;
    ang = RAD2DEG(atan2(x,z));
 correct_angle(&ang);
 return (ang);
}

float *Sintab = NULL;
float *Costab = NULL;
float *Tantab = NULL;
float *aTantab = NULL;
/*
#define floatcos(x)     Costab[(int)(x*10)] //cos((x) * M_PI / 128.0)
#define floatsin(x)     Sintab[(int)(x*10)] //sin((x) * M_PI / 128.0)
#define floattan(x)     Tantab[(int)(x*10)] //tan((x) * M_PI / 128.0) */

void math_exit(void)
{
 if(Sintab)
    free(Sintab);
 if(Costab)
    free(Costab);
 if(Tantab)
    free(Tantab);
}

int math_ini(void)
{
 int i;
 Sintab = (float*)calloc(3600,sizeof(float));
 Costab = (float*)calloc(3600,sizeof(float));
 Tantab = (float*)calloc(3600,sizeof(float));

  if(!Sintab || !Costab || !Tantab)
  {
     math_exit();
     return FALSE;
  }
  for(i=0; i<3600; i++)
  {
      Sintab[i] = sin(DEG2RAD((float) i / 10.0));
      Costab[i] = cos(DEG2RAD((float) i / 10.0));
      Tantab[i] = tan(DEG2RAD((float) i / 10.0));
  }
  return TRUE;
}

