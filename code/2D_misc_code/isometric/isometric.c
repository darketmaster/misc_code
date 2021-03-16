#include "isometric.h"

int Wiso, Hiso;
double RELATION_ISOMETRIC;
double BETA;  //angulo del eje isometrico x con respecto al x en coorden rectangulares
double ALFA;  //angulo del eje isometrico y con respecto al x en coorden rectangulares
double SIN_BETA, COS_BETA, SINCOS_BETA;
double SIN_ALFA, COS_ALFA, SINCOS_ALFA;

//transformaciones
//calcula la componente x rectangular de la coordenada x,y isometrica
inline float x_iso_to_rec(float x,float y)
{
 float xrec;
 xrec = x*COS_BETA + y*COS_ALFA;
 return xrec;
}
//calcula la componente y rectangular de la coordenada x,y isometrica
inline float y_iso_to_rec(float x,float y)
{
 float yrec;
 yrec = y*SIN_BETA + x*SIN_ALFA;
 return yrec;
}
//calcula la componente x isometrica de las coordenadas x,y rectangulares
inline float x_rec_to_iso(float x,float y)
{
 float xiso;
 //xiso = (y*COS_ALFA - x*SIN_BETA)/(SIN_ALFA*COS_ALFA - SIN_BETA*COS_BETA);
 xiso = (y*COS_ALFA - x*SIN_BETA)/(SINCOS_ALFA - SINCOS_BETA);
 return xiso;
}
//calcula la componente y isometrica de las coordenadas x,y rectangulares
inline float y_rec_to_iso(float x,float y)
{
 float yiso;
 //yiso = (y*COS_BETA - x*SIN_ALFA)/(SIN_BETA*COS_BETA - SIN_ALFA*COS_ALFA);
 yiso = (y*COS_BETA - x*SIN_ALFA)/(SINCOS_BETA - SINCOS_ALFA);
 return yiso;
}

void ini_isometric_math(float xrel,float yrel)
{
 Wiso = xrel;
 Hiso = yrel;
 if(yrel != 0)
 {
    RELATION_ISOMETRIC = yrel / xrel;
    BETA = atan2(yrel,xrel);
    ALFA = M_PI - BETA;

 }else
 {
   RELATION_ISOMETRIC = 99999999999;
   BETA = M_PI2;
   ALFA = M_PI2;
 }

 SIN_BETA = sin(BETA);
 COS_BETA = cos(BETA);
 SINCOS_BETA = SIN_BETA * COS_BETA;
 SIN_ALFA = sin(ALFA);
 COS_ALFA = cos(ALFA);
 SINCOS_ALFA =  SIN_ALFA * COS_ALFA;
}
