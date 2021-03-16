#ifndef __MDF_H__
#define __MDF_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "3ddef.h"

#ifndef MDFSIG
#define MDFSIG 0X3146444D
#endif
/////////////////////////////////////////////////////////////////////////////
//                        MDFMODEL
/////////////////////////////////////////////////////////////////////////////
typedef struct triVertex_t
{
   unsigned char vertex[3];
   unsigned char lightNormalIndex;
} triVertex_t;

typedef struct texCoord_t
{
   short s, t;
} texCoord_t;

typedef struct tri_mdf
{
   unsigned short v[3];        //poligon vertices id
   unsigned short uv[3];
} tri_mdf;

typedef struct mdf_tri_t
{
   unsigned char v[3];
} mdf_tri_t;

typedef struct mdf_frame_t
{
   CVector3 scale;
   CVector3 translate;
   mdf_tri_t *vertices;
} mdf_frame_t;

typedef struct glCommandV_t
{
   float s, t;
   int vertexIndex;
} glCommandV_t;

/*
GL COMMANDS 
At offsetGlCommands bytes into the file, there is the gl command list, 
which is made up of a series of numGlCommands int's and float's, 
organized into groups. Each group starts with an int. 
If it is positive, it is followed by that many glCommandVertex_t structures, 
which form a triangle strip. If it is negative, 
it is followed by -x glCommandVertex_t structures, 
which form a triangle fan. 
A 0 indicates the end of the list. 
The list is an optimized way of issuing commands when rendering with OpenGl. 
typedef struct
{
   float s, t;
   int vertexIndex;
} glCommandVertex_t;
float s, t: These two floats are used to map a vertex onto a skin. 
The horizontal axis position is given by s, and the vertical axis position is given by t. 
The range for s and for t is 0.0 to 1.0. Note that the ranges are different than in the textureCoordinate_t structure. 
They are stored as floats here because that's the way Quake2 passes them to OpenGl. 
int vertexIndex: Index into the array of vertices stored in each frame. 
*/

class MDFMODEL
{
 public:
   int frameSize; 
   int numVertices; 
   int numTexCoords; 
   int numTriangles; 
   int numFrames; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetEnd; 
   
   int numGlCommands; //new
   int offsetGlCommands; //new
   
   long *glCommand;
   
   mdf_frame_t *frames;
   tri_mdf *triangles;
   CVector2 *texturecoord;
      
   MDFMODEL();
   ~MDFMODEL();
   void blank();
   
   void load_md2(char *filename);
   void save_mdf(char *filename);
   void load_mdf(char *filename);  
   void destroy();   
   void debug();
   virtual void scale(float _scale);
};


/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __MDF_H__
