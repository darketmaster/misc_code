#ifndef __FILELOG_H__
#define __FILELOG_H__

#include <stdlib.h>
#include <stdio.h>
/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////


#define add_separator_in_log() add_in_log("\n\n********************************************\n\n");

extern int error_code;
extern char name_file_log[256];

void create_file_log(char *filename);
void add_error(int err,char *format, ...);
void add_in_log(char *format, ...);

/////////////////file binary signatures

int signature_load_file_b(FILE *f,int nbytes,char *csig);
int signature_save_file_b(FILE *f,int nbytes,char *csig);


/////////////////file text signatures
int signature_load_file_t(FILE *f,char *csig);
int signature_save_file_t(FILE *f,char *csig);
  
int fexists(char *filename);

/////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __FILELOG_H__
