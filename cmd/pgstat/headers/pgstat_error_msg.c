#include <stdio.h>

#include "pgstat_error_msg.h"


void pgstat_usage_error_print()
{
	printf("Usage:	pgstat [-A] [-C] [-p] [-s key | -S key] [-t number] [-T u | d]	\n");
	printf("	[-r string] [-R string] [-P pg ...] [-c processor_id... ] \n");			
	printf("	[interval [count]]\n");

}


void T_arg_error_print(char* optch)
{
	printf("pgstat: Invalid -T %c argument\n",optch);

	pgstat_usage_error_print();
}


void t_arg_error_print()
{
	printf("pgstat: -t should specify positive integer\n");
	
	pgstat_usage_error_print();
}


void sS_arg_error_print(char* optarg)
{
	printf("pgstat: invalid sort key %s\n",optarg);

	pgstat_usage_error_print();
}


void sS_error_print()
{
	printf("pgstat: -S and -s flags can not be used together\n");

	pgstat_usage_error_print();
}


void invalid_opt_arg_error_print(char optch)
{
	printf("Option %c requires an argument\n",optch);
	
	pgstat_usage_error_print();
}


void invalid_option_error_print(char optch)
{
	printf("Unknown option: %c\n",optch);

	pgstat_usage_error_print();
}
