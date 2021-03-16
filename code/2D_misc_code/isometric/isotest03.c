#include "allegro.h"
#include "isometric.h"

#define NAME_INI "prg.ini"

FILE *flog = NULL;

#define open_flog() flog = fopen("file.log","w")
#define close_flog() if(flog) fclose(flog);

void testlog()
{
   float frx,fry;
   float fix,fiy;

   open_flog();
   fprintf(flog,"RELATION_ISOMETRIC : %f\n",RELATION_ISOMETRIC);
   fprintf(flog,"BETA : %f  DEG: %f\n",BETA,RADTODEG(BETA));
   fprintf(flog,"ALFA : %f  DEG: %f\n",ALFA,RADTODEG(ALFA));
   fprintf(flog,"SIN_BETA : %f, COS_BETA : %f, SINCOS_BETA : %f\n",SIN_BETA, COS_BETA, SINCOS_BETA);
   fprintf(flog,"SIN_ALFA : %f, COS_ALFA : %f, SINCOS_ALFA : %f\n",SIN_ALFA, COS_ALFA, SINCOS_ALFA);

   fix = sqrt(320);
   fiy = 0;
   frx = x_iso_to_rec(fix,fiy);
   fry = y_iso_to_rec(fix,fiy);
   fprintf(flog,"iso(%f,%f)->REC(%f,%f)\n",fix,fiy,frx,fry);
   fix = 0;
   fiy = sqrt(320);
   frx = x_iso_to_rec(fix,fiy);
   fry = y_iso_to_rec(fix,fiy);
   fprintf(flog,"iso(%f,%f)->REC(%f,%f)\n",fix,fiy,frx,fry);
   fix = sqrt(320);
   fiy = sqrt(320);
   frx = x_iso_to_rec(fix,fiy);
   fry = y_iso_to_rec(fix,fiy);
   fprintf(flog,"iso(%f,%f)->REC(%f,%f)\n",fix,fiy,frx,fry);

   frx = 16;
   fry = 8;
   fix = x_rec_to_iso(frx,fry);
   fiy = y_rec_to_iso(frx,fry);
   fprintf(flog,"REC(%f,%f)->iso(%f,%f)\n",frx,fry,fix,fiy);
   frx = -16;
   fry = 8;
   fix = x_rec_to_iso(frx,fry);
   fiy = y_rec_to_iso(frx,fry);
   fprintf(flog,"REC(%f,%f)->iso(%f,%f)\n",frx,fry,fix,fiy);
   frx = 0;
   fry = 16;
   fix = x_rec_to_iso(frx,fry);
   fiy = y_rec_to_iso(frx,fry);
   fprintf(flog,"REC(%f,%f)->iso(%f,%f)\n",frx,fry,fix,fiy);

   close_flog();
}

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

 for(i = 0; i < 8;i++)
     for(j = 0; j < 8 ;j++)
     {
       x = ((i-j-1) * w/2) + xoff;
       y = ((i+j) * h/2) + yoff;
       draw_rle_sprite(dest, (RLE_SPRITE *)data[2].dat, x, y);
       textprintf(dest, font, x+4, y+4, makecol(255, 255, 255), "%d,%d",i,j);//j*8+i);
     }//end for

 rect(dest,xoff,yoff,xoff+80,yoff+60,makecol(255,0,0));

 if(!data)
   return 0;

 unload_datafile(data);
 return 0;
}

int demo2_tilemap(BITMAP *dest);
int demo3_tilemap(BITMAP *dest);

int main()
{
   BITMAP *buffer;

   //save_config(NAME_INI);
   if(ini_liballeg())
      return 1;

   ini_isometric_math(32.0, 16.0);
   testlog();

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
 int salir=0;
 int xoff = SCREEN_W/4;
 int yoff = 40;
 int i,j,x,y;
 int mapw,maph;
 int nw,nh;
 int c,xini,yini,x1,y1,incx,incy;

 float rposx,rposy;
 float iposx,iposy;
 float dtile;
 int ntil;
 float VEL = 2;
 int xo,yo,xx,yy;
 int wview = 160;
 int hview = 120;

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

 dtile = sqrt(Wiso*Wiso/4+Hiso*Hiso/4);
 //ntil = dscreen/dtile;
 ////ntil = 2*wview/Wiso;
 iposx = 0.0;
 iposy = 0.0;

 data = load_datafile("data/tiles01.dat");

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
             iposy -= VEL;
        break;
        case KEY_DOWN:
             iposy += VEL;
        break;
        case KEY_LEFT:
             iposx -= VEL;
        break;
        case KEY_RIGHT:
             iposx += VEL;
        break;
        case KEY_W:
             hview -= 1;
        break;
        case KEY_Z:
             hview += 1;
        break;
        case KEY_A:
             wview -= 1;
        break;
        case KEY_D:
             wview += 1;
        break;
      }//end switch
     }//end if

     clear_to_color(dest, makecol(0, 0, 128));

     ntil = 2*wview/Wiso;

     yini = iposy/dtile - (wview-hview)/(2*Hiso);
     xini = iposx/dtile - ntil/2 + yini;

     xx   = (int)x_iso_to_rec(iposx - (xini*dtile),iposy - (yini*dtile));
     yy   = (int)y_iso_to_rec(iposx - (xini*dtile),iposy - (yini*dtile));

     rposx = x_iso_to_rec(iposx,iposy);
     rposy = y_iso_to_rec(iposx,iposy);

     //yini = yini - (wview-hview)/(2*Hiso);
     //xini = xini - ntil/2 + yini;

     //set_clip(dest,xoff,yoff,xoff+wview,yoff+hview);
     rectfill(dest,xoff,yoff,xoff+wview,yoff+hview,makecol(255,255,255));

     for(i=0,incx=0,incy=0; i <= ntil; i++)
     {
         if(i%2!=0) incy++;
         if(i%2==0 && i>0 ) incx++;
         x1 = xini - incx;
         y1 = yini - incy;

         for(j = 0; j <= (hview/Hiso) ;j++,x1++,y1++)
         {
              if((x1>=0&&x1<mapw) && (y1>=0&&y1<maph))
                if(map[x1][y1] >= 0)
                {
                x = ( ( (x1-xini) - (y1-yini) -1) * Wiso/2) + xoff;// - xx;
                y = ( ( (x1-xini) + (y1-yini)   ) * Hiso/2) + yoff;// - yy;
                draw_rle_sprite(dest, (RLE_SPRITE *)data[2].dat, x, y);
                //textprintf(dest, font, x+4, y+4, makecol(255, 255, 255), "%d,%d",x1,y1);
                }
         }//end for j
     }//end for i

 //set_clip(dest,0,0,dest->w,dest->h);
 rect(dest,xoff,yoff,xoff+wview,yoff+hview,makecol(255,0,0));

 textprintf(dest, font, 0, 8, makecol(255, 0, 0), "xini %d, yini %d ntil% d",xini,yini,ntil);
 textprintf(dest, font, 0, 16, makecol(255, 0, 0), "ipos (%.3f %.3f)",iposx,iposy);
 textprintf(dest, font, 0, 24, makecol(255, 0, 0), "rpos (%.3f %.3f)",rposx,rposy);
 textprintf(dest, font, 0, 32, makecol(255, 0, 0), "xx,yx(%d,%d)",xx,yy);

/////ciclo fin
     blit(dest, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
     }//end while !keypressed

 if(!data)
   return 0;

 unload_datafile(data);
 return 0;
}
