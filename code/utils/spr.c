/* 
   Name:  Spr.c
   Author:  Mauricio lopez
   Description: source code for a conversor bmp to spr 4 bits, 16 colors
   Date: 03/12/01 12:53
*/
#include "spr.h"
#include <allegro.h>
#include "allegro/aintern.h"

/////////////////-----------Spr functions------------//////////////
Spr *create_spr(int w, int h)
{
 Spr *spr = NULL;
 spr = (Spr*)calloc(1,sizeof(Spr));
 if(spr == NULL) return NULL;

 spr->w = w;
 spr->h = h;
 spr->data = (Ubyte *)calloc(w*h/2,sizeof(Ubyte));

 if(spr->data  == NULL) { free(spr); spr = NULL;}
 else memset(spr->data, 0, w*h/2 * sizeof(Ubyte));

 return spr;
}

Spr *get_spr(BITMAP *bmp)
{
 Spr *spr = NULL;
 int i,j,k;
 Ubyte temp,pix1,pix2;
 spr = (Spr*)calloc(1,sizeof(Spr));
 if(spr == NULL) return NULL;

 spr->w = bmp->w;
 spr->h = bmp->h;
 spr->data = (Ubyte *)calloc(bmp->w*bmp->h/2,sizeof(Ubyte));

 if(spr->data  == NULL) { free(spr); spr = NULL;}
 else
 {
   for(j = 0; j < bmp->h ; j++)
       for(i = 0; i < bmp->w ; i+=2)
       {
        pix1 = (Ubyte)getpixel(bmp,i  ,j);
        pix2 = (Ubyte)getpixel(bmp,i+1,j);
        temp = 0;
        temp = (pix1 << 4) + pix2;
        spr->data[(j*bmp->w/2)+(i/2)] = temp;
       }
 }

 return spr;
}

int Spr_getpixel(Spr *spr,int x,int y)
{
 int mod,div,pos;
 int pix = -1;
 if(x < spr->w && y < spr->h)
 {
    mod = x & 0x1;  //el modulo = w%2
    div = x >> 1;   //la division w / 2
    pos = y*spr->w/2 + div;
    pix = 0;        //inicializar pix a cero
    if(mod == 0)
       pix = spr->data[pos] >> 4;      //toma los primeros 4 bits
    else pix = spr->data[pos] & 0x0F;  //toma los ultimos 4 bits
 }
 return pix;
}

void Spr_putpixel(Spr *spr,int x,int y,int col)
{
 Ubyte pix1 = 0, pix2 = 0;
 int mod,div,temp,pos;
  if(x < spr->w && y < spr->h)
 {
    mod = x & 0x1;  //el modulo = w%2
    div = x >> 1;   //la division w / 2
    pos = y*spr->w/2 + div;
    pix1 = spr->data[pos] & 0xF0;
    pix2 = spr->data[pos] & 0x0F;
    temp = col & 0x0F;                  //asegurarse que solo va hasta 4 bits
    if(mod == 0)
       spr->data[pos] = (temp << 4) + pix2;
    else
       spr->data[pos] = pix1 + temp;
 }
}

void destroy_spr(Spr *spr)
{
 if(spr)
 {
    if(spr->data) free(spr->data);
    free(spr);
    spr = NULL;
 }
}

void save_Spr(Spr *spr,char *filename)
{
 FILE *f;
 f=fopen(filename,"wb");
 if(f)
 {
   fwrite(&spr->w,sizeof(int),1,f);
   fwrite(&spr->h,sizeof(int),1,f);
   fwrite(spr->data,sizeof(Ubyte),spr->w*spr->h/2,f);
   fclose(f);
 }
}

void save_Spr_txt(Spr *spr,char *filename)
{
 FILE *f;
 int i,j;
 f=fopen(filename,"wt");
 if(f)
 {
   fprintf(f,"int W= %d;\n",spr->w);
   fprintf(f,"int H= %d;\n",spr->h);
   fprintf(f,"data[%d] = {\n",spr->w*spr->h/2);
   for(j = 0; j<spr->h; j++)
   {
      for(i=0; i < spr->w/2 ; i++)
          fprintf(f,"%02X, ",spr->data[j*(spr->w/2) + i]);
      fprintf(f,"\n");
   }
   fprintf(f,"};");
   fclose(f);
 }
}

Spr* load_Spr(char *filename)
{
 FILE *f;
 Spr *spr=NULL;
 f=fopen(filename,"rb");
 if(f)
 {
   spr = (Spr*)calloc(1,sizeof(Spr));
   if(spr == NULL) { fclose(f); return NULL;}
   fread(&spr->w, sizeof(int), 1, f);
   fread(&spr->h, sizeof(int), 1, f);
   spr->data = (Ubyte *)calloc( spr->w*spr->h/2, sizeof(Ubyte));
   fread(spr->data,sizeof(Ubyte), spr->w*spr->h/2 ,f);
   fclose(f);
 }
 return spr;
}

Spr * load_Spr_txt(char *filename)
{
 FILE *f;
 int i,j,temp;
 Spr *spr=NULL;
 f=fopen(filename,"rt");
 if(f)
 {
   spr = (Spr*)calloc(1,sizeof(Spr));
   if(spr == NULL) { fclose(f); return NULL;}
   fscanf(f,"int W= %d;\n",&spr->w);
   fscanf(f,"int H= %d;\n",&spr->h);
   fscanf(f,"data[%d] = {\n",temp);
   spr->data = (Ubyte *)calloc( spr->w*spr->h/2, sizeof(Ubyte));
   for(j = 0; j<spr->h; j++)
   {
      for(i=0; i < spr->w/2 ; i++)
          fscanf(f,"%02X, ",&spr->data[j*(spr->w/2) + i]);
      fscanf(f,"\n");
   }
   fscanf(f,"};");
   fclose(f);
 }
 return spr;
}

//////////////////-------list Spr functions-------------------------/////////////////////
list_Spr *create_list_Spr(int n)
{
 list_Spr *lspr=NULL;
 int i;

 lspr = (list_Spr*) calloc(1,sizeof(list_Spr));
 if(lspr == NULL) return NULL;

 lspr->actual = 0;
 lspr->n = n;
 lspr->w = (int*)calloc(n,sizeof(int));
 lspr->h = (int*)calloc(n,sizeof(int));
 lspr->sprites = (Spr**)calloc(n,sizeof(Spr*));

 if((lspr->w==NULL) && (lspr->h==NULL) && (lspr->sprites==NULL))
 {
   if(lspr->w) free(lspr->w);
   if(lspr->h) free(lspr->h);
   if(lspr->sprites) free(lspr->sprites);
   if(lspr) free(lspr);
   lspr = NULL;
 }else
 for(i = 0; i< n ;i++)
     lspr->sprites[i] = NULL;

 return lspr;
}

int add_Copy_Spr(list_Spr *lspr,Spr *spr)
{
 if(lspr->actual < lspr->n)
 {
  lspr->sprites[lspr->actual] = create_spr(spr->w, spr->h);
  memcpy(lspr->sprites[lspr->actual]->data,spr->data,sizeof(Ubyte)*spr->w*spr->h/2);
  lspr->actual++;
 }
}

int add_Spr(list_Spr *lspr,Spr *spr)
{
 if(lspr->actual < lspr->n)
 {
  lspr->sprites[lspr->actual] = spr;
  lspr->actual++;
 }
}

int add_Spr_bmp(list_Spr *lspr,BITMAP *bmp)
{
 if(lspr->actual < lspr->n)
 {
  lspr->sprites[lspr->actual] = (Spr*)get_spr(bmp);
  lspr->actual++;
 }
}

void destroy_list_Spr(list_Spr *lspr)
{
 int i;
 if(lspr)
 {
    if(lspr->sprites)
    {
        for(i = 0; i<lspr->n ;i++)
            destroy_spr(lspr->sprites[i]);
        free(lspr->sprites);
    }
    if(lspr->w)
       free(lspr->w);
    if(lspr->h)
       free(lspr->h);
    free(lspr);
    lspr = NULL;
 }
}

void save_list_Spr(list_Spr *lspr,char *filename)
{
 FILE *f;
 int n;
 f=fopen(filename,"wb");
 if(f)
 {
    fwrite(&lspr->actual,sizeof(int),1,f);

    for(n=0;n<lspr->actual;n++)
    {
       fwrite(&lspr->w[n],sizeof(int),1,f);
       fwrite(&lspr->h[n],sizeof(int),1,f);
    }//end for

    for(n=0;n<lspr->actual;n++)
    {
       fwrite(&lspr->sprites[n]->w,sizeof(int),1,f);
       fwrite(&lspr->sprites[n]->h,sizeof(int),1,f);
       fwrite(lspr->sprites[n]->data,sizeof(Ubyte),lspr->sprites[n]->w*lspr->sprites[n]->h/2,f);
    }//end for
   fclose(f);
 }
}

list_Spr* load_list_Spr(char *filename)
{
 FILE *f;
 int n,i;
 list_Spr *lspr=NULL;
 f=fopen(filename,"rb");
 if(f)
 {
   n = 0;
   lspr = (list_Spr *)calloc(1,sizeof(list_Spr));
   fread(&n,sizeof(int),1,f);
   lspr->n = n;
   if(lspr == NULL) {fclose(f); return NULL;}
   lspr->actual = n;
   lspr->w = (int*)calloc(n,sizeof(int));
   lspr->h = (int*)calloc(n,sizeof(int));
   lspr->sprites = (Spr**)calloc(n,sizeof(Spr*));

   if((lspr->w==NULL) && (lspr->h==NULL) && (lspr->sprites==NULL))
   {
    if(lspr->w) free(lspr->w);
    if(lspr->h) free(lspr->h);
    if(lspr->sprites) free(lspr->sprites);
    if(lspr) free(lspr);
    fclose(f);
    return NULL;
   }else
    for(i = 0; i< lspr->n ;i++)
     lspr->sprites[i] = NULL;

   for(i=0; i<lspr->n; i++)
   {
      fread(&lspr->w[i],sizeof(int),1,f);
      fread(&lspr->h[i],sizeof(int),1,f);
   }//end for load tamaños

   for(i=0; i<lspr->n; i++)
   {
     lspr->sprites[i] = (Spr*)calloc(1,sizeof(Spr));
     if(lspr->sprites[i] == NULL) { destroy_list_Spr(lspr); fclose(f); return NULL;}
     fread(&lspr->sprites[i]->w, sizeof(int), 1, f);
     fread(&lspr->sprites[i]->h, sizeof(int), 1, f);
     lspr->sprites[i]->data = (Ubyte *)calloc( lspr->sprites[i]->w*lspr->sprites[i]->h/2, sizeof(Ubyte));
     fread(lspr->sprites[i]->data,sizeof(Ubyte), lspr->sprites[i]->w*lspr->sprites[i]->h/2 ,f);
   }//end for load data spr

   fclose(f);
 }
 return lspr;
}

////////////////raceracion de spr a bitmaps//////////
BITMAP *get_bmp_spr(Spr *spr,RGB *pal)
{
 BITMAP *bmp = NULL;
 int i,j,k;
 Ubyte temp,pix1,pix2;

 bmp = create_bitmap_ex(16,spr->w,spr->h);
 if(bmp  == NULL) return NULL;

 for(j = 0; j < bmp->h ; j++)
     for(i = 0; i < bmp->w ; i+=2)
     {
        temp = spr->data[(j*bmp->w/2)+(i/2)];
        pix1 = temp >> 4;
        pix2 = temp & 0x0F;
        _putpixel16(bmp,i  ,j,makecolrgb(pal[pix1]));
        _putpixel16(bmp,i+1,j,makecolrgb(pal[pix2]));
     }
 return bmp;
}

///draw 4bits pallete in 16 bits pallete W y H debe ser divisible por 8 exactamente
void blit_4_in_16_RGB(Spr *spr,BITMAP *dest,RGB *pal)
{
   unsigned long address;
   int x, y;
   int mod;
   int w1;
   short temp1,temp2;

   bmp_select(dest);
   w1 = spr->w/2;
   for (y=0; y<spr->h; y++) {
        address = bmp_write_line(dest, y);

         for (x=0; x<w1; x++) {
             mod = y*w1 + x;
             temp1 = (short)makecolrgb(pal[(int)(spr->data[mod] >> 4)]);
             temp2 = (short)makecolrgb(pal[(int)(spr->data[mod] & 0x0F)]);
             bmp_write16(address, temp1);
             address+=sizeof(short);
             bmp_write16(address, temp2);
             address+=sizeof(short);
         }
   }
   bmp_unwrite_line(dest);
}

void blit_4_in_16_int(Spr *spr,BITMAP *dest,int *pal)
{
   unsigned long address;
   int x, y;
   int mod;
   int w1;
   short temp1,temp2;

   bmp_select(dest);
   w1 = spr->w/2;
   for (y=0; y<spr->h; y++) {
        address = bmp_write_line(dest, y);

         for (x=0; x<w1; x++) {
             mod = y*w1 + x;
             temp1 = (short)pal[(int)(spr->data[mod] >> 4)];
             temp2 = (short)pal[(int)(spr->data[mod] & 0x0F)];
             bmp_write16(address, temp1);
             address+=sizeof(short);
             bmp_write16(address, temp2);
             address+=sizeof(short);
         }
   }
   bmp_unwrite_line(dest);
}

void save_pallete_16_RGB(RGB *pal,char *filename)
{
 FILE *f=NULL;
 int i;
 f = (FILE *)fopen(filename,"wt");
 if(f)
 {
    for(i =0 ;i< 16; i++)
        fprintf(f,"%X R:%03d G:%03d B:%03d\n",i,pal[i].r,pal[i].g,pal[i].b);
    fclose(f);
 }
}
void load_pallete_16_RGB(RGB *pal,char *filename)
{
 FILE *f=NULL;
 int i,temp;
 f = (FILE *)fopen(filename,"rt");
 if(f)
 {
    for(i =0 ;i< 16; i++)
        fscanf(f,"%X R:%03d G:%03d B:%03d\n",&temp,&pal[i].r,&pal[i].g,&pal[i].b);
    fclose(f);
 }
}
void save_pallete_16_int(int *pal,char *filename)
{
 FILE *f=NULL;
 int i;
 f = (FILE *)fopen(filename,"wt");
 if(f)
 {
    for(i =0 ;i< 16; i++)
        fprintf(f,"%X c:%d\n",i,pal[i]);
    fclose(f);
 }
}
void load_pallete_16_int(int *pal,char *filename)
{
 FILE *f=NULL;
 int i,temp;
 f = (FILE *)fopen(filename,"rt");
 if(f)
 {
    for(i =0 ;i< 16; i++)
        fscanf(f,"%X c:%d\n",&temp,&pal[i]);
    fclose(f);
 }
}

void convert_RGB_to_int_16(RGB *pals,int *pald)
{
 int i;
 for(i=0;i<16;i++)
     pald[i] = makecolrgb(pals[i]);
}

void convert_int_to_RGB_16(RGB *pald,int *pals)
{
 int i;
  for(i=0;i<16;i++)
  {
     pald[i].r = getr16(pals[i]);
     pald[i].g = getg16(pals[i]);
     pald[i].b = getb16(pals[i]);
  }
}
