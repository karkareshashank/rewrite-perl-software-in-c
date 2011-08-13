#include <stdio.h>

#include "projadd_error_msg.h"

// This function prints the usage error if error happens for projadd
void usage_error_print_projadd()
{
	printf(" Usage: projadd [-n] [-f filename] [-p projid [-o]] [-c comment]\n");
	printf("        	[-U user[,user...]] [-G group[,group...]]\n");
	printf("                [-K name[=value[,value...]]] project\n");

}



void option_o_without_p_error_print()
{
	
	printf("projadd : -o requires -p projid to be specified \n");
	usage_error_print_projadd();
	exit(2);

}


//This function prints the error message if the user enter no argument with the option which requires argument
void missing_argument_error_print(char optch)
{
	printf("Option %c requires an argument\n",optch);
	usage_error_print_projadd();
	exit(3);
}



// This function prints error message if the user enter the invald option wiht the command
void unknown_option_error_print(char optch)
{
	printf("Unknown option: %c\n",optch);
	usage_error_print_projadd();
	exit(2);
}




void invalid_cmdline_arg_error_print()
{
	printf("projadd: Invalid command-line arguments\n");
	usage_error_print_projadd();

}












