#include <stdio.h>

#include "pginfo_error_msg.h"


void pginfo_usage_error_print()
{
	printf("Usage:	pginfo [-T] [-p] [-v] [-r string] [-R string] [pg ... | -c processor_id ...]\n");
	printf("	pginfo -s [-v] [-r string] [-R string] [pg ... | -c processor_id ...]\n");
	printf("	pginfo -C | -I [-r string] [-R string] [pg ... | -c processor_id ...]\n");
	printf("	pginfo -h\n");

}



void IC_error_print()
{
	printf("pginfo: -I and -C flags can not be used together\n");

	pginfo_usage_error_print();
}


void ICpsT_error_print()
{
	printf("pginfo: options -C and -I can not be used with -p -s or -T\n");

	pginfo_usage_error_print();
}


void ps_error_print()
{
	printf("pginfo: option -p can not be used with -s\n");

	pginfo_usage_error_print();
}


void sT_error_print()
{
	printf("pginfo: option -T can not be used with -s\n");

	pginfo_usage_error_print();
}


void invalid_opt_arg_error_print(char optch)
{
	printf("Option %c requires an argument\n",optch);
	
	pginfo_usage_error_print();
}


void invalid_option_error_print(char optch)
{
	printf("Unknown option: %c\n",optch);

	pginfo_usage_error_print();
}
