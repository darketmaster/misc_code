#ifndef __ISOMETRIC_H_
#define __ISOMETRIC_H_
#include <stdlib.h>
#include "allegro.h"
#include "math.h"

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif
////////////////////////////////////////////////////////////////

//valores  para tang beta = .5 en DEG
#define C_BETA        26.5650511771
#define C_ALFA        153.434948823
#define C_COS_BETA    0.894427191
#define C_SIN_BETA    0.4472135955
#define C_COS_ALFA    -0.894427191
#define C_SIN_ALFA    0.4472135955

#define M_PI 3.14159265359
#define M_PI2 1.5707963268
#define M_2PI 6.28318530718

#define C_PI_DIV_180  57.2957795131
#define C_180_DIV_PI  0.0174532925199

#define RADTODEG(x)  (x)*57.2957795131
#define DEGTORAD(x)  (x)*0.0174532925199

extern int Wiso, Hiso;
extern double RELATION_ISOMETRIC;
extern double BETA;  //angulo del eje isometrico x con respecto al x en coorden rectangulares
extern double ALFA;  //angulo del eje isometrico y con respecto al x en coorden rectangulares
extern double SIN_BETA, COS_BETA, SINCOS_BETA;
extern double SIN_ALFA, COS_ALFA, SINCOS_ALFA;

//transformaciones
//calcula la componente x rectangular de la coordenada x,y isometrica
inline float x_iso_to_rec(float x,float y);
//calcula la componente y rectangular de la coordenada x,y isometrica
inline float y_iso_to_rec(float x,float y);
//calcula la componente x isometrica de las coordenadas x,y rectangulares
inline float x_rec_to_iso(float x,float y);
//calcula la componente y isometrica de las coordenadas x,y rectangulares
inline float y_rec_to_iso(float x,float y);
void ini_isometric_math(float xrel,float yrel);

/////////////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __ISOMETRIC_H_
