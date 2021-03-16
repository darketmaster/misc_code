#include "allegro.h"
#include "spr.h"
#define NAME_INI "ini.ini"

#define STR_VERSION "List_spr creator v0.1 beta"

#define MAXNAMES 1000

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

RGB defaulpal[16] = {
                     {63 , 0 ,63},{11 ,11 , 8},{ 0 ,25 , 0},{23 ,17 , 7},
                     {27 ,21 ,12},{52 , 9 , 0},{44 ,31 ,15},{ 0 ,37 , 0},
                     { 0 ,45 , 0},{ 0 ,63 , 0},{ 0 ,11 ,36},{ 0 ,25 ,51},
                     {39 ,39 ,39},{59 ,47 ,32},{60 ,53 ,43},{56 ,57 ,56}
                     };

int main()
{
   BITMAP *buffer;
   BITMAP *bmp=NULL;
   int salir=0;
   int tempkey;
   char cadtemp[128]="";
   int i;
   int ndata=-1;
   list_Spr *lspr=NULL;
   RGB pal[16];

   for (i=0;i<16;i++)
        pal[i] = defaulpal[i];

   //save_config(NAME_INI);
   if(ini_liballeg())
      return 1;

   if(!file_select("load lsp", cadtemp, "lsp"))
      return 1;
   else
      lspr = load_list_Spr(cadtemp);

   if(lspr==NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("invalid data...");
   }

   /* allocate the memory buffer */
   buffer = create_bitmap(SCREEN_W, SCREEN_H);
   text_mode(-1);


   while(!salir) {

        if(keypressed())
        {
           tempkey = readkey() >> 8;
           switch(tempkey)
           {
            case KEY_ESC:
                 if(alert("Exit?", "Sure?", NULL, "Yes", "NO", 121, 110)==1)
                    salir = 1;
            break;
            case KEY_L:
                 sprintf(cadtemp,"\0");
                 if(file_select("load RGB pallete", cadtemp, "TXT"))
                 {
                   load_pallete_16_RGB(pal,cadtemp);
                 }
            break;
            case KEY_LEFT:
                 if( ndata>0 ) ndata--;
            break;
            case KEY_RIGHT:
                 if( ndata < (lspr->n-1) )
                    ndata++;
            break;
            case KEY_V:
                 if( (ndata>=0) && (ndata<lspr->n) )
                 {
                  if(bmp) destroy_bitmap(bmp);
                  bmp = get_bmp_spr(lspr->sprites[ndata],pal);
                 }
            break;
           }
        }
        clear(buffer);
        if(bmp)
           blit(bmp, buffer, 0, 0, (SCREEN_W-bmp->w)/2, (SCREEN_H-bmp->h)/2, SCREEN_W, SCREEN_H);
        textprintf(buffer, font, 0, 0, makecol(255,255,255), "cursor for view data");
        textprintf(buffer, font, 0, 8, makecol(255,255,255), "L load pallete");
        textprintf(buffer, font, 0, 16, makecol(255,255,255), "press ESC for EXIT");
        textprintf(buffer, font, 0, 24, makecol(255,255,255), "press V for actualize spr");
        if(lspr->n>0)
        {
           textprintf(buffer, font, 0, SCREEN_H-24, makecol(255,255,255), "actual data: %d",ndata);
           textprintf(buffer, font, 0, SCREEN_H-16, makecol(255,255,255), "num data in list: %d",lspr->n);
        }
        textprintf(buffer, font, 0, SCREEN_H-8, makecol(255,255,255), STR_VERSION);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   }

   destroy_list_Spr(lspr);

   if(bmp) destroy_bitmap(bmp);

   destroy_bitmap(buffer);
   set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
   allegro_message("version: %s",STR_VERSION);

   return 0;
}END_OF_MAIN();
