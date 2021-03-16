/*
  Name:      INITVAR.CPP 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  INITIALIZATION ROUTINES
*/
#include "initvar.h"
#include "timers.h"
#include "filelog.h"

INIT_ENVIROMENT::INIT_ENVIROMENT(char *filename)
{
 int dv,dm,flag;
 
 create_file_log(filename);
 add_in_log("INIT ALL\n");
 add_separator_in_log();
 
 add_in_log("initializing allegro.....[%s]\n",allegro_init()?"FAIL":"OK");          
 add_in_log("installing keyboard......[%s]\n",install_keyboard()?"FAIL":"OK");
 add_in_log("installing timer.........[%s]\n",install_timer()?"FAIL":"OK"); 
 add_in_log("installing mouse.........[%d buttons]\n",install_mouse());  
 add_in_log("installing joystick.....[%s] %d detected\n",install_joystick(JOY_TYPE_AUTODETECT)?"FAIL":"OK",num_joysticks);  
 
 set_config_file("config.ini");
 override_config_file( "config.ini" );
 mode=get_config_int ( "VIDEO" , "DRIVER" , GFX_AUTODETECT ) ;
 bpp=get_config_int ( "VIDEO" , "BPP" , 16 ) ;
 w=get_config_int ( "VIDEO" , "W" , 400 ) ;
 h=get_config_int ( "VIDEO" , "H" , 300 ) ;
 idig=get_config_int ( "AUDIO" , "DIGI_DRIVER" , DIGI_NONE ) ;
 imid=get_config_int ( "AUDIO" , "MIDI_DRIVER" , MIDI_NONE ) ;
 
 add_separator_in_log();
 add_in_log("variables video : %d\n",mode);
 add_in_log("variables bpp   : %d\n",bpp);
 add_in_log("variables W     : %d\n",w);
 add_in_log("variables h     : %d\n",h);
 add_in_log("variables Audio : %d\n",idig);
 add_in_log("variables midi  : %d\n",imid);
 
 set_color_depth(bpp);
 flag = set_gfx_mode(mode, w, h, 0, 0);
 add_in_log("installing gfx..........[%s]\n",flag?"FAIL":"OK"); 
 if (flag!= 0)
     allegro_message("Error initialising gfx system\n%s\n", allegro_error);
     
 this->blitting_mode(MODE_DOUBLEBUFFER);//|BUFFER_VIDEO);
  
 add_in_log("creating colors....\n"); 
 //create_colors();
 text_mode(-1);

 dv = detect_digi_driver(idig);
 dm = detect_midi_driver(imid);
 if(dv==0) idig=DIGI_NONE;
 if(dm==0) imid=MIDI_NONE;
 flag = install_sound(idig, imid, NULL);
 add_in_log("installing sound system...[%s]\n",flag?"FAIL":"OK"); 
 if(flag)
    allegro_message("Error initialising sound system\n%s\n", allegro_error);
    
 add_in_log("init completed.\n");
 add_separator_in_log();
 
 set_color_conversion(COLORCONV_NONE);
  
}


INIT_ENVIROMENT::~INIT_ENVIROMENT()
{
 add_separator_in_log();
 this->destroy_video_buffers();
 add_in_log("shutdown allegro.....\n"); 
 allegro_exit();
}

void INIT_ENVIROMENT::destroy_video_buffers()
{
 int i;
 if((modeblit&MODE_PAGEFLIPING)==0)
 {
     add_in_log("destroy buffer bitmap in %X\n",buffer); 
     destroy_bitmap(buffer);
 }
 else
 {
     for(i=0;i<2;i++)
     {
        add_in_log("destroy video page %d bitmap in %X\n",i,page[i]);
        destroy_bitmap(page[i]);
     }
 }
}

void INIT_ENVIROMENT::blitting_mode(int type)
{
 modeblit = type;
 active_page = 0;
 
 add_separator_in_log();
 
 if((modeblit&MODE_PAGEFLIPING)==0)
 { 
     add_in_log("creating buffer....\n");
     if(modeblit&BUFFER_VIDEO)     
        buffer = create_video_bitmap(SCREEN_W,SCREEN_H);
     else buffer = create_bitmap(SCREEN_W,SCREEN_H);
     add_in_log("buffer in %X\n",buffer); 
 }
 else 
 {
     add_in_log("creating video page 0....\n");
     page[0] = create_video_bitmap(SCREEN_W,SCREEN_H);
     add_in_log("page 0 in %X\n",page[0]);
     add_in_log("creating video page 1....\n");
     page[1] = create_video_bitmap(SCREEN_W,SCREEN_H);
     add_in_log("page 1 in %X\n",page[1]);
     buffer = page[active_page];
 }
}

void INIT_ENVIROMENT::blit_buffer()
{
   acquire_screen();
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   release_screen();  
}

void INIT_ENVIROMENT::page_fliping()
{
     show_video_bitmap(buffer);     
     active_page = (active_page + 1) & 1;
     buffer = page[active_page];   
}

void INIT_ENVIROMENT::update_blit()
{
  if((modeblit&MODE_PAGEFLIPING))
     this->page_fliping();
  else this->blit_buffer();
}
