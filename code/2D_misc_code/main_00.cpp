#include <stdio.h>
#include <math.h>

#include "allegro.h"
#include "initvar.h"
#include "filelog.h"
#include "timers.h"
#include "input.h"
#include "sprite.h"
#include "sload.h"
#include "ANIM.H"

///                    U,    D,    L,    R,   b1,   b2,   b3,   b4,   b5,   b6,   b7,   b8,   b9,  b10,  b11,   b12
int Kmapdef[16] = {KEY_W,KEY_S,KEY_A,KEY_D,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,KEY_O,KEY_P,KEY_F,KEY_G,KEY_H, KEY_J};
int Kpaddef[16] = {    0,    0,    0,    0,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,    11};

int Kmapdefp2[16] = {KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEY_I,KEY_O,KEY_P,KEY_J,KEY_K,KEY_L,KEY_B,KEY_N,KEY_M,KEY_G,KEY_H, KEY_J};


int counter_c = 0;

PROG_DATA procdat = {0, 100, 0, 0};

void progress_func()
{
   int x, y;
   int cprog;
   char cad[8]; 
   
   x = (SCREEN_W/2)-102;
   y = (SCREEN_H/2)-12;
   cprog = (int)((_prog_data->cur/100.0) * 200.0);
   
   clear(_prog_bmp);
   
   textout_centre(_prog_bmp, font, "LOADING...", SCREEN_W/2, SCREEN_H/2 - 30, makecol(255,255,255));
   
   rect(_prog_bmp, x, y, x+204, y+24, makecol(255,255,255)); 
   rect(_prog_bmp, x+1, y+1, x+203, y+23, makecol(128,128,128));
   
   rectfill(_prog_bmp, x+2, y+2, x+2+cprog, y+22, makecol(200,0,0));
   sprintf(cad, "%03d\%",_prog_data->cur);
   textout_centre(_prog_bmp, font, cad, SCREEN_W/2, SCREEN_H/2, makecol(255,255,255));
    
   acquire_screen();
   blit(_prog_bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   release_screen();
   //vsync();
   //int val;
   //val = readkey();
}

void continue_func()
{
   int x, y;
   int cprog;
   char cad[8]; 
   
   x = (SCREEN_W/2)-102;
   y = (SCREEN_H/2)-12;
   cprog = (int)((_prog_data->cur/100.0) * 200.0);
   
   clear(_prog_bmp);
   
   textout_centre(_prog_bmp, font, "LOADING...", SCREEN_W/2, SCREEN_H/2 - 30, makecol(255,255,255));
   
   rect(_prog_bmp, x, y, x+204, y+24, makecol(255,255,255)); 
   rect(_prog_bmp, x+1, y+1, x+203, y+23, makecol(128,128,128));
   
   rectfill(_prog_bmp, x+2, y+2, x+2+cprog, y+22, makecol(200,0,0));
   sprintf(cad, "%03d\%",_prog_data->cur);
   textout_centre(_prog_bmp, font, cad, SCREEN_W/2, SCREEN_H/2, makecol(255,255,255));
   textout_centre(_prog_bmp, font, "PRESS A KEY FOR CONTINUE...", SCREEN_W/2, SCREEN_H-24, makecol(255,255,255));
    
   acquire_screen();
   blit(_prog_bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   release_screen();
}

void example(INIT_ENVIROMENT  *env)
{
   float tanim;  
   ini_progress_proc(env->buffer, &procdat);
   progress_proc = progress_func;
   
   progress_proc();
   
   INTERFACE_INPUT myinputs; 
   myinputs.create(2);
   myinputs.add(0, KEYBOARD_PAD, 14,Kmapdef, 0, Kpaddef);
   myinputs.add(1, KEYBOARD_PAD, 14,Kmapdefp2, 1, Kpaddef);
   
   _prog_data->cur++;
   progress_proc();
      
   ANIMATION animdata;  
   animdata.load("data\\samus\\animation.txt");
   animdata.save("animation_debug.txt");
   
   STANIMATION myani;
   myani.create(120, &animdata);
   myani.set_canim(0); 
   
   _prog_data->cur = 10;
   progress_proc();
/*   
   SPRITES mysprites;
   mysprites.load("data\\ninja\\ninja.pcx", 5, 6);
   _prog_data->cur = 50;
   progress_proc();*/
   
   SPRITES samusspr;
   samusspr.load("data\\samus\\Gravitysuitsamus.pcx", 24, 21);
   _prog_data->cur = 99;
   progress_proc();
   
   _prog_data->cur = 100;
   continue_func();   
   while (!keypressed());
   
   add_in_log("---------------main_loop-------------------\n");
   while (!key[KEY_ESC])
   {   
    if(counter_c+1 < decs)
    {
     counter_c = decs;
     myinputs.update_inputs();
     
     if(myinputs.input[0].b)
     {
        if(GETBIT(myinputs.input[0].b,4) && !GETBIT(myinputs.input[0].lb,4))
        myani.set_canim(myani.canim+1);
        if(GETBIT(myinputs.input[0].b,5) && !GETBIT(myinputs.input[0].lb,5))
           if(myani.canim>0)
              myani.set_canim(myani.canim-1);
           else myani.set_canim(myani.pinfo->n_animations-1);
     }
     
     tanim = myani.Animate();
     
     clear(env->buffer);
     
     set_palette(samusspr.pal[0]);
     samusspr.draw(env->buffer, myani.get_ckanim(), SCREEN_W/2, SCREEN_H/2, 0);
     

     chkfps();
     textprintf(env->buffer, font,20, SCREEN_H-16, makecol(255, 0, 0), "%.2f fps, animation: %04d %s", fps, myani.get_ckanim(), myani.get_namekanim());
     
    int posy=0;
	int tam=8;
	int *pt;
     for(int nc=0; nc<myinputs.num_inputs; nc++)
     {
      pt = myinputs.input[nc].tb;
      textprintf(env->buffer, font, 0, posy, makecol(255, 100, 200), "input: %05d", myinputs.input[nc].b);
      posy+=tam;    
      textprintf(env->buffer, font, 0, posy, makecol(255, 100, 200), "butt: %03d %03d %03d %03d %03d %03d %03d %03d", 
                 pt[0],pt[1],pt[2],pt[3], pt[4],pt[5],pt[6],pt[7]);
      posy+=tam;
      textprintf(env->buffer, font, 0, posy, makecol(255, 100, 200), "butt: %03d %03d %03d %03d %03d %03d %03d %03d",
                 pt[8],pt[9],pt[10],pt[11], pt[12],pt[13],pt[14],pt[15]);
      posy+=tam;
    }//en for
     
     
     env->update_blit();
    }//end if counter c
   }
}


int main(void)
{
   srand(time(NULL));
   INIT_ENVIROMENT env("debug.log");   
   set_palette(desktop_palette);
   
   install_tick();   
   ///////////////////////
   example(&env);
   //////////////////////
   remove_tick(); 

   return 0;
}END_OF_MAIN();
