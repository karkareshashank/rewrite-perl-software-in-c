#include <stdio.h>

#include "kstat_error_msg.h"


// Prints the usage message of the kstat command
void kstat_usage_error_print()
{
	printf("Usage:\n");									
	printf("kstat [ -qlp ] [ -T d|u ] [ -c class ]\n");					
	printf("      [ -m module ] [ -i instance ] [ -n name ] [ -s statistic ]\n");	
	printf("	      [ interval [ count ] ]\n");						
	printf("	kstat [ -qlp ] [ -T d|u ] [ -c class ]\n");					
	printf("	      [ module:instance:name:statistic ... ]\n");				
	printf("	      [ interval [ count ] ]	\n");

}



// Prints the error if invalid timepstamp argument is provided
void T_option_error_print(char opt[])
{
	printf("kstat: Invalid timestamp specifier %s\n",opt);
	kstat_usage_error_print();
}



// Prints the error if an unknown option is provided
void invalid_option_error_print(char opt)
{
	printf("Unknown option : %c\n",opt);
	kstat_usage_error_print();
}



void no_argument_error_print()
{
	kstat_usage_error_print();
}



// Prints the error if both m,i,n,s options and pattern is provided
void mutually_exclusive_error_print()
{
	printf("kstat: module:instance:name:statistic and -m -i -n -s are mutually exclusive\n");
	kstat_usage_error_print();
}

