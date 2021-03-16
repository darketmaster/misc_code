#include "initgl.h"
#include "timers.h"
#include "log.h"

INIT_ENVIROMENT::INIT_ENVIROMENT(char *filename)
{
 int dv,dm,flag;
 
 create_file_log(filename);
 add_in_log("INIT ALL\n");
 add_separator_in_log();
 
 add_in_log("initializing allegro.....[%s]\n",allegro_init()?"FAIL":"OK");  
 add_in_log("initializing allegrogl...[%s]\n",install_allegro_gl()?"FAIL":"OK");                                 
 add_in_log("installing keyboard......[%s]\n",install_keyboard()?"FAIL":"OK");
 add_in_log("installing timer.........[%s]\n",install_timer()?"FAIL":"OK"); 
 add_in_log("installing mouse.........[%d buttons]\n",install_mouse());  
 add_in_log("installing joystick.....[%s] %d detected\n",install_joystick(JOY_TYPE_AUTODETECT)?"FAIL":"OK",num_joysticks);  
 
 //set_config_file("config.ini");
 override_config_file( "config.ini" );
 //mode=get_config_int ( "VIDEO" , "DRIVER" , GFX_OPENGL ) ;
 mode = GFX_OPENGL;
 bpp =get_config_int ( "VIDEO" , "BPP" , 16 ) ;
 zbpp =get_config_int ( "VIDEO" , "ZBUFFER" , 8 ) ;
 ffullscreen = get_config_int ( "VIDEO" , "FULLSCREEN" , TRUE ) ;
 w = get_config_int ( "VIDEO" , "W" , 640 ) ;
 h = get_config_int ( "VIDEO" , "H" , 480 ) ;
 idig = get_config_int ( "AUDIO" , "DIGI_DRIVER" , DIGI_NONE ) ;
 imid = get_config_int ( "AUDIO" , "MIDI_DRIVER" , MIDI_NONE ) ;
 
 add_separator_in_log();
 add_in_log("variables video : %d\n",mode);
 add_in_log("variables FULLSCREEN: %d\n",ffullscreen);
 add_in_log("variables bpp   : %d\n",bpp);
 add_in_log("variables zbpp   : %d\n",zbpp);
 add_in_log("variables W     : %d\n",w);
 add_in_log("variables h     : %d\n",h);
 add_in_log("variables Audio : %d\n",idig);
 add_in_log("variables midi  : %d\n",imid); 
 
 /* Set up AllegroGL */
 allegro_gl_clear_settings();
 allegro_gl_set(AGL_COLOR_DEPTH, bpp);
 allegro_gl_set(AGL_Z_DEPTH, zbpp);
 allegro_gl_set(AGL_FULLSCREEN, ffullscreen);
 allegro_gl_set(AGL_DOUBLEBUFFER, 1);
 allegro_gl_set(AGL_RENDERMETHOD, 1);
 allegro_gl_set (AGL_STENCIL_DEPTH, 0);
 allegro_gl_set(AGL_SUGGEST, AGL_COLOR_DEPTH | AGL_DOUBLEBUFFER | AGL_RENDERMETHOD
		       | AGL_Z_DEPTH | AGL_WINDOWED | AGL_STENCIL_DEPTH);//AGL_WINDOWED AGL_FULLSCREEN
		       
 add_in_log("NOW installing gfx..........\n");
 set_color_depth(bpp);
 flag = set_gfx_mode(mode, w, h, 0, 0);
 add_in_log("installing gfx..........[%s]\n",flag?"FAIL":"OK"); 
 if (flag!= 0)
     allegro_message ("Error setting OpenGL graphics mode:\n%s\nAllegro GL error : %s\n", allegro_error, allegro_gl_error);
  
 //allegro_gl_screen_mode (AGL_MODE_DIRECT); 
  
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
}


INIT_ENVIROMENT::~INIT_ENVIROMENT()
{
 add_separator_in_log();
 add_in_log("shutdown allegro.....\n"); 
 allegro_exit();
}

void INIT_ENVIROMENT::update_blit()
{
  allegro_gl_flip();
}
