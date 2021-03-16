#ifndef __MODELS_H__
#define __MODELS_H__

#include <stdlib.h>
#include <allegro.h>
#include "alleggl.h"
#include "3ddef.h"
//#include "3ds.h"
#include "mdf.h"
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////
//                          textures
///////////////////////////////////////////////////////////////////////////
#define TLOW   0
#define TNORM  1
#define THIGH  2

#define TNOMASKED  0
#define TMASKED    1

GLuint make_texture(char *filename, int type, int masked);
/*
///////////////////////////////////////////////////////////////////////////
//                           misc functions
///////////////////////////////////////////////////////////////////////////
void draw_t3DModel(t3DModel *g_3DModel, int g_ViewMode, UINT g_Texture[]);
void set_textures_g_3DModel(t3DModel *g_3DModel,UINT g_Texture[], char *PATHTEXTURES3DS);
void debug_g_3DModel(t3DModel *g_3DModel);

*/
///////////////////////////////////////////////////////////////////////////
//                           _anim
///////////////////////////////////////////////////////////////////////////
/* File in ascii format
   //ANIMATION_FORMAT
   
   ANIMATION.
   NAME= "ANIMATION_NAME 0"
   N_FRAMES= xxx             
   LIST_BEGIN.
   xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx.....
   LIST_END.   
   
   ANIMATION.
   NAME= "ANIMATION_NAME 1"
   N_FRAMES= xxx             
   LIST_BEGIN.
   xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx.....
   LIST_END.
   
   .
   .
   .
   
   ANIMATION.   
   NAME= "ANIMATION_NAME n"
   N_FRAMES= xxx             
   LIST_BEGIN.
   xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx xxx.....
   LIST_END.
      
*/

class _anim{
public:
 char name[64];          //animation name
 int n_anim;            //number of the animation key frames
 short *keyframe;        //key frame vector
 
 _anim();
 _anim(char *name,int tam); 
 ~_anim();
 
 void load(FILE *fp);
 void save(FILE *fp); 
    
 void destroy();
 void create(char *name,int tam); 
 void set(int n, short data);
 void debug(); 
};

///////////////////////////////////////////////////////////////////////////
//                           _anim
///////////////////////////////////////////////////////////////////////////

class ANIMATION{
public:

 short n_animations; //number animations
 _anim *animation;//vector<_anim> animation;
 
 ANIMATION();
 ~ANIMATION();
 
 void load(char *filename);
 void save(char *filename);
};

///////////////////////////////////////////////////////////////////////////
//                           STANIMATION
///////////////////////////////////////////////////////////////////////////

class STANIMATION{
public:
 
 int velanim;         //vel in miliseconds
 float anim_per;      //percentaje 1000 / velanim
 
 ANIMATION *pinfo;    //pointer to animation info
 
 int canim;           //current animation
 
 int currkanim;       //current key animation frame
 int nextkanim;       //next key animation frame
 
 float elapsedTime;   //temp var elapsedTime
 float lastTime;      //temp var lastTime
 float time;
 
 STANIMATION();
 ~STANIMATION();
 
 void create(int _velanim, ANIMATION *_pinfo); 
 void set_vel(int _velanim);
 void set_canim(int _canim);
 float Animate();
 
 int get_ckanim();
 int get_nkanim(); 
 
};

///////////////////////////////////////////////////////////////////////////
//                           BBOX
///////////////////////////////////////////////////////////////////////////

class BBOX : public _bbox{

   public:
   GLubyte color[3];
   BBOX(){};
   BBOX(CVector3 *_min,CVector3 *_max);
   BBOX(CVector3 *_min,CVector3 *_max, GLubyte col[]);
   void init(CVector3 *_min,CVector3 *_max, GLubyte col[]);
   ~BBOX(){};
   void render();
};

///////////////////////////////////////////////////////////////////////////

typedef struct tri_mda
{
   unsigned short v[3];        //poligon vertices id
   unsigned short uv[3];
} tri_mda;


///////////////////////////////////////////////////////////////////////////
//                           frame_obj3d
///////////////////////////////////////////////////////////////////////////

class frame_obj3d{
public:

   int numVertices;
   CVector3  *pVerts;		// The object's vertices
   CVector3  *pNormals;		// The object's normals
   _bbox bbox;
   
   frame_obj3d();
   ~frame_obj3d();
      
   void create(int nv);
   void destroy();
   void save_data(FILE *fp);
   void scale(float _scale);
   
   void save_data_mdf(FILE *fp);
   void load_data_mdf(FILE *fp);    
};

///////////////////////////////////////////////////////////////////////////
//                           _obj3d
///////////////////////////////////////////////////////////////////////////
#define MDFSIG 0X3146444D //('M' + ('D' << 8) + ('F' << 16) + ('1' << 24))
/*
   MDFSIG         4 bytes
   numTriangles   4 bytes
   numTexCoords   4 bytes   
   numFrames      4 bytes 
     
   offsetTexCoords 4 bytes
   offsetTriangles 4 bytes
   offsetFrames    4 bytes
   
   
   TRIANGLES
   pTri            sizeof(tri_mda)*numTriangles bytes
   
   TEXTURECOORD
   texcoord        sizeof(CVector2)*numTexCoords bytes
   
   FRAMES
   numVertices     4 bytes
   pVerts          sizeof(CVector3)*numvertices bytes
   pNormals        sizeof(CVector3)*numvertices bytes
   bbox            sizeof(_bbox)  bytes   
*/
class _obj3d{
public:

   int numFrames;
   int numTexCoords; 
   int numTriangles;
   tri_mda   *pTri;			// The faces information of the object 
   CVector2 *texcoord;
        
   frame_obj3d *data;        //frame_md2 data vector      
   CVector3  *pVtemp;		//tmp for animation   
   BBOX tbbox;
   
   _obj3d();
   ~_obj3d();
   
   void create(int nf, int nt, int ntr);
   void destroy();
   void set_tri(int n, tri_mda *tri);
   
   void load(char *filename);
   void render_interpoled(int cframe, int nframe, float t);   
   void save_verts_info(char *filename,int frame); 
   void scale(float _scale);
   void calculate_interpoled(int cframe, int nframe, float t);
   void render();
   void render_outline();
   
   void load_mdf(char *filename);
   void save_mdf(char *filename);       
};


/////////////////////////////////////////////////////////////////////////////
//               GLMATERIAL
/////////////////////////////////////////////////////////////////////////////
#define MAX_NUM_TEXTURES 256
class GLMATERIAL{
public:
   int cur;      //num of max current texture (zero based)
   int max;      //max textures in vector
   GLuint *tex;  //texture vector
   
   GLMATERIAL();
   ~GLMATERIAL();
   GLMATERIAL(int n);
   
   void create(int n);
   void destroy();
   int add(char *filename, int type, int masked);
   int replace(int n, char *filename, int type, int masked);
};


/////////////////////////////////////////////////////////////////////////////
//               MDF_OBJECT
/////////////////////////////////////////////////////////////////////////////

class MDF_OBJ : public MDFMODEL{

public:

  CVector3  *pTVerts  ;	// The object's temp vertices
  CVector3  *pTVNormals;	// The object's temp vertices normals
  CVector3  *pTNormals;	// The object's temp normals
  BBOX tbbox          ;   //temporal BBOX
  _bbox     *bbox;        // frames bboxs

  MDF_OBJ();
  ~MDF_OBJ();    
  
  void create_mdf_obj();
  void blank_mdf_obj();
  void import_md2(char *filename);
  void import_mdf(char *filename);
  void destroy_mdf_obj();
  void debug_mdf_obj();
  void calculate_bbox();
  void calculate_interpoled(int cframe, int nframe, float t, int smooth);
  
  void render_flat();
  void render_smooth(); 
  void render_frame(int cframe); 
  void draw_normasl();
  void gl_render_flat();
  void gl_render_smooth();
  
  void scale(float _scale);
  
};




/////////////////////////////////////////////////////////////////////////////
/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif




/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __MODELS_H__


