/*
  Name:      ANIM.CPP 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  ANIM ROUTINES
*/

#include "anim.h"
#include "filelog.h"
#include "timers.h"
#include <string.h>
#include <ctype.h>

///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//                           _anim
///////////////////////////////////////////////////////////////////////////

_anim::_anim()
{ 
 name[0] = '\0'; 
 n_anim = 0;
 keyframe = NULL;
}

_anim::_anim(char *name,int tam)
{
  create(name, tam);
} 

_anim::~_anim()
{
  destroy();
}
 
void _anim::destroy()
{
   if(n_anim>0) 
      if(keyframe) 
         delete [] keyframe;
   name[0] = '\0'; 
   n_anim = 0;
   keyframe = NULL;
}
 
void _anim::create(char *name,int tam)
{
 strcpy(this->name,name); 
 n_anim = tam;
 if(n_anim>0)
    keyframe = (short*)new short[n_anim];
 else keyframe = NULL;
}
 
void _anim::set(int n, short data)
{
   if((n_anim>0) && (n<n_anim))
      if(keyframe) keyframe[n] = data;           
}

void _anim::load(FILE *fp)
{
    char line[256];
    char *p;
    int stop=0;
    int c,n;
    char tname[64] = "";
    
   	p = fgets(line, sizeof(line), fp);
	while(p != NULL && !stop)
	{

		if(strncmp(p, "NAME= \"", strlen("NAME= \"")) == 0 )
		{
			p+=7;
			c=0;
			while(*p != '\n' && *p != '"' && c<63)
			{
			   tname[c] = *p;
			   p++;
			   c++;
			}			
			tname[c] = '\0';			
		}
		else		
		if(strncmp(p, "N_FRAMES= ", strlen("N_FRAMES= ")) == 0 )
		{
			p+=10;
			while(*p == ' ') p++;			
			if(isdigit(*p))
            {			
			   sscanf(p, "%d", &n);			   
            }
            if (n<0) n = 0;			
            create(tname,n);
		}
		else
		if(strncmp(p, "LIST_BEGIN.", strlen("LIST_BEGIN.")) == 0 )
		{
		   int l = strlen("LIST_END.");		   
		   p = fgets(line, sizeof(line), fp);
		   c = 0;  //add_in_log("LIST_BEGIN localized\n");
           while(p != NULL && strncmp(p, "LIST_END.", l))
           {
              
              while(*p != '\n' && c < n_anim)
              {
                while(*p == ' ') p++;
                
                if(isdigit(*p))
                {
                   sscanf(p, "%d", &n);   //add_in_log("line p: %s\n", p); //add_in_log("scan data %d: %d\n",c,n);
                   set(c, (short)n);
                   c++;
                }
                while(*p != ' ' && *p != '\n') p++;
                
              }
              p = fgets(line, sizeof(line), fp);              
           }   //if(strncmp(p, "LIST_END.", l)==0) add_in_log("LIST_END localized\n");           
           stop = 1;
		}
	    p = fgets(line, sizeof(line), fp);	
	}
}

void _anim::save(FILE *fp)
{
  int c;
  fprintf(fp,"NAME= \"%s\"\n", name);
  fprintf(fp,"N_FRAMES= %d\n", n_anim);
  
  fprintf(fp,"LIST_BEGIN.\n"); 
  for(c=0; c<n_anim; c++)
  {
    fprintf(fp,"%d ", keyframe[c]); 
  }
  fprintf(fp,"\nLIST_END.\n\n");     
}

void _anim::debug()
{
  int c;
  add_in_log("ANIMATION NAME [%s]\n", name);
  add_in_log("n_anim: %d\n", n_anim);
  
  add_in_log("keyframe list %X:\n", keyframe); 
  for(c=0; c<n_anim; c++)
  {
    add_in_log("%d ", keyframe[c]); 
  }
  add_in_log("\n");           
}

///////////////////////////////////////////////////////////////////////////
//                           ANIMATION
///////////////////////////////////////////////////////////////////////////
ANIMATION::ANIMATION()
{
  n_animations = 0; 
  animation = NULL;
}

ANIMATION::~ANIMATION()
{   
   if(n_animations>0) 
      if(animation) 
         free(animation);//delete [] animation; //i use free for use realloc
   n_animations = 0;
   animation = NULL;
}

void ANIMATION::load(char *filename)
{
    char line[256];
    char *p;
    FILE *fp;
    
    fp = fopen(filename,"rt");
    
    if(fp)
    {
    
   	   p = fgets(line, sizeof(line), fp);
	   while(p != NULL)
	   {
		 if(strncmp(p, "ANIMATION.", strlen("ANIMATION.")) == 0 )
		 {
		    animation = (_anim*) realloc(animation, sizeof(_anim)*(n_animations+1));
            animation[n_animations].load(fp);
            n_animations++;
		 }
	     p = fgets(line, sizeof(line), fp);	
	   }
	   fclose(fp);
   }//end if
}

void ANIMATION::save(char *filename)
{
    FILE *fp;
    int c;
    
    fp = fopen(filename,"wt");
    
    if(fp)
    {
		    for( c=0; c<n_animations; c++)
		    {
		       fprintf(fp, "ANIMATION.\n");
		       animation[c].save(fp);
            }
	   fclose(fp);
    }//end if
}


///////////////////////////////////////////////////////////////////////////
//                           STANIMATION
///////////////////////////////////////////////////////////////////////////

STANIMATION::STANIMATION()
{
 create(1000, NULL);
 canim = -1; 
}

STANIMATION::~STANIMATION()
{
}
 
void STANIMATION::create(int _velanim, ANIMATION *_pinfo)
{
 velanim = _velanim;
 anim_per = 1000.0f / (float(velanim)); 
 pinfo = _pinfo; 
 canim = 0; 
 currkanim = 0;
 nextkanim = 0; 
 elapsedTime = 0.0f;
 lastTime = 0.0f;
} 

void STANIMATION::set_vel(int _velanim)
{
 velanim = _velanim;
 anim_per = 1000.0f / (float(velanim)); 
}

void STANIMATION::set_canim(int _canim)
{
 if(_canim < pinfo->n_animations)
    canim = _canim;
 else canim = 0;
 currkanim = 0;
 nextkanim = 0; 
}

float STANIMATION::Animate()
{

	float time,t;
	
    time = decs; 
    nextkanim = (currkanim + 1) % pinfo->animation[canim].n_anim;	
	elapsedTime = time - lastTime;
    t = elapsedTime / anim_per; 
     
	if (elapsedTime >= anim_per )
	{
		currkanim = nextkanim;
		lastTime = time;
	}    
	return t;
}

int STANIMATION::get_ckanim()
{ 
   int ret = 0;
   if(pinfo) if(pinfo->animation[canim].keyframe)
        ret = (int)pinfo->animation[canim].keyframe[currkanim];
   return ret;
}

int STANIMATION::get_nkanim()
{
   int ret = 0;
   if(pinfo)  if(pinfo->animation[canim].keyframe)
        ret = (int)pinfo->animation[canim].keyframe[nextkanim];
   return ret;
}

char *STANIMATION::get_namekanim()
{
   return pinfo->animation[canim].name;
}
