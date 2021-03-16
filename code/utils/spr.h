/* 
   Name:  Spr.h
   Author:  Mauricio lopez
   Description: source code for a conversor bmp to spr 4 bits, 16 colors
   Date: 03/12/01 12:53
*/
#ifndef __SPR_H__
#define __SPR_H__
#include <stdlib.h>
#include "allegro.h"

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif
////////////////////joy macros///////////////////////////////////////
//para este formato w y h deben ser pares
typedef unsigned char Ubyte;

typedef struct Spr{
int w,h;
Ubyte *data;       //el valor del array sera de w*h/2 cada pixel 4 bits
}Spr;

typedef struct list_Spr{
int n;
int actual;       //cuantos Spr hay en la lista
int *w,*h;  //data de los tamaños
Spr **sprites;
}list_Spr;

Spr *create_spr(int w, int h);
Spr *get_spr(BITMAP *bmp);
void destroy_spr(Spr *spr);
int Spr_getpixel(Spr *spr,int x,int y);
void Spr_putpixel(Spr *spr,int x,int y,int col);

#define makecolrgb(col)  makecol16(col.r*4,col.g*4,col.b*4)
BITMAP *get_bmp_spr(Spr *spr,RGB *pal);

void save_Spr(Spr *spr,char *filename);
void save_Spr_txt(Spr *spr,char *filename);
Spr* load_Spr(char *filename);
Spr * load_Spr_txt(char *filename);

list_Spr *create_list_Spr(int n);
int add_Copy_Spr(list_Spr *lspr,Spr *spr);
int add_Spr(list_Spr *lspr,Spr *spr);
int add_Spr_bmp(list_Spr *lspr,BITMAP *bmp);
void destroy_list_Spr(list_Spr *lspr);

void save_list_Spr(list_Spr *lspr,char *filename);
list_Spr* load_list_Spr(char *filename);
//////////////------PALLETE ROUTINES-----/////////////////////
void blit_4_in_16_RGB(Spr *spr,BITMAP *dest,RGB *pal);
void blit_4_in_16_int(Spr *spr,BITMAP *dest,int *pal);

void save_pallete_16_RGB(RGB *pal,char *filename);
void load_pallete_16_RGB(RGB *pal,char *filename);
void save_pallete_16_int(int *pal,char *filename);
void load_pallete_16_int(int *pal,char *filename);

void convert_RGB_to_int_16(RGB *pals,int *pald);
void convert_int_to_RGB_16(RGB *pald,int *pals);
/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __SPR_H__
