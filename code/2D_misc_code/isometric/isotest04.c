#include "allegro.h"
#include "isometric.h"

#define NAME_INI "prg.ini"

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
      allegro_message("ini no found \n tama�o estandar 640x480");
   }

   set_color_depth(16);
   if (set_gfx_mode(GFX_AUTODETECT, sw, sh, 0, 0) != 0) {  //_WINDOWED
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
      gfxerror=1;
   }
   return gfxerror;
}

int demo_tilemap(BITMAP *dest)
{
 DATAFILE *data = NULL;
 int w = 32;
 int h = 16;
 int xoff = SCREEN_W/2;
 int yoff = 50;
 int i,j,x,y;
 int salir=0;

 int xx,yy,xo,yo;
 int posx,posy;

 char map[8][8] = {{1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1},
                   {1, 1, 1, 1, 1, 1, 1, 1}
                };

 data = load_datafile("data/tiles01.dat");
 posx = 0;
 posy = 0;

     while (!salir) {

     if(keypressed())
     {
      int tempkey = readkey() >> 8;
      switch (tempkey)
      {
        case KEY_ESC:
             salir = 1;
        break;
        case KEY_UP:
             posy -= 1;
        break;
        case KEY_DOWN:
             posy += 1;
        break;
        case KEY_LEFT:
             posx -= 1;
        break;
        case KEY_RIGHT:
             posx += 1;
        break;
        case KEY_W:
             //hview -= 1;
        break;
        case KEY_Z:
             //hview += 1;
        break;
        case KEY_A:
             //wview -= 1;
        break;
        case KEY_D:
             //wview += 1;
        break;
      }//end switch
     }//end if
     clear_to_color(dest, makecol(0, 0, 128));

 
   xo = posx & 15;
   yo = posy & 15;
   xx   = xo - yo;
   yy   = (xo/2) + (yo/2);

 for(i = 0; i < 8;i++)
     for(j = 0; j < 8 ;j++)
     {
       x = ((i-j-1) * w/2) + xoff +xx;
       y = ((i+j) * h/2) + yoff +yy;
       draw_rle_sprite(dest, (RLE_SPRITE *)data[2].dat, x, y);
       textprintf(dest, font, x+4, y+4, makecol(255, 255, 255), "%d,%d",i,j);//j*8+i);
     }//end for

 rect(dest,xoff,yoff,xoff+80,yoff+60,makecol(255,0,0));
 /////ciclo fin
 blit(dest, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
 }//end while !keypressed

 if(!data)
   return 0;

 unload_datafile(data);
 return 0;
}

int demo2_tilemap(BITMAP *dest);

int main()
{
   BITMAP *buffer;

   //save_config(NAME_INI);
   if(ini_liballeg())
      return 1;

   ini_isometric_math(32.0, 16.0);

   /* allocate the memory buffer */
   buffer = create_bitmap(SCREEN_W, SCREEN_H);
   text_mode(-1);

   demo2_tilemap(buffer);

   destroy_bitmap(buffer);

   return 0;
}END_OF_MAIN();


////////////////////////////
int demo2_tilemap(BITMAP *dest)
{
 DATAFILE *data = NULL;
 int xoff = SCREEN_W/2;
 int yoff = 50;
 int i,j,x,y;
 int mapw,maph;
 int nw,nh;
 int c,xini,yini,x1,y1,incx,incy;

 float rposx,rposy;
 float iposx,iposy;
 float dscreen,dtile;
 int ntil,sig;

 char map[16][16] = {
                   { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1},
                   { 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1}
                };

 mapw = 16;
 maph = 16;

 dscreen = sqrt(80*80+60*60);
 dtile = sqrt(Wiso/2*Wiso/2+Hiso/2*Hiso/2);
 ntil = dscreen/dtile + 1;
 iposx = dtile * 3.0;
 iposy = dtile * 0.0;

 sig = 1;

 data = load_datafile("data/tiles01.dat");

    while (!keypressed()) {
      clear_to_color(dest, makecol(0, 0, 128));

     textprintf(dest, font, 0, 0, makecol(255, 255, 255), "dscreen %f, dtile %f",dscreen,dtile);
     textprintf(dest, font, 0, 8, makecol(255, 255, 255), "xini %d, yini %d ntil% d",xini,yini,ntil);

     xini = iposx/dtile - ntil/2;
     yini = iposy/dtile;
     for(i=0,incx=0,incy=0; i < ntil; i++)
     {
         if(i%2!=0) incy++;
         if(i%2==0 && i>0 ) incx++;
         x1 = xini + incx; y1 = yini - incy;
         for(j = 0; j < (60/Hiso) + 2 ;j++)
         {
              if((x1>=0&&x1<mapw) && (y1>=0&&y1<maph))
                if(map[x1][y1] >= 0)
                {
                x = ((x1-xini-y1+yini-1) * Wiso/2) + xoff - (int)x_iso_to_rec((iposx/dtile - xini - ntil/2)*dtile,(iposy/dtile - yini)*dtile);
                y = ((x1-xini+y1-yini) * Hiso/2) + yoff - (int)y_iso_to_rec((iposx/dtile - xini - ntil/2)*dtile,(iposy/dtile - yini)*dtile);
                draw_rle_sprite(dest, (RLE_SPRITE *)data[2].dat, x, y);
                textprintf(dest, font, x+4, y+4, makecol(255, 255, 255), "%d,%d",x1,y1);
                }
             x1++;
             y1++;
         }//end for j
     }//end for i

     iposx += .1*sig;
     iposy += .1*sig;
     if(iposx > dtile * mapw) sig=-1;
     if(iposx < 0) sig=1;

/*
 for(i = 0; i < mapw;i++)
     for(j = 0; j < maph ;j++)
     {
       x = ((i-j-1) * Wiso/2) + xoff;
       y = ((i+j) * Hiso/2) + yoff;
       draw_rle_sprite(dest, (RLE_SPRITE *)data[2].dat, x, y);
       textprintf(dest, font, x+4, y+4, makecol(255, 255, 255), "%d,%d",i,j);
     }//end for*/
 rect(dest,xoff,yoff,xoff+80,yoff+60,makecol(255,0,0));

/////ciclo fin
     blit(dest, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
     }//end while !keypressed

 if(!data)
   return 0;

 unload_datafile(data);
 return 0;
}
