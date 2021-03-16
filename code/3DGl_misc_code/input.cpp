#include "log.h"
#include "input.h"




//                        1        2      4        8
const DBYTE Vbit[16] = {0X0001, 0X0002, 0X0004, 0X0008,
                        0X0010, 0X0020, 0X0040, 0X0080, 
                        0X0100, 0X0200, 0X0400, 0X0800, 
                        0X1000, 0X2000, 0X4000, 0X8000
                        };

///////////////////////////////////////////////////////////////////////////
//                           helper
///////////////////////////////////////////////////////////////////////////                        
int update_gpad(INPUT_INF *i_inf)
{
  int i;
  
  i_inf->lb = i_inf->b;
  i_inf->b = 0;
  
  if(PADU(i_inf->ipad)) //bit 1
  {
     SETBIT(i_inf->b, PUP   );
     i_inf->tb[PUP]++;  
  } else i_inf->tb[PUP] = 0; 
   
  if(PADD(i_inf->ipad)) //bit 2
  {
     SETBIT(i_inf->b, PDOWN );
     i_inf->tb[PDOWN]++;
  } else i_inf->tb[PDOWN] = 0;  
    
  if(PADL(i_inf->ipad)) //bit 3
  {
     SETBIT(i_inf->b, PLEFT );
     i_inf->tb[PLEFT]++;
  } else i_inf->tb[PLEFT] = 0; 
     
  if(PADR(i_inf->ipad)) //bit 4
  {
     SETBIT(i_inf->b, PRIGHT);
     i_inf->tb[PRIGHT]++;
  } else i_inf->tb[PRIGHT] = 0; 
  
  for(i=4; i<i_inf->nbuttons; i++)
  {
     if(BUTTON(i_inf->ipad,i_inf->pmap[i]))
     {
       SETBIT(i_inf->b, i);
       i_inf->tb[i]++;
     } 
     else i_inf->tb[i] = 0; 
  }
  return i_inf->b;
}

int update_keyb(INPUT_INF *i_inf)
{
  int i;
  
  i_inf->lb = i_inf->b;
  i_inf->b = 0;
  
  for(i=0; i<i_inf->nbuttons; i++)
  {
     if(key[i_inf->kmap[i]])
     {
       SETBIT(i_inf->b, i);
       i_inf->tb[i]++;
     } 
     else i_inf->tb[i] = 0; 
  }
  
  return i_inf->b;
}

int update_keyb_gpad(INPUT_INF *i_inf)
{
  int i;
  
  i_inf->lb = i_inf->b;
  i_inf->b = 0;
  
  if(PADU(i_inf->ipad)||key[i_inf->kmap[PUP]]) //bit 1
  {
     SETBIT(i_inf->b, PUP   );
     i_inf->tb[PUP]++;  
  } else i_inf->tb[PUP] = 0; 
   
  if(PADD(i_inf->ipad)||key[i_inf->kmap[PDOWN]]) //bit 2
  {
     SETBIT(i_inf->b, PDOWN );
     i_inf->tb[PDOWN]++;
  } else i_inf->tb[PDOWN] = 0;  
    
  if(PADL(i_inf->ipad)||key[i_inf->kmap[PLEFT]]) //bit 3
  {
     SETBIT(i_inf->b, PLEFT );
     i_inf->tb[PLEFT]++;
  } else i_inf->tb[PLEFT] = 0; 
     
  if(PADR(i_inf->ipad)||key[i_inf->kmap[PRIGHT]]) //bit 4
  {
     SETBIT(i_inf->b, PRIGHT);
     i_inf->tb[PRIGHT]++;
  } else i_inf->tb[PRIGHT] = 0; 
  
  for(i=4; i<i_inf->nbuttons; i++)
  {
     if(BUTTON(i_inf->ipad,i_inf->pmap[i])||key[i_inf->kmap[i]])
     {
       SETBIT(i_inf->b, i);
       i_inf->tb[i]++;
     } 
     else i_inf->tb[i] = 0; 
  }
  
  return i_inf->b;
}

///////////////////////////////////////////////////////////////////////////
//                           INPUT_INF
///////////////////////////////////////////////////////////////////////////


INPUT_INF::INPUT_INF()
{
  int temp[16] = {0};
  
  create(KEYBOARD_ONLY, 16, temp, temp);  
}

INPUT_INF::~INPUT_INF()
{
}

void INPUT_INF::create(IUBYTE _type, IUBYTE nb, int *_kmap, int *_pmap)
{
  int i;
  
  type = _type;
  
  if(nb<=MAXBUTTONS)
     nbuttons = nb;
  else nbuttons = 16;
  for(i=0; i<nbuttons; i++) tb[i] = 0;  
  lb = 0;
  b = 0;  
  
  switch(type)
  {
     case KEYBOARD_ONLY:
          update_f = update_keyb;
          for(i=0; i< nbuttons; i++)
             kmap[i] = _kmap[i];
     break;
     case KEYBOARD_PAD:
          update_f = update_keyb_gpad;
          for(i=0; i< nbuttons; i++)
          {
              kmap[i] = _kmap[i];
              pmap[i] = _pmap[i];
          }
     break;
     case GAMEPAD_ONLY:
          update_f = update_gpad;
          for(i=0; i< nbuttons; i++)
              pmap[i] = _pmap[i];
     break;
  }
}

void INPUT_INF::sel_pad(IUBYTE _type, IUBYTE _ipad, int *_pmap)
{
  type = _type;
  if(_ipad < num_joysticks)
  {
     ipad = _ipad;
     for(int i=0; i< nbuttons; i++)
         pmap[i] = _pmap[i];
         
     if(type==GAMEPAD_ONLY) update_f = update_gpad;
     else update_f = update_keyb_gpad;
  }
  else
      {
        ipad = 0;
        type = KEYBOARD_ONLY;
        update_f = update_keyb;
      }
}

int INPUT_INF::update()
{
 return update_f(this);
}



///////////////////////////////////////////////////////////////////////////
//                           INTERFACE_INPUT
///////////////////////////////////////////////////////////////////////////

INTERFACE_INPUT::INTERFACE_INPUT()
{
  if(!install_joystick(JOY_TYPE_AUTODETECT))
  {
     joys = num_joysticks;
  }
  else joys = 0;
  
  num_inputs = 0;
  input = NULL;
}

INTERFACE_INPUT::~INTERFACE_INPUT()
{
}

void INTERFACE_INPUT::create(int n)
{
  joys = num_joysticks;
  num_inputs = n;
  input = (INPUT_INF *)new INPUT_INF[num_inputs];
}

void INTERFACE_INPUT::destroy()
{
  joys = 0;
  if(input) delete [] input;
  num_inputs = 0;
  input = NULL;
}

void INTERFACE_INPUT::add(int n, IUBYTE _type, IUBYTE nb, int *_kmap, IUBYTE _ipad, int *_pmap)
{
  if(n < num_inputs)
  {
    input[n].create(_type, nb, _kmap, _pmap);
    input[n].sel_pad(_type, _ipad, _pmap);
  }
}

void INTERFACE_INPUT::update_inputs()
{ 
 int i;
 
 poll_joystick();  
 //poll_keyboard();
 
 for(i=0; i<num_inputs; i++) 
    input[i].update();
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

///////////////////////////////////////////////////////////////////////////
//                           _
///////////////////////////////////////////////////////////////////////////
