#include "gamevar.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////
//                           FILEPATH
///////////////////////////////////////////////////////////////////////////
FILEPATH::FILEPATH()
{
 n_filepaths = 0;
 ppath = NULL;
}

FILEPATH::FILEPATH(int n)
{
  create(n);
}

FILEPATH::~FILEPATH()
{
  destroy();
}
 
void FILEPATH::create(int n)
{
 
 n_filepaths = n;  
 ppath = (NPATH*) new NPATH[n_filepaths]; 
 add_in_log("creating %d paths in %X\n",n_filepaths,ppath);
 
}

void FILEPATH::resize(int n)
{
 NPATH *temp;
 int ttam;
  
 ttam = n_filepaths;
 n_filepaths = n;
 temp = (NPATH*) new NPATH[n_filepaths];
 
 ttam = ttam<n_filepaths? ttam:n_filepaths;
  
 memcpy(temp, ppath, ttam*sizeof(NPATH)); 
 if(ppath) delete [] ppath;
 ppath = temp;
  
 add_in_log("resize %d paths in %X\n",n_filepaths,ppath);
}

void FILEPATH::destroy()
{
 if(ppath) delete [] ppath;
 n_filepaths = 0;
 ppath = NULL;
}

void FILEPATH::add(int n, char *cad)
{  
  if(n < n_filepaths)
     strcpy(ppath[n].str,cad);
}

void FILEPATH::save(FILE *fp)
{
  fprintf(fp,"NUMFILEPATHS= %d\n", n_filepaths);
  for(int i=0; i<n_filepaths; i++)
  {
    fprintf(fp,"PATH[%02d]= \"%s\"\n", i, ppath[i].str);
  }  
}

void FILEPATH::load_fron_cfg_file(char *filename, char *section, char *def)
{
 int n;
 int i;
 char *ptr;
 char ctemp[32];

 override_config_file(filename);  
 n = get_config_int ( section , "NPATHS" , 0) ; 
 destroy();
 
 if(n>0 && n<MAXNPATHS)
 {
   create(n);
   for(i=0; i<n_filepaths; i++)
   {
      sprintf(ctemp,"PATH_%02d",i); 
      ptr = (char *)get_config_string(section, ctemp, def);
      add(i, ptr);
   }
 } 
 else
    {
     create(1);      //create 1 path 
     add(0, def);    //add main path
     }
}


///////////////////////////////////////////////////////////////////////////
//                           GAMEPATHS
///////////////////////////////////////////////////////////////////////////
Gamepaths::Gamepaths()
{
 n_gamepaths = 0;
 name = NULL;
 dat = NULL;
}

Gamepaths::~Gamepaths()
{
  destroy();
} 

void Gamepaths::create(int n)
{
 n_gamepaths = n;
 name = (_STR32*) new _STR32[n_gamepaths];
 dat = (FILEPATH*) new FILEPATH[n_gamepaths];
 
 add_in_log("creating %d paths name:%X, dat:%X\n",n_gamepaths, name, dat);
}

void Gamepaths::destroy()
{
 if(name) delete [] name;
 if(dat) 
 {
    for(int i=0; i<n_gamepaths; i++)
        dat[i].destroy();
    delete [] dat;
 }
 n_gamepaths = 0;
 name = NULL;
 dat = NULL;
}

void Gamepaths::add(int n, char *cad)
{
  if(n < n_gamepaths)
     strcpy(name[n].str,cad);
}

void Gamepaths::save(char *filename)
{
  FILE *fp;
  
  fp=fopen(filename,"wt");
  if(fp)
  {
    fprintf(fp,"NUM GAMEPATHS= %d\n", n_gamepaths);
    for(int i=0; i<n_gamepaths; i++)
    {
      fprintf(fp,"GAMEPATH NAME[%02d]= \"%s\"\n", i, name[i].str);
      dat[i].save(fp);
      fprintf(fp,"===========================================\n");
    }    
     fclose(fp);
  }
   else
      add_in_log("FILE %s error...",filename);
}

void Gamepaths::load_fron_cfg_file(char *filename)
{
  for(int i=0; i<n_gamepaths; i++)
      dat[i].load_fron_cfg_file(filename, name[i].str, name[i].str);  
}

///////////////////////////////////////////////////////////////////////////
//                           _FNODE
///////////////////////////////////////////////////////////////////////////
_FNODE::_FNODE()
{
  create();
}

_FNODE::~_FNODE()
{
  destroy();
}
  
void _FNODE::create()
{
    info = (al_ffblk *) new al_ffblk;
    next = NULL;
    //add_in_log("creating NODE WHIT info in %X %X\n",info, next);
}

void _FNODE::destroy()
{
    //add_in_log("destroy NODE whith info in %X next %X\n",info,next);
    if(info) delete info;

    if(next)
    {
       next->destroy();
       delete next;
    }    
    
    info = NULL;
    next = NULL;
}

void _FNODE::save(FILE *fp)
{
  if(info)
    fprintf(fp,"%-32s %08d ATTR: %03d CREATED: %s",info->name,info->size,info->attrib,ctime(&info->time));
  if(next) next->save(fp);
}

///////////////////////////////////////////////////////////////////////////
//                           FLISTDATA
///////////////////////////////////////////////////////////////////////////

FLISTDATA::FLISTDATA()
{
 create();
}

FLISTDATA::~FLISTDATA()
{
 destroy();
}

void FLISTDATA::create()
{
 n = 0;
 path[0] = '\0';
 root = NULL;
 ptr = NULL;
}

void FLISTDATA::destroy() 
{
  if(root) root->destroy();
  create();
}

void FLISTDATA::add(al_ffblk *info)
{
  _FNODE *node;
    
  node = (_FNODE*) new _FNODE; 
  *(node->info) = *info;
    
  if(root==NULL)
  {
    root = node;
    n++;
  }
  else
  {  
    ptr = root;  
    while(ptr->next) ptr = ptr->next;
    n++;
    ptr->next = node;
  }
}

void FLISTDATA::save(char *filename)
{
  FILE *f;
  f=fopen(filename,"wt");
  if(f)
  {
    fprintf(f,"NUMFILES: %d in PATH: %s\n", n, path);
    fprintf(f,"ROOT IN: %X\n", root);
    if(root) root->save(f);
    fclose(f);
  }
} 

void FLISTDATA::create(char *fpath, char *pattern, int attrib)
{
  al_ffblk t_info;
  char cadtemp[256];
  int find;
    
  destroy();
  create();
  
  if(fpath)   
     strcpy(path,fpath);
  else
     strcpy(path,".");
  
  put_backslash(path);
  sprintf(cadtemp,"%s%s", path, pattern);

  add_in_log("creating FLISTDATA %s\n",cadtemp);

  find = al_findfirst(cadtemp, &t_info, attrib);
  if(!find)
  {  
     while(!find)
     {
       if( (t_info.attrib&attrib) == attrib)
          if(t_info.name[0]!='.') 
             add(&t_info);              
       find = al_findnext(&t_info);
     }
     al_findclose(&t_info);
  }
}


///////////////////////////////////////////////////////////////////////////
//                           _
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//                           _
///////////////////////////////////////////////////////////////////////////

