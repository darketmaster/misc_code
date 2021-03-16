#include "allegro.h"
#include "spr.h"
#define NAME_INI "ini.ini"

#define STR_VERSION "bitmap to spr conversor v0.1 beta"

int sw=640,sh=480;
int windowed = 1;

int load_config(char *fname)
{
 FILE *f;

 f=fopen(fname,"r");
 if (f==NULL) return 1;

 fscanf(f,"W:%d H:%d\n",&sw,&sh);
 fscanf(f,"Windowed = %d",&windowed);

 fclose(f);
 return 0;
}

void save_config(char *fname)
{
 FILE *f;

 f=fopen(fname,"w");
 if(f)
 {
 fprintf(f,"W:%d H:%d\n",sw,sh);
 fprintf(f,"Windowed = %d",windowed);

 fclose(f);
 }//end if f
}

int ini_liballeg()
{
   int gfxerror=0;
   int mode = GFX_AUTODETECT;
   allegro_init();
   install_timer();
   install_mouse();
   install_keyboard();

  if(load_config(NAME_INI))
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("ini no found \n tamaño estandar 640x480");
   }

   set_color_depth(8);
   if(windowed) mode = GFX_AUTODETECT_WINDOWED;

   if (set_gfx_mode(mode, sw, sh, 0, 0) != 0) {
       set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
       allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
       gfxerror=1;
   }

   return gfxerror;
}

int main()
{
   BITMAP *buffer;
   Spr *spr;
   BITMAP *dibujo;
   PALLETE pal;
   int palint[16];
   char filename[128]="";
   char fname[30];
   char cad[128];
   char *ptr;

   //save_config(NAME_INI);
   if(ini_liballeg())
      return 1;

   /* allocate the memory buffer */
   buffer = create_bitmap(SCREEN_W, SCREEN_H);
   text_mode(-1);
    select_palette(desktop_pallete);

   if(file_select_ex("select bitmap to convert", filename, "BMP;PCX;TGA", 127, sw,sh))
   {
      dibujo = load_bitmap(filename,pal);
      select_palette(pal);
/*
      while (!keypressed()) {
             clear_to_color(buffer, 255);
             blit(dibujo, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
             textprintf(buffer, font, 0, 0, rand()%256, "press key to exit..");
             blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
     }*/

     sprintf(fname,get_filename(filename));
     ptr = get_extension(fname);
     ptr --;
     *ptr = '\0';

     spr=get_spr(dibujo);
     sprintf(cad,"%s.txt",fname);
     save_Spr_txt(spr,cad);
     sprintf(cad,"%s.spr",fname);
     save_Spr(spr,cad);

     sprintf(cad,"%s_pal.txt",fname);
     save_pallete_16_RGB(pal,cad);
     convert_RGB_to_int_16(pal,palint);
     sprintf(cad,"%s_palint.txt",fname);
     save_pallete_16_int(palint,cad);

     destroy_bitmap(dibujo);
     destroy_spr(spr);
   }

   destroy_bitmap(buffer);
   set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
   allegro_message("version: %s",STR_VERSION);

   return 0;
}END_OF_MAIN();
