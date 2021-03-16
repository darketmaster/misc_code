#include "assert.h"
#include <allegro.h>
#include <string.h>
#include "log.h"
#include <time.h>
#include <stdio.h>

int error_code;
char name_file_log[64];

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
     uvsprintf(buf, format, ap);
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
     uvsprintf(buf, format, ap);
     va_end(ap);
     fputs(buf, f);      
     fclose(f);
   }
}
