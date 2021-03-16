#include "3ddef.h"
#include "timers.h"
#include <stdio.h>


int n_poly_sec = 0;
int _polycount = 0;

void chkfps_polycount()
{
 frame++;
 if (t > _Timevel) {
     fps = (_Timevel * frame) / t;
     t = 0;
     frame = 0;
     n_poly_sec = _polycount;
     _polycount = 0;        
 }
}

/* A simple font to display some info on screen */
FONT *agl_font;

/////////////////////////////////////////////////////////////////
//////////////////////////////	Math Functions  ////////////////////////////////*

// This divides a vector by a single number (scalar) and returns the result
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;							// The variable to hold the resultant vector
	
	vResult.x = vVector1.x / Scaler;			// Divide Vector1's x value by the scaler
	vResult.y = vVector1.y / Scaler;			// Divide Vector1's y value by the scaler
	vResult.z = vVector1.z / Scaler;			// Divide Vector1's z value by the scaler

	return vResult;								// Return the resultant vector
}

// This returns the cross product between 2 vectors
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vCross;
}

// This returns the normal of a vector
CVector3 Normalize(CVector3 vNormal)
{
	double Magnitude;		

	Magnitude = Mag(vNormal);

	return vNormal / (float)Magnitude;
}

float Magnitude(CVector3 vNormal)
{
	return (float)fsqrt( (vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z) );
}

CVector3 Normal(CVector3 vVector1,CVector3 vVector2)					
{														
	CVector3 vNormal = Cross(vVector1, vVector2);

	vNormal = Normalize(vNormal);

	return vNormal;	
}

float DotProduct( const CVector3 &vVector1, const CVector3 &vVector2 )
{
	return vVector1.x * vVector2.x + vVector1.y * vVector2.y + vVector1.z * vVector2.z;
}

// Determines the normal from any three points on a plane.
CVector3 getNormal(CVector3 point1, CVector3 point2, CVector3 point3) {
	CVector3 theNormal;
	theNormal.x = (point1.y - point3.y)*(point2.z - point3.z) - (point2.y - point3.y)*(point1.z - point3.z);
	theNormal.y = (point2.x - point3.x)*(point1.z - point3.z) - (point1.x - point3.x)*(point2.z - point3.z);
	theNormal.z = (point1.x - point3.x)*(point2.y - point3.y) - (point2.x - point3.x)*(point1.y - point3.y);
	return theNormal;
}

/////////////////////////////////////////////////////////////////////////////////
/* For display, we'd like to convert the QUAT back to heading, pitch and roll
 * These don't serve any purpose but to make it look human readable.
 * Note: Produces incorrect results.
 */
void convert_quat(QUAT *q, float *heading, float *pitch, float *roll) {
	MATRIX_f matrix;
	quat_to_matrix(q, &matrix);
	
	*heading = fatan2(matrix.v[0][2], matrix.v[0][0]);
	*pitch   = fasin(matrix.v[0][1]);
	*roll    = fatan2(matrix.v[2][1], matrix.v[2][0]);
}

///////////////////////////////////////////////////////////////////////////
//                           class bbox
///////////////////////////////////////////////////////////////////////////

_bbox::_bbox()
{
}


_bbox::_bbox(CVector3 *_min,CVector3 *_max)
{
  init(_min, _max);
}

_bbox::~_bbox()
{
}
   
void _bbox::init(CVector3 *_min,CVector3 *_max)
{
   min = *_min;
   max = *_max;
}

void _bbox::scale(float _scale)
{
   min *= _scale;
   max *= _scale;
}
///////////////////////////////////////////////////////////////////////////
//                           collision bbox
///////////////////////////////////////////////////////////////////////////

int check_bb_collision(_bbox *bb1, _bbox *bb2)
{
 if(bb2->max.x < bb1->min.x)  return 0;
 if(bb2->min.x > bb1->max.x)  return 0;
 
 if(bb2->max.y < bb1->min.y)  return 0;
 if(bb2->min.y > bb1->max.y)  return 0;
 
 if(bb2->max.z < bb1->min.z)  return 0;
 if(bb2->min.z > bb1->max.z)  return 0;     
 return 1 ;                     
}

   
