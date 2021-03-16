#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdlib.h>
#include "allegro.h"
#include <stdio.h>

#define PADL(a) joy[(a)].stick[0].axis[0].d1
#define PADR(a) joy[(a)].stick[0].axis[0].d2
#define PADU(a) joy[(a)].stick[0].axis[1].d1
#define PADD(a) joy[(a)].stick[0].axis[1].d2
#define BUTTON(a,x)  joy[(a)].button[(x)].b

#define KEYBOARD_ONLY 0
#define KEYBOARD_PAD  1
#define GAMEPAD_ONLY  2

typedef unsigned char IUBYTE;
typedef unsigned short DBYTE;

extern const DBYTE Vbit[16];

#define GETBIT(A,B) ((A)&Vbit[(B)])
#define SETBIT(A,B) A=(A)|Vbit[(B)]

#define PUP    0
#define PDOWN  1
#define PLEFT  2
#define PRIGHT 3

#define MAXBUTTONS 16

class INPUT_INF{
  public:
  
  IUBYTE type;  //input interface type
  IUBYTE ipad;  //pad index for input
  int nbuttons; //input buttons
  int pmap[16]; //mapped for pad buttons
  int kmap[16]; //mapped for keyboard buttons
  DBYTE lb;     //data for last key pressed
  DBYTE b;      //data for key pressed
  int tb[16];   //data for time pressed in cicles  
  int (*update_f) (INPUT_INF *i_inf);
  
  INPUT_INF();
  ~INPUT_INF();  
  void create(IUBYTE _type, IUBYTE nb, int *_kmap, int *_pmap);
  void sel_pad(IUBYTE _type, IUBYTE _ipad, int *_pmap);
  int update();
};

int update_gpad(INPUT_INF *i_inf);
int update_keyb(INPUT_INF *i_inf);
int update_keyb_gpad(INPUT_INF *i_inf);


///////////////////////////////////////////////////////////////////////////
//                           INTERFACE_INPUT
///////////////////////////////////////////////////////////////////////////

class INTERFACE_INPUT{
public:

  int joys;
  int num_inputs;
  INPUT_INF *input;

  INTERFACE_INPUT();
  ~INTERFACE_INPUT();
  void create(int n);
  void destroy(); 
  void add(int n, IUBYTE _type, IUBYTE nb, int *_kmap, IUBYTE _ipad, int *_pmap);
  void update_inputs();
  
};


///////////////////////////////////////////////////////////////////////////
//                           misc functions
///////////////////////////////////////////////////////////////////////////

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __INPUT_H__
