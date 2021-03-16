#ifndef _BONES_H_
#define _BONES_H_

#include <stdio.h>
#include <GL/gl.h>

#include "3dmatrix.h"
#include "MS3DMODEL.H" 

#define MS_MAX_NAME             32
#define MS_MAX_PATH             256

#define BONES3DHEADER "BONES3D" 

/////////////////////////////////////////////////////////////////////////////


class KEYFRAME
{
	public:
		int	Time;
        float	Pos[3];
		float	Rot[3];		
	
    KEYFRAME();
    ~KEYFRAME();    	
    void create(int time, float *p, float *r);
};



/////////////////////////////////////////////////////////////////////////////

class BONE3D
{
	public:
		char	Name[MS_MAX_NAME];
		char	ParentName[MS_MAX_NAME];
		BONE3D	*Parent;						// pointer to parent bone (or NULL)
		float	startPosition[3];
		float	startRotation[3];
		MATRIX_f	m_relative;				// fixed transformation matrix relative to parent 
		MATRIX_f	m_final;				// absolute in accordance to animation
		MATRIX_f    m_absolute;
		int		NumKeys;
		KEYFRAME *KeyFrames;	
     
      	BONE3D();
		~BONE3D();
		
		void  create(char *name, char *parentname, float *spos, float *srot, int nkeys, KeyFrame *keyp, KeyFrame *keyr);		
		void  clear();
		void  destroy( void );
		void  render( void );
		void  advanceTo( float time );
		void  initialize( void );
		void  debug( void );
		int   save_txt_chunk(FILE *f);
		int   load_txt_chunk(FILE *f);
		int   save_bin_chunk(FILE *f);
		int   load_bin_chunk(FILE *f);
		int   loadFromMs3dAsciiSegment( FILE *f );
};

   

/////////////////////////////////////////////////////////////////////////////

class BONES3D{
    
    public:    
        int     NumFrames;   // length of animation (= Frames)
		int		num_bones;   //num bones 
		BONE3D	*bones;      //bones array
		int     num_atach;   //num atached bones
		BONES3D **atach;     //array pointers to atached bones
		
		BONES3D();
        ~BONES3D();
        
        void copy_from_model(Model *model);
        void clear();
        void destroy();
        bool link( void );
        void initialize( void );
        void render( void );
        void render_atach( void ); 
        void advanceAnimation( float CurrentTime );
        void advanceAnimation_atach( float CurrentTime );        
        void debug( void ); 
        int  save_txt_chunk(FILE *f);
        int  load_txt_chunk(FILE *f);
        int  save_bin_chunk(FILE *f);
        int  load_bin_chunk(FILE *f);
        int  save_txt(char *filename);
        int  load_txt(char *filename);
        int  save_bin(char *filename);
        int  load_bin(char *filename);
        int  loadFromMs3dAsciiSegment( FILE *f, char *szLine );
};

#endif          /* ifndef _BONES_H_ */
	
