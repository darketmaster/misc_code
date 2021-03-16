/*
  Name:      FILELOG.C 
  Copyright: 2006
  Author:    MAURICIO LOPEZ TORRES 
  Date: 20/07/06 09:34
  Description:  FILE LOG ROUTINES
*/
#include <string.h>
#include "filelog.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

int error_code;
char name_file_log[256];

void create_file_log(char *filename)
{
  FILE *f;
  time_t time_;

  time(&time_);
  strcpy(name_file_log,filename);
  f = fopen(name_file_log,"w");  
  add_in_log("created: %s\n",ctime(&time_));
  add_in_log("===========================================\n\n");
  fclose(f);
}

void add_error(int err,char *format, ...)
{
  char buf[512];
  va_list ap;
  FILE *f;
  
  f = fopen(name_file_log,"r+");
  if(f) {
     fseek(f, 0, SEEK_END);
     va_start(ap, format);
     vsprintf(buf, format, ap);
     va_end(ap);
     fputs(buf, f);      
     fclose(f);
   }
   error_code = err;
}

void add_in_log(char *format, ...)
{
  char buf[512];
  va_list ap;
  FILE *f;
  
  f = fopen(name_file_log,"r+");
  if(f) {
     fseek(f, 0, SEEK_END);
     va_start(ap, format);
     vsprintf(buf, format, ap);
     va_end(ap);
     fputs(buf, f);      
     fclose(f);
   }
}

////////////////////header signatures procedures////////////////////

int signature_load_file_b(FILE *f,int nbytes,char *csig)
{
  char signature[24];
  fread(signature, sizeof(char), nbytes, f);
  if(stricmp(signature,csig) == 0)
      return 1;
  else {
  return 0;
  }
}

int signature_save_file_b(FILE *f,int nbytes,char *csig)
{
  fwrite(csig, sizeof(char), nbytes, f);
  return 1;
}

int signature_load_file_t(FILE *f,char *csig)
{
    char szLine[256];
    if (!fgets (szLine, 256, f))
		return 0;
    if(strncmp(szLine, csig, strlen(csig))== 0)
        return 1;
    return 0;
} 
   
int signature_save_file_t(FILE *f,char *csig)
{
    int res;
    res = fprintf(f, "%s\n", csig );
    return res;
}  

////////////////////////////////////////////////////////////////////////////
int fexists(char *filename)
{
    FILE *f;
    
    f = fopen(filename, "rb");    
    if(!f) 
       return 0;        
    fclose(f);
    
    return 1;
}    

