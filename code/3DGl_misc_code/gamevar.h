#ifndef __GAMEVAR_H__
#define __GAMEVAR_H__

#include <stdlib.h>
#include "allegro.h"
#include <stdio.h>

#define MAXCAD 256
#define MAXNPATHS 20

///////////////////////////////////////////////////////////////////////////
//                           misc
///////////////////////////////////////////////////////////////////////////
typedef struct NPATH{
char str[MAXCAD];
};

typedef struct _STR32{
char str[32];
};

///////////////////////////////////////////////////////////////////////////
//                           FILEPATH
///////////////////////////////////////////////////////////////////////////
class FILEPATH{
public:
 int n_filepaths; //number of filepaths
 NPATH *ppath;    //pointer to file path
 
 FILEPATH();
 FILEPATH(int n);
 ~FILEPATH();
 
 void create(int n);
 void resize(int n);
 void destroy();
 void add(int n, char *cad);
 void save(FILE *fp);
 void load_fron_cfg_file(char *filename, char *section, char *def);
 
};

///////////////////////////////////////////////////////////////////////////
//                           GAMEPATHS
///////////////////////////////////////////////////////////////////////////

class Gamepaths{
public:
 int n_gamepaths;        //number of path type
 _STR32 *name;           //name for path type
 FILEPATH *dat;          //data for all paths type

 Gamepaths();
 ~Gamepaths();
 
 void create(int n);
 void destroy();
 void add(int n, char *cad);
 void save(char *filename);
 void load_fron_cfg_file(char *filename);
};

///////////////////////////////////////////////////////////////////////////
//                           _FNODE
///////////////////////////////////////////////////////////////////////////

class _FNODE{
public:

  al_ffblk *info;
  _FNODE *next;
  
  _FNODE();
  ~_FNODE();  
  void create();
  void destroy();
  void save(FILE *fp);     
};

///////////////////////////////////////////////////////////////////////////
//                           FLISTDATA
///////////////////////////////////////////////////////////////////////////
class FLISTDATA{
public:

 int n;
 char path[256];
 _FNODE *root;
 _FNODE *ptr;
 
 FLISTDATA();
 ~FLISTDATA();
 void create();
 void destroy(); 
 void add(al_ffblk *info);
 void save(char *filename); 
 void create(char *fpath, char *pattern, int attrib);    
};


/////////////////////////////////////////////////////////////////////////////

/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////







/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __GAMEVAR_H__
