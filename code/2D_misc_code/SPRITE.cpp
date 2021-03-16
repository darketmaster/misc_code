/*
  Name:      SPRITE.CPP
  Copyright: 
  Author: m   MAURICIO LOPEZ
  Date: 22/07/06 11:48
  Description:  SPRITES ROUTINES
*/

#include "filelog.h"
#include "sprite.h"
#include "sload.h"


///////////////////////////////////////////////////////////////////////////
//                           SPRITES CLASS
///////////////////////////////////////////////////////////////////////////
void SPRITES::clear()
{
     nsprites = 0;
     npal = 0;
     nc = nf = 0;
     sprites = NULL;
     pal = NULL;
}

void SPRITES::destroy()
{
     int i;
     
     if(nsprites>0)
       if(sprites)
       {   
         for(i=0;i<nsprites;i++) 
            if(sprites[i]) 
               destroy_bitmap(sprites[i]);
         delete[] sprites;
       }
     if(npal>0)
       if(pal)
       {   
         for(i=0;i<npal;i++) 
            if(pal[i]) 
               delete[] pal[i];
         delete[] pal;
       } 
       
    clear();    
}
  
SPRITES::SPRITES()
{
   clear();
}

SPRITES::SPRITES(char *filename, int nc, int nf)
{
   load(filename, nc, nf);
}

SPRITES::~SPRITES()
{
    destroy();
}

int SPRITES::load(char *filename, int nc, int nf)
{
    BITMAP *bmp;
    int i, j;
    int tw, th;    //size of tile
    int c;
    
    add_in_log("--LOADING SPRITES--\n");
    
    this->nc = nc;
    this->nf = nf;
    pal = (RGB**) new RGB* [MAXPALS];
    npal = 1;
    pal[0] = (RGB*) new RGB[256];
    bmp = load_bitmap(filename, pal[0]);
    
    if(!bmp)
    {
       destroy();
       return 1;     
    }
    
    tw = bmp->w/nc;
    th = bmp->h/nf;
    nsprites = nc * nf;
    
    sprites = (BITMAP**) new BITMAP* [nsprites];
    
    add_in_log("nc: %d, nf: %d, nsprites: %d\n", this->nc, this->nf, nsprites);
    add_in_log("PAL: %X, SPRITES: %X\n", pal, sprites);
    add_in_log("Load bitmap: %s\n", filename);
    
    for(j=0; j<nf; j++)
        for(i=0;i<nc;i++)
        {
            c = ((j*nc)+i);
            sprites[c] = create_bitmap_ex(bitmap_color_depth(bmp), tw, th);
            //add_in_log("Sprite[%d]: %X\n", c, sprites[c]);
            blit(bmp, sprites[c], i * tw, j * th, 0, 0, tw, th);
            //_prog_data->cur++;
            //progress_proc();
        }
    
    destroy_bitmap(bmp);
}

void SPRITES::draw(BITMAP *dest, int ns, int x, int y, int flip)
{
   void (*_draw)(BITMAP *bmp, BITMAP *sprite, int x, int y);
   
   if(flip) _draw = draw_sprite_h_flip;
   else _draw = draw_sprite;
   
   _draw(dest, sprites[ns], x, y);   
}



///////////////////////////////////////////////////////////////////////////
//                           _
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//                           _
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//                           _
///////////////////////////////////////////////////////////////////////////
