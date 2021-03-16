#ifndef __3VECTOR_H_
#define __3VECTOR_H_

#include <stdlib.h>

#include "float.h"
#include "math.h"

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

/////this is for allegro compatibility
typedef struct V3D_f                /* a 3d point (floating point version) */
{
   float x, y, z;                   /* position */
   float u, v;                      /* texture map coordinates */
   int c;                           /* color */
} V3D_f;

// This is our basic 3D point/vector class
class CVector2
{
public:
	
	CVector2() { x=0; y=0;}

	CVector2(float X, float Y) 
	{ 
		x = X; y = Y;
	}
	
	void init(float X, float Y, float Z) 
	{ 
		x = X; y = Y;
	}

	CVector2 operator+(CVector2 vVector)
	{
		return CVector2(vVector.x + x, vVector.y + y);
	}

	CVector2 operator-(CVector2 vVector)
	{
		return CVector2(x - vVector.x, y - vVector.y);
	}
	
	CVector2 operator*(float num)
	{
		return CVector2(x * num, y * num);
	}

	CVector2 operator/(float num)
	{
		return CVector2(x / num, y / num);
	}
	
	CVector2& operator+= (CVector2 vVector)
	{
		x += vVector.x;		y += vVector.y;
		return *this;
	}

	CVector2& operator-= (CVector2 vVector)
	{
		x -= vVector.x;		y -= vVector.y;
		return *this;
	}

	CVector2& operator*= (float value)	
	{
		x *= value;		y *= value;
		return *this;
	}

	CVector2& operator/= (float value)
	{
		x /= value;		y /= value;
		return *this;
	}

	CVector2 operator- ()
    {	
		return CVector2(-x, -y);
	}
	
	float length()
	{
		return sqrtf((x*x) + (y*y));
	}
	
	void unitize()
	{
		float length = this->length();
		if (length == 0)
			return;
		x /= length;		y /= length;
	}

	CVector2 unit()
	{
		float length = this->length();
		if (length == 0)
			return *this;		
		return CVector2(x / length, y / length);
	}

	float x, y;						
};

// This is our basic 3D point/vector class
class CVector3
{
public:
	
	CVector3() { x=0; y=0; z=0;}

	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}
	
	void init(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	CVector3 operator+(CVector3 vVector)
	{
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	CVector3 operator-(CVector3 vVector)
	{
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	CVector3 operator*(float num)
	{
		return CVector3(x * num, y * num, z * num);
	}

	CVector3 operator/(float num)
	{
		return CVector3(x / num, y / num, z / num);
	}
	
	CVector3& operator+= (CVector3 vVector)
	{
		x += vVector.x;		y += vVector.y;		z += vVector.z;
		return *this;
	}

	CVector3& operator-= (CVector3 vVector)
	{
		x -= vVector.x;		y -= vVector.y;		z -= vVector.z;
		return *this;
	}

	CVector3& operator*= (float value)	
	{
		x *= value;		y *= value;		z *= value;
		return *this;
	}

	CVector3& operator/= (float value)
	{
		x /= value;		y /= value;		z /= value;
		return *this;
	}

	CVector3 operator- ()
    {	
		return CVector3(-x, -y, -z);
	}
	
	float length()
	{
		return sqrtf(x*x + y*y + z*z);
	}
	
	void unitize()
	{
		float length = this->length();
		if (length == 0)
			return;
		x /= length;		y /= length;		z /= length;
	}

	CVector3 unit()
	{
		float length = this->length();
		if (length == 0)
			return *this;		
		return CVector3(x / length, y / length, z / length);
	}

    void draw(CVector3 *inipos, float l); //must user defined
    
   	CVector3& operator= (V3D_f vVector)
	{
		x = vVector.x;		y = vVector.y;		z = vVector.z;
		return *this;
	}
	
	float x, y, z;
    						
};

#define Mag(Normal) (sqrtf(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler);
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);
CVector3 Normalize(CVector3 vNormal);
float Magnitude(CVector3 vNormal);
CVector3 Normal(CVector3 vVector1,CVector3 vVector2);
float DotProduct( const CVector3 &vVector1, const CVector3 &vVector2 );

// Determines the normal from any three points on a plane.
CVector3 getNormal(CVector3 point1, CVector3 point2, CVector3 point3);




/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __3VECTOR_H_
