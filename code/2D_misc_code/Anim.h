/*
  Name:      ANIM.H 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  ANIM ROUTINES
*/

#ifndef __ANIM_H_
#define __ANIM_H_

#include <stdlib.h>
#include <stdio.h>
#include "allegro.h"

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

/////////////////////anim rutines//////////////////////////////////////////////////////////

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
 char *get_namekanim();
 
};


/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __ANIM_H_
