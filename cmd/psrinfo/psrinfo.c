#include <stdio.h>
#include <string.h>


// Including user defined header file each ment for specific purpose
#include "./headers/psrinfo_error_msg.h"			// All error messages are in it
#include "./headers/psrinfo_struct_init.h"		// Contains the structure and its initialization function
#include "./headers/psrinfo_functions.h"			// Contains functions for each option seperately



//
// Main code begins
int main(int argc,char** argv)
{
	psrinfo_options* psropt;	

	int optch;							// Holds the individual option provided
	char optstring[] = ":vps";					// Holds the option list to verify the given option
	extern int optind ;						// Holds the option number being processed
	extern int optopt;						// Holds the error option if that option is not in the optstring
		
	int count = 0;							// For loop variable
	int count1 = 0;							// count for the length of the extracted number in string
	int count2 = 0;							// Count for the argument
	int len;							// Stores length of the extracted number in string formats
	int err_arg_flag = 0;						// Stores 0 if argument is number else 1
	int temp_argc ;							// To store the value of argc for future use
	int exit_flag = 0;						// To decide whether program exit with 0 or other
	
	optind = 1;							// Initializing optind to 1 so that it start processing option
	temp_argc = argc;						// Assigning value of argc to temp_argc

	psropt = (psrinfo_options*)malloc(sizeof(psrinfo_options));	// Allocating space for the strucutre

	init_structure(psropt);						// Initializing the stucture

	
	// Processing command line options and arguments
	while((optch = getopt(argc,argv,optstring)) != -1)
	{
	
		switch(optch)
		{
			case 'v':
				if(psropt->s_option_status == 1)		//checking if s is also used or not
				{
					mutually_exclusive_error_print();
					exit_flag = 1;
					goto exit;
				}
				psropt->v_option_status = 1;
				break;
			case 's':
				if(psropt->v_option_status == 1)		//checking if v is alos used or not
				{
					mutually_exclusive_error_print();
					exit_flag = 1;
					goto exit;
				}
				psropt->s_option_status = 1;
				
				break;
			case 'p':
				psropt->p_option_status = 1;
				break;
			case '?':						//if unknown argument 
				printf("Unknown Option : %c\n",optopt);		// then print the error message
				usage_error_print();
				exit_flag = 1;
				goto exit;
				break;
		}	

	}


	
	// Checking whether only one argument is provided if option s is present
	if(psropt->s_option_status == 1)
	{
		if((temp_argc - optind) != 1)
		{
			only_one_argument_s_error_print();
			exit_flag = 1;
			goto exit;
		}
	}
	

	
	// Storing the argument values in the array if the argument is a number
	// If not number then error message is shown
	for(count = optind;count < temp_argc;count++)
	{
		len = strlen(argv[count]);
		for(count1 = 0;count1 < len;count1++)
		{
			if(argv[count][count1] >= '0' && argv[count][count1] <= '9')
				continue;
			else
			{
				invalid_argument_error_print(argv[count]);
				err_arg_flag = 1;
				break;
			}

		}
		if(err_arg_flag != 1)
			psropt->arguments[count2++] = atoi(argv[count]);
		err_arg_flag  = 0;
	}


	//
	// Condition for every combination of the options and thus calling specific function
	if(psropt->p_option_status == 0 && psropt->v_option_status == 0 && psropt->s_option_status == 0)
		psrinfo_no_opt_fn(psropt);
	else if(psropt->v_option_status == 1 && psropt->p_option_status != 1)
		psrinfo_v_opt_fn(psropt);
	else if(psropt->v_option_status == 1 && psropt->p_option_status == 1)
		psrinfo_vp_opt_fn(psropt);
	else if(psropt->s_option_status == 1)
		psrinfo_s_sp_opt_fn(psropt);
	else if(psropt->p_option_status == 1 && psropt->v_option_status != 1 && psropt->s_option_status != 1)
		psrinfo_p_opt_fn(psropt);

exit:
	if(exit_flag == 1)
		exit(1);
	else
		return 0;
}

