#ifndef __projmod_error_msg_H__
#define __projmod_error_msg_H__

void usage_error_print_projmod();

void option_o_without_p_error_print();

void missing_argument_error_print(char);

void unknown_option_error_print(char);

void invalid_cmdline_arg_error_print();

void mutually_exclusive_error_print();

void mutually_exclusive_Af_error_print();

void option_ars_without_UGK_error_print(int,int,int);

void missing_option_error_print();




#endif
