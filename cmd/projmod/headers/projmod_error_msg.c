#include <stdio.h>

#include "projmod_error_msg.h"


// function which prints the usage error if it happens for projmod
void usage_error_print_projmod()
{
	printf("Usage: projmod [-n] [-f filename]\n");
	printf("       projmod [-n] [-A|-f filename] [-p projid [-o]] [-c comment]\n");
	printf("               [-a|-s|-r] [-U user[,user...]] [-G group[,group...]]\n");
	printf("               [-K name[=value[,value...]]] [-l new_projectname] project\n");

}





void option_o_without_p_error_print()
{
	
	printf("projmod : -o requires -p projid to be specified \n");
	usage_error_print_projmod();
	exit(2);

}


//This function prints the error message if the user enter no argument with the option which requires argument
void missing_argument_error_print(char optch)
{
	printf("Option %c requires an argument\n",optch);
	usage_error_print_projmod();
	exit(3);
}



// This function prints error message if the user enter the invald option wiht the command
void unknown_option_error_print(char optch)
{
	printf("Unknown option: %c\n",optch);
	usage_error_print_projmod();
	exit(2);
}




void invalid_cmdline_arg_error_print()
{
	printf("projmod: Invalid command-line arguments\n");
	usage_error_print_projmod();

}


void missing_option_error_print()
{
	printf("projmod: missing -c, -G, -l, -p, -U, or -K\n");
	usage_error_print_projmod();

}



void mutually_exclusive_Af_error_print()
{
	printf("projmod: -A and -f are mutually exclusive\n");
	usage_error_print_projmod();
	exit(2);
}



void option_ars_without_UGK_error_print(int a_flag,int r_flag,int s_flag)
{
	char option;

	if(a_flag == 1)
		option = 'a';
	else if(r_flag == 1)
		option = 'r';
	else if(s_flag == 1)
		option = 's';

	printf("projmod : -%c require -U user , -G group or -K attribute to be specified\n",option);
	usage_error_print_projmod();

	exit(2);

}



// this function prints the error message if the user user any two or all of the a,r,s options at same time
void mutually_exclusive_error_print()
{
	printf("projmod: -a, -r, and -s are mutually exclusive\n");
	usage_error_print_projmod();
	exit(2);
}









