#include "3dvector.h"
#include <stdio.h>
#include <math.h>

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
	return (float)sqrtf( (vNormal.x * vNormal.x) + (vNormal.y * vNormal.y) + (vNormal.z * vNormal.z) );
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
