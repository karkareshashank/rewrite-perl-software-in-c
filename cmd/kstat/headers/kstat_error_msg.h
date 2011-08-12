#ifndef __kstat_error_msg_H__
#define __kstat_error_msg_H__

void kstat_usage_error_print();

void T_option_error_print(char*);

void invalid_option_error_print(char);

void no_argument_error_print();

void mutually_exclusive_error_print();


#endif
