// Including pre defined header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


// Including user defined header files
#include  "./headers/kstat_structure.h"
#include  "./headers/kstat_error_msg.h"
#include  "./headers/kstat_functions.h"



// Main code starts here
int main(int argc,char** argv)
{
	int 		optch;					// Stores the return from the getopt fn
	extern int 	optind;					// Stores the current position of the option processed
	int		temp_argc;				// Stores the value of argc
	int 		exit_status 	= 0;			// Stores the exit status 
	int 		count		= 0;			//
	int		count1		= 0;			// All the four counts are for looping 
	int		count2		= 0;			// and counting purpose
	int		count3		= 0;			//
	int 		mins_flag	= 0;			// To check if m,i,n,s option is used or not

	char 		optstring[] 	= ":lpqT:c:m:i:n:s:";	// Stores all the options allowed with kstat
	char*		temp_buf;				// Temporary variable
	char*		mins_buf;				// To process mins values
	
	kstat_options* 	kopt;					// This structure contains all the option status and argument
	mins_arg*	new;					// Creates a node of the linklist which stores mins values
	mins_arg*	temp;					// Temporary pointer to the node



	// Allocating space for variables
	new		= (mins_arg*)malloc(sizeof(mins_arg));
	kopt 		= (kstat_options*)malloc(sizeof(kstat_options));
	temp_buf 	= (char*)malloc(sizeof(char)*200);
	mins_buf	= (char*)malloc(sizeof(char)*1024);


	// Initializing structure and  optind variable
	init_structure(kopt);
	mins_struct_init(new);
	optind = 1;
	temp_argc = argc;


	// Processing the options with getopt function
	while((optch = getopt(argc,argv,optstring)) != -1)
	{
		switch(optch)
		{
			case 'l':
					kopt->l_option_status = 1;
					break;
		
			case 'p':
					kopt->p_option_status = 1;
					break;

			case 'q':
					kopt->q_option_status = 1;
					break;
	
			case 'T':
					if(strcmp(optarg,"u") == 0 || strcmp(optarg,"d") == 0)
						kopt->T_argument = optarg[0];
					else
					{
						T_option_error_print(optarg);
						exit_status = 2;
						goto end;
					}
					break;

			case 'c':
					strcpy(kopt->c_arg,optarg);
					break;
	
			case 'm':
					strcpy(new->m_argument,optarg);
					mins_flag = 1;
					break;

			case 'i':
					strcpy(new->i_argument,optarg);
					mins_flag = 1;
					break;

			case 'n':
					strcpy(new->n_argument,optarg);
					mins_flag = 1;
					break;
		
			case 's':
					strcpy(new->s_argument,optarg);
					mins_flag = 1;
					break;

			case '?':
					invalid_option_error_print(optopt);
					exit_status = 2;
					break;

			case ':':
					no_argument_error_print(optopt);
					exit_status = 2;
					break;					

		}
	}


	// Default node with all values as * is used if m,i,n,s options are not used
	kopt->head = new;
	

	// Extracts interval , count values from the command line
	if(temp_argc == 2)
	{
		if(kopt->l_option_status == 0 && kopt->p_option_status == 0 && kopt->q_option_status == 0)
		{
			if(all_digit(argv[1]) == 1)
				kopt->interval = atoi(argv[1]);
			else
			{
				new = (mins_arg*)malloc(sizeof(mins_arg));		
				new->next = NULL;
				kopt->head = new;
				parse_pattern(new,argv[1]);
			}
		}
	}
	else if(temp_argc >= 3)
	{
		if(all_digit(argv[temp_argc-1]) == 1)
		{
			if(all_digit(argv[temp_argc-2]) == 1)
			{
				kopt->interval 	= atoi(argv[temp_argc-2]);
				if(kopt->interval  < 1)
				{
					fprintf(stderr,"kstat: Interval must be an integer >= 1");
					kstat_usage_error_print();
					exit(3);
				}
				kopt->count	= atoi(argv[temp_argc-1]);	
				if(kopt->count     < 1)
				{
					fprintf(stderr,"kstat: Interval must be an integer >= 1");
					kstat_usage_error_print();
					exit(3);
				}
			}
			else
			{
				kopt->interval	= atoi(argv[temp_argc-1]);
				if(kopt->interval  < 1)
				{
					fprintf(stderr,"kstat: Interval must be an integer >= 1");
					kstat_usage_error_print();
					exit(3);
				}
			}
		}

		count1 = 0;		


		// Extracts all the patterns provided without m,i,n,s options
		for(count = optind ; count < temp_argc ; count++)
		{
			if(all_digit(argv[count]) == 0)
			{
			   if(mins_flag == 0)			
			   {	count3++;
				new = (mins_arg*)malloc(sizeof(mins_arg));
				new->next = NULL;
				for(count1 =0 ;count1 < strlen(argv[count]);count1++)
					if(argv[count][count1] == ':')
						count2++;
				if(count2 > 3)
				{	count3--;continue;}
				if(count3 == 1)
				{
					kopt->head = new;
					temp 	   = new;
					parse_pattern(new,argv[count]);
				}
				else
				{
					temp->next = new;
					temp 	   = new;
					parse_pattern(new,argv[count]);
				}
			   }
			   else
				{
				mutually_exclusive_error_print();
				exit(2);
				}
			}
					
		}

	}


	// Calls the kstat function to perform the action 
	exit_status = kstat_fn(kopt);

end:

	// Exit according to the exit_status set previously
	exit(exit_status);
}	


