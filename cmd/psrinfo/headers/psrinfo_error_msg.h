/* This file contains all the function corresponding 
   to the specific errors
*/
#ifndef __psrinfo_error_message_H__
#define __psrinfo_error_message_H__

#include <stdio.h>

void usage_error_print(void);
void invalid_argument_error_print(char*);
void mutually_exclusive_error_print(void);
void only_one_argument_s_error_print(void);




#endif
