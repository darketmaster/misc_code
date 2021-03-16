/*
  Name:      BASE.CPP 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  BASE ROUTINES
*/

#include <STDIO.H>
#include <STRING.H>
#include "base.h"
#include "allegro.h"

///////////////////////////////////////////////////////////////////////
_move::_move() 
{ 
    memset(name, 0, sizeof(char) * MAXNAMESIZE ); 
}
_move::~_move()
{
}
void _move::ini(char *cad) 
{ 
     strcpy(name, cad);
}
///////////////////////////////////////////////////////////////////////
