#ifndef __LOG_H__
#define __LOG_H__

#include <stdlib.h>
#include "allegro.h"
/*  Allow inclusion in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////


#define add_separator_in_log() add_in_log("\n\n********************************************\n\n");

extern int error_code;
extern char name_file_log[64];

void create_file_log(char *filename);
void add_error(int err,char *format, ...);
void add_in_log(char *format, ...);


/////////////////////////////////////////////////////////////////////////////



/*-----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif // __LOG_H__
