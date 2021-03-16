#include "allegro.h"
#include "spr.h"
#define NAME_INI "ini.ini"

#define STR_VERSION "sprcreator v0.1 beta"

int sw=640,sh=480;

int load_config(char *fname)
{
 FILE *f;

 f=fopen(fname,"r");
 if (f==NULL) return 1;

 fscanf(f,"W:%d H:%d",&sw,&sh);

 fclose(f);
 return 0;
}

void save_config(char *fname)
{
 FILE *f;

 f=fopen(fname,"w");
 if(f)
 {
 fprintf(f,"W:%d H:%d",sw,sh);

 fclose(f);
 }//end if f
}

int ini_liballeg()
{
   int gfxerror=0;
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
   if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, sw, sh, 0, 0) != 0) {
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

   //save_config(NAME_INI);
   if(ini_liballeg())
      return 1;

   /* allocate the memory buffer */
   buffer = create_bitmap(SCREEN_W, SCREEN_H);
   text_mode(-1);
   dibujo = load_bitmap("sprites.pcx",pal);

   select_palette(pal);

   while (!keypressed()) {
      clear_to_color(buffer, 255);
      blit(dibujo, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
      textprintf(buffer, font, 0, 0, 0, "Double buffered (%s)",gfx_driver->name);
      blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   }

   spr=get_spr(dibujo);
   destroy_bitmap(dibujo);
   save_Spr_txt(spr,"spr.txt");
   save_Spr(spr,"spr.spr");
   destroy_spr(spr);

   //spr = load_Spr_txt("spr.txt");
   spr = load_Spr("spr.spr");
   dibujo = get_bmp_spr(spr,pal);
   save_pcx("pcx.pcx",dibujo,pal);
   save_pallete_16_RGB(pal,"pal.txt");
   convert_RGB_to_int_16(pal,palint);
   save_pallete_16_int(palint,"intpal.txt");

   destroy_bitmap(dibujo);
   destroy_spr(spr);
   destroy_bitmap(buffer);

   set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
   allegro_message("version: %s",STR_VERSION);

   return 0;
}END_OF_MAIN();
