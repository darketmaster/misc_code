#include "allegro.h"
#include "spr.h"
#define NAME_INI "ini.ini"

#define STR_VERSION "sprcreator v0.1 beta"

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

   set_color_depth(16);
   if(windowed) mode = GFX_AUTODETECT_WINDOWED;

   if (set_gfx_mode(mode, sw, sh, 0, 0) != 0) {
       set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
       allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
       gfxerror=1;
   }

   return gfxerror;
}

void rotatepal(RGB *pal)
{
  RGB temp;
  int i;
  temp = pal[0];
  for(i=1;i<16;i++)
      pal[i-1]=pal[i];
  pal[15] = temp;
}

void rotatepalint(int *pal)
{
  int temp;
  int i;
  temp = pal[0];
  for(i=1;i<16;i++)
      pal[i-1]=pal[i];
  pal[15] = temp;
}

void rotatepalint_horse(int *pal)
{
 int temp;
 temp = pal[2];
 pal[2] = pal[7];
 pal[7] = pal[8];
 pal[8] = pal[9];
 pal[9] = temp;
}

int main()
{
   BITMAP *buffer;
   Spr *spr;
   BITMAP *dibujo;
   RGB pal[16];
   int palint[16];

   //save_config(NAME_INI);
   if(ini_liballeg())
      return 1;

   /* allocate the memory buffer */
   buffer = create_bitmap(SCREEN_W, SCREEN_H);
   text_mode(-1);

   spr = load_Spr("spr.spr");
   load_pallete_16_RGB(pal,"pal.txt");
   load_pallete_16_int(palint,"intpal.txt");
   dibujo = get_bmp_spr(spr,pal);
   //convert_RGB_to_int_16(pal,palint);

   while (!keypressed()) {
      clear_to_color(buffer, 255);

      blit_4_in_16_int(spr,dibujo,palint);
      blit(dibujo, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
      rotatepalint_horse(palint);

      blit_4_in_16_RGB(spr,dibujo,pal);
      blit(dibujo, buffer, 0, 0, dibujo->w, 0, SCREEN_W, SCREEN_H);
      rotatepal(pal);

      textprintf(buffer, font, 0, 0, 0, "Double buffered (%s)",gfx_driver->name);
      blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   }

   destroy_bitmap(dibujo);
   destroy_spr(spr);
   destroy_bitmap(buffer);

   set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
   allegro_message("version: %s",STR_VERSION);

   return 0;
}END_OF_MAIN();
