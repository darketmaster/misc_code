#ifndef __3DDEF_H_
#define __3DDEF_H_

#include <stdlib.h>
#include "allegro.h"

#include "float.h"

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

/* Define M_PI in case the compiler doesn't */
#ifndef M_PI
   #define M_PI   3.1415926535897932384626433832795
#endif

//////////////////////////////////////////////////////////////////////////////
extern int n_poly_sec;  //polys per second
extern int _polycount;  //counter of polys

void chkfps_polycount();

//////////////////////////////////////////////////////////////////////////////
/* Macro to convert radians to degrees */
#define RAD2DEG(x) ((x) * 57.295778) //180/M_PI
#define DEG2RAD(x) ((x) * 0.0174532) //M_PI/180

#define Mag(Normal) (fsqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

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
		return fsqrt((x*x) + (y*y));
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
		return fsqrt(x*x + y*y + z*z);
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

	float x, y, z;
 
    void draw(CVector3 *inipos, float l); //must user defined
    						
};


CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler);
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);
CVector3 Normalize(CVector3 vNormal);
float Magnitude(CVector3 vNormal);
CVector3 Normal(CVector3 vVector1,CVector3 vVector2);
float DotProduct( const CVector3 &vVector1, const CVector3 &vVector2 );

// Determines the normal from any three points on a plane.
CVector3 getNormal(CVector3 point1, CVector3 point3, CVector3 point4);

//////////////////////////////////////////////////////////////////////////////
/* For display, we'd like to convert the QUAT back to heading, pitch and roll
 * These don't serve any purpose but to make it look human readable.
 * Note: Produces incorrect results.
 */
void convert_quat(QUAT *q, float *heading, float *pitch, float *roll);


//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//                           class bbox
///////////////////////////////////////////////////////////////////////////

class _bbox{
public:
   CVector3 min,max;
  
   _bbox();  
   _bbox(CVector3 *_min,CVector3 *_max);
   ~_bbox();
   
   void init(CVector3 *_min,CVector3 *_max);
   void scale(float _scale);
   virtual void render() {};   
};

///////////////////////////////////////////////////////////////////////////
//                           collision bbox
///////////////////////////////////////////////////////////////////////////

int check_bb_collision(_bbox *bb1, _bbox *bb2);


/* A simple font to display some info on screen */
extern FONT *agl_font;


/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __3DDEF_H_
