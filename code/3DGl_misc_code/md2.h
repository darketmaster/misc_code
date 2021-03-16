#ifndef __MD2_H__
#define __MD2_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/////////////////////////////////////////////////////////////////////////////

#define MAX_Triangles     4096
#define MAX_Vertices      2048
#define MAX_Texture_Coord 2048 
#define MAX_Frames        512
#define MAX_Skins         32


typedef struct model_t 
{ 
   int magic; 
   int version; 
   int skinWidth; 
   int skinHeight; 
   int frameSize; 
   int numSkins; 
   int numVertices; 
   int numTexCoords; 
   int numTriangles; 
   int numGlCommands; 
   int numFrames; 
   int offsetSkins; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetGlCommands; 
   int offsetEnd; 
} model_t;

#define MAGIC_WORD 0x32504449 //844121161 //int "IDP2" //('I' + ('D' << 8) + ('P' << 16) + ('2' << 24))
#define byte unsigned char

typedef struct triangleVertex_t
{
   byte vertex[3];
   byte lightNormalIndex;
} triangleVertex_t;

typedef struct frame_t
{
   float scale[3];
   float translate[3];
   char name[16];
   triangleVertex_t *vertices;
} frame_t;

typedef struct triangle_t
{
   short vertexIndices[3];
   short textureIndices[3];
} triangle_t;

typedef struct textureCoordinate_t
{
   short s, t;
} textureCoordinate_t;

typedef struct glCommandVertex_t
{
   float s, t;
   int vertexIndex;
} glCommandVertex_t;


class MD2MODEL
{
 public:
   int magic; 
   int version; 
   int skinWidth; 
   int skinHeight; 
   int frameSize; 
   int numSkins; 
   int numVertices; 
   int numTexCoords; 
   int numTriangles; 
   int numGlCommands; 
   int numFrames; 
   int offsetSkins; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetGlCommands; 
   int offsetEnd; 
   
   frame_t *frames;
   triangle_t *triangles;
   textureCoordinate_t *texturecoord;
   
   MD2MODEL(char *filename);
   ~MD2MODEL();
   
   void load(char *filename);
   void destroy();
   
   void debug();
};


#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __MD2_H__
