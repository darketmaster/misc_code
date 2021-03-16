#include <stdio.h>
#include <math.h>

#include "allegro.h"
#include "alleggl.h"
#include <GL/glu.h>

#include "initgl.h"
#include "log.h"
#include "timers.h"
#include "3ddef.h"
#include "camera.h"
#include "mdf.h"
#include "models.h"
#include "3ds.h"
#include "textout.h"
#include "input.h"
#include "Frustum.h"


class PARTICLE{
public:

CVector3 pos;
float size;
GLubyte rgba[4];
  
  PARTICLE(){ size = 2; memset(rgba, 0, sizeof(GLubyte)); }
  ~PARTICLE(){}
  
  void init(CVector3 *p, GLubyte r, GLubyte g, GLubyte b)
  {
    pos = *p;
    rgba[0] = r;
    rgba[1] = g;
    rgba[2] = b;
  }

  void draw()
  {
  float tx1,ty1,tx2,ty2;
  
  tx1 = pos.x + size;
  tx2 = pos.x - size;
  ty1 = pos.y + size;
  ty2 = pos.y - size;
  
  glEnable(GL_BLEND);
  //glBlendFunc(GL_ONE_MINUS_SRC_COLOR, GL_ONE);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
  glDisable(GL_LIGHTING);
  
  glColor4ubv(rgba);
  glBegin(GL_TRIANGLE_STRIP);
  glTexCoord2d(1,1); 
  glVertex3f(tx1, ty1, pos.z);
  glTexCoord2d(0,1); 
  glVertex3f(tx2, ty1, pos.z);
  glTexCoord2d(1,0); 
  glVertex3f(tx1, ty2, pos.z);
  glTexCoord2d(0,0); 
  glVertex3f(tx2, ty2, pos.z);
  glEnd();
  }

};

class PARTICLES
{
  public:
  int num;
  
  PARTICLE *par;
  GLuint texture;
  
  
  void init(int n)
  {
     CVector3 temp;
     num = n;
     par = (PARTICLE*) new PARTICLE[num];
     
     texture = make_texture("data/gfx/flare.pcx", TLOW, TMASKED);
     
     for(int i=0; i<num; i++)
     {
         temp.init(32-rand()%64,32-rand()%64,32-rand()%64);
         par[i].init(&temp,rand()%255,rand()%255,rand()%255);
         par[i].rgba[3] = rand()%255;
     }
     
  }  
  void destroy()
  {
     if(par) delete [] par;
     num = 0;
     par = NULL;
  }  
  PARTICLES()
  {
     num = 0;
     par = NULL;
  }
  PARTICLES(int n)
  {
     init(n);
  }
  ~PARTICLES()
  {
     destroy();
     glDeleteTextures(1, &texture);
  }
  void draw()
  {
     glEnable (GL_TEXTURE_2D);   
     glBindTexture(GL_TEXTURE_2D,  texture);
     for(int i=0; i<num; i++)
     {
         par[i].draw();
     }     
  }
  void process()
  {
     for(int i=0; i<num; i++)
     {
        par[i].rgba[3]--;
        if(par[i].rgba[3]<10)
        {
         CVector3 temp;
         temp.init(32-rand()%64,32-rand()%64,32-rand()%64);
         par[i].init(&temp,rand()%255,rand()%255,rand()%255);
         par[i].rgba[3] = rand()%255;
        }   
     }     
  } 
       
    
};






void init_light()
{
 GLfloat LightAmbient[]=	{ 0.5f, 0.5f, 0.5f, 1.0f };
 GLfloat LightDiffuse[]=	{ 1.0f, 1.0f, 1.0f, 1.0f };
 GLfloat LightPosition[]=	{ 0.0f, 20.0f, 0.0f, 1.0f };

 glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
 glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
 glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
 glEnable(GL_LIGHT1);
}

int counter_c = 0;
int salir = 0;

#define fontname "data/fonts/refont.pcx"
#define fontname2 "data/fonts/castle8x8.pcx"

///                    U,    D,    L,    R,   b1,   b2,   b3,   b4,   b5,   b6,   b7,   b8,   b9,  b10,  b11,   b12
int Kmapdef[16] = {KEY_W,KEY_S,KEY_A,KEY_D,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,KEY_O,KEY_P,KEY_F,KEY_G,KEY_H, KEY_J};
int Kpaddef[16] = {    0,    0,    0,    0,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,    11};

//Gamepaths gamepaths;

void loading_screen()
{
  int salir=0;
  
  GLuint fonttex;
  GLuint fontbase;
  
  fonttex = make_texture(fontname, TLOW, TMASKED);  
  fontbase = init_cfont(0.9f); 

  CAMERA camera(SCREEN_W, SCREEN_H, 48, 1, 0, 0, 20, 0, 0, 0);
//////////////////////////////

  //while(!salir)
  {
/////////////////////////////////////   
    if(keypressed())
    {
       salir = 1;   
    }//end if keypressed
    
	glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
   
    camera.set_camera();    
    
    ////////////////
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);  
    glDisable(GL_DEPTH_TEST); 

    OrthoMode(0, 0, SCREEN_W, SCREEN_H);   
    glEnable (GL_TEXTURE_2D);   
	glBindTexture(GL_TEXTURE_2D,  fonttex);	
	glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor3ub(255, 255, 255);
	{
	int posy,posx;
	int tam=32;
	char text[] = "LOADING...";
    posx = (SCREEN_W - (strlen(text)*tam))/2;
    posy = SCREEN_H/2;   
    print_text(fontbase, tam, posx, posy, text);   
    posy+=tam;
    }

    PerspectiveMode();
////////////////////////////////////////////////////////////////////////////  
      
	glFlush();
	
	allegro_gl_flip();
/////////////////////////////////
	
}//end while(!salir)

  glDeleteTextures(1, &fonttex);
  destroy_cfont(fontbase);
/* 
  MDF_OBJ mdfobj;
  mdfobj.import_md2("data/models/bolt/tris.md2");
  mdfobj.debug_mdf_obj();*/
}

#define modelname1 "data/models/bolt/tris.md2"
#define pcxname1 "data/models/bolt/hard.pcx"

//#define modelname1 "data/models/alucard/tris.md2"
//#define pcxname1 "data/models/alucard/texture.pcx"

int   g_shade	  = GL_SMOOTH;
int   g_ViewMode  = GL_TRIANGLES;
bool  g_bLighting = true;
bool  g_bTextured = true;	
bool  g_bOutline  = false;
bool  g_bbbox     = false;
bool  g_breflex   = false;

int  draw_mode = 0;

void example()
{
  GLuint fonttex;
  GLuint fontbase;
  GLuint tex;
  int keytemp;
  float tanim;
  float g_RotateX = -90, g_RotationSpeed = 0;
  
  int b_mfrus;
  float angle, x, y, z;
  
  PARTICLES particles;
  particles.init(100);
  
  ANIMATION animdata;  
  animdata.load("data/bolt.txt");  
  STANIMATION myani;
  myani.create(8, &animdata);
  myani.set_canim(0); 
  
  tex = make_texture(pcxname1, TNORM, TNOMASKED); 
  MDF_OBJ mdfobj;
  mdfobj.import_md2(modelname1);
  
  INTERFACE_INPUT myinputs;  
  myinputs.create(2);
  myinputs.add(0, KEYBOARD_PAD, 14,Kmapdef, 0, Kpaddef);
  myinputs.add(1, GAMEPAD_ONLY, 14,Kmapdef, 1, Kpaddef);
  
  fonttex = make_texture(fontname2, TLOW, TMASKED); 
  fontbase = init_cfont(1.0f); 
  
  CAMERA camera(SCREEN_W, SCREEN_H, 48, 1, 0, 0, 64, 0, 0, 0);
  //camera.set_offsets(0, 0);  
  
  mdfobj.calculate_interpoled(myani.get_ckanim(), myani.get_nkanim(), tanim, 1);
//////////////////////////////

  while(!salir)
  {
  //if(counter_c+1 < decs)
  {
     //counter_c = decs;
     
     if(counter_c+1 < decs)
     {
        counter_c = decs;
        myinputs.update_inputs();
        particles.process();
     }
     
/////////////////////////////////////   
    if(keypressed())
    {

       keytemp = readkey()>>8;
       switch(keytemp)
       {
         case KEY_ESC:
            salir = 1;
         break;
         case KEY_SPACE:
       		 myani.set_canim(myani.canim+1);
         break;
         case KEY_F1:
       		 if(g_ViewMode == GL_TRIANGLES) {
			    g_ViewMode = GL_LINE_STRIP;
	      	 } else {
			   g_ViewMode = GL_TRIANGLES;
		     }
         break;
         case KEY_F2:
         	 g_bTextured = !g_bTextured;
         break;
         case KEY_F3:
      		 if(g_shade == GL_SMOOTH) {
			    g_shade = GL_FLAT;
	      	 } else {
			   g_shade = GL_SMOOTH;
		     }
             glShadeModel(g_shade);
         break;
         case KEY_F4:
         	 g_bLighting = !g_bLighting;
         break;
         case KEY_F5:
         	 g_bOutline = !g_bOutline;
         break;
         case KEY_F6:
         	 g_bbbox = !g_bbbox;
         break;
         case KEY_9:
       		 g_RotationSpeed += 0.1f;
         break;
         case KEY_0:
       		 g_RotationSpeed -= 0.1f;
         break;
         case KEY_ENTER:
       		 draw_mode++;
       		 if(draw_mode>3) draw_mode = 0;
         break;
       }//end switch(keytemp)       
       camera.process_input();    
    }//end if keypressed    
    
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    camera.set_camera();
    g_Frustum.CalculateFrustum();
    
    tanim = myani.Animate();
    mdfobj.calculate_interpoled(myani.get_ckanim(), myani.get_nkanim(), tanim, 1);
 
    ////draw
    //////////////////////////////////////////////////////////
    
    if(g_bLighting) {	
       glEnable(GL_LIGHTING);
    } else {
    glDisable(GL_LIGHTING);
    }
    if(g_bTextured) glEnable (GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);
    
       
    glBindTexture (GL_TEXTURE_2D, tex);
	glColor3ub(255, 255, 255);	
	 
	glPushMatrix();   
	
    glRotatef(g_RotateX, 0.0, 1.0f, 0.0);
    
    //glBegin(g_ViewMode); mdfobj.render_frame(0);	  
    //glBegin(g_ViewMode);mdfobj.render_flat();
    //glBegin(g_ViewMode);mdfobj.render_smooth();
    //glBegin(g_ViewMode);glTranslatef(0.0,0.0,40.0); 
    //glBegin(g_ViewMode);mdfobj.gl_render_flat(); 
    //glBegin(g_ViewMode);mdfobj.gl_render_smooth();
    
    //glTranslatef(0.0,0.0,32.0);
    b_mfrus = g_Frustum.BoxInFrustum( mdfobj.tbbox.min.x, mdfobj.tbbox.min.y, mdfobj.tbbox.min.z, 
              mdfobj.tbbox.max.x, mdfobj.tbbox.max.y, mdfobj.tbbox.max.z );
    if(b_mfrus)
    switch(draw_mode)
    {
       case 0: glBegin(g_ViewMode);mdfobj.render_flat();
       break;
       case 1: mdfobj.gl_render_flat(); 
       break;
       case 2: glBegin(g_ViewMode);mdfobj.render_smooth();
       break;
       case 3: mdfobj.gl_render_smooth();
       break;
    }
      
    if(g_bbbox)mdfobj.tbbox.render();    
    //mdfobj.draw_normasl();
    
    glPopMatrix();
    
    glPushMatrix();
    {
    
    allegro_gl_quat_to_glrotatef (&camera.orientation, &angle, &x, &y, &z) ;
    if(_isnan(angle)) angle = 0.0;
    glRotatef(-angle, x, y, z);
    particles.draw();
    }
    glPopMatrix();                  
    
              
    g_RotateX += g_RotationSpeed;               
    //////////////////////////////////////////////////////////                         
    chkfps_polycount();    
    ////////////////text out//////////////////////////////////
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);  
    glDisable(GL_DEPTH_TEST); 

    OrthoMode(0, 0, SCREEN_W, SCREEN_H);   
    glEnable (GL_TEXTURE_2D);   
	glBindTexture(GL_TEXTURE_2D,  fonttex);	
	glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    glColor3ub(100, 100, 255);
	{
	int posy=0;
	int tam=8;
	int *pt;
   
    print_text(fontbase, tam, 0, posy, "Light:%d Mod:%d Tex:%d Olin:%d Bbox:%d", 
               g_bLighting, g_ViewMode, g_bTextured, g_bOutline, g_bbbox);
    posy+=tam;
    print_text(fontbase, tam, 0, posy, "FPS: %.2f t:%.2f pps:%d ppf :%.2f", fps, tanim, n_poly_sec, float(n_poly_sec)/float(fps));   
    posy+=tam;
    print_text(fontbase, tam, 0, posy, "fonttex:%d base:%d", fonttex, fontbase);   
    posy+=tam;
    print_text(fontbase, tam, 0, posy, "Num_ani: %d curf:%d nextf:%d", 
               myani.pinfo->animation[myani.canim].n_anim, myani.currkanim, myani.nextkanim);
    posy+=tam;
    print_text(fontbase, tam, 0, posy, "Ani:%d %s Cf:%d Nf:%d",
               myani.canim,myani.pinfo->animation[myani.canim].name, myani.get_ckanim(), myani.get_nkanim());
    posy+=tam;
    print_text(fontbase, tam, 0, posy, "bbox: (%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f)", mdfobj.tbbox.min.x, mdfobj.tbbox.min.y, mdfobj.tbbox.min.z,
               mdfobj.tbbox.max.x, mdfobj.tbbox.max.y, mdfobj.tbbox.max.z);
    posy+=tam;
    for(int nc=0; nc<myinputs.num_inputs; nc++)
    {
      pt = myinputs.input[nc].tb;
      print_text(fontbase, tam, 0, posy, "input: %05d", myinputs.input[nc].b);
      posy+=tam;    
      print_text(fontbase, tam, 0, posy, "butt: %03d %03d %03d %03d %03d %03d %03d %03d", 
                 pt[0],pt[1],pt[2],pt[3], pt[4],pt[5],pt[6],pt[7]);
      posy+=tam;
      print_text(fontbase, tam, 0, posy, "butt: %03d %03d %03d %03d %03d %03d %03d %03d",
                 pt[8],pt[9],pt[10],pt[11], pt[12],pt[13],pt[14],pt[15]);
    }//en for
    posy+=tam;
    print_text(fontbase, tam, 0, posy, "DRAWMODE: %d b_frus:%d ang:%f",draw_mode,b_mfrus,angle);
    }
 

    PerspectiveMode();  
   
////////////////////////////////////////////////////////////////////////////  
	glScalef(1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);	
      
	glFlush();	
	allegro_gl_flip();
/////////////////////////////////    
   }//end if()	
 }//end while(!salir)

 destroy_cfont(fontbase);
}




int main(void)
{
   INIT_ENVIROMENT env("debug.log");
   
   srand(time(NULL));   

   glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
   glEnable(GL_LIGHT0);								// Turn on a light with defaults set
   //init_light();
   glEnable(GL_LIGHTING);							// Turn on lighting
   glEnable(GL_COLOR_MATERIAL);						// Allow color
   
   glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
   glEnable (GL_DEPTH_TEST);
   
   glDepthFunc (GL_LESS);
   glEnable (GL_CULL_FACE);
   glCullFace (GL_BACK);   
   glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
   glShadeModel(GL_SMOOTH);
      
   allegro_gl_use_mipmapping(TRUE);//allegro_gl_use_mipmapping(FALSE);//   
   allegro_gl_set_texture_format(GL_RGB);
   
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);   
   //////////////////////////////////  
   
   install_tick();   
   ///////////////////////
   loading_screen();
   example();
   //////////////////////
   remove_tick();
  
    return 0;
}
END_OF_MAIN();


