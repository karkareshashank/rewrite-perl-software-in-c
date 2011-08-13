#include "psrinfo_error_msg.h"


//
// This function prints the Usage error message
void usage_error_print()
{
	printf("Usage:\n");
	printf("\tpsrinfo [-v] [-p] [processor_id ....]\n");
	printf("\tpsrinfo  -s  [-p] processor_id\n");
}


//
// This function prints the invalid argument error message
void invalid_argument_error_print(char* err_arg)
{
	printf("psrinfo : processor %s : Invalid Argument \n",err_arg);
}


//
// This function prints the error message if both s and v are used together 
void mutually_exclusive_error_print()
{
	printf("psrinfo : options -s and -v are mutually exclusive \n");
	usage_error_print();
}

//
// This function prints the error message if no or more that 1 arguments are provided with it
void only_one_argument_s_error_print()
{
	printf("psrinfo : must specify exactly one processor if -s is used \n");
	usage_error_print();
}

