#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "./headers/project_file_check.h"
#include "./headers/projadd_structure.h"
#include "./headers/projadd_error_msg.h"
#include "./headers/projadd_functions.h"



int main(int argc,char** argv)
{
	char 	     optstring[] = ":onc:f:G:K:p:U:";
	
	extern int   optind;
	int 	     temp_argc   = argc;
	int 	     optch;
	int 	     count_of_K  = 0;

	K_argument *new 	 = NULL;
	K_argument *temp 	 = NULL;
	projadd_options *projadd = NULL;	
	optind			 = 1;

	projadd = (projadd_options*)malloc(sizeof(projadd_options));

	init_structures(projadd);		// Initializes all variables in the structures to 0 and NULL

	while((optch = getopt(argc,argv,optstring)) != -1)
	{

		switch(optch)
		{
			case 'f':
					
					strcpy(projadd->f_argument,optarg);//copying argument in the variable
					break;

			case 'c':       
					strcpy(projadd->c_argument,optarg);	//copying argument in the variable
					break;
			
			case 'G':	
					strcpy(projadd->G_argument,optarg);	//copying argument in the variable
					break;
		
			case 'U':
					strcpy(projadd->U_argument,optarg);	//copying argument in the variable
					break;
			
			case 'p':
					projadd->p_argument = atoi(optarg);	//copying argument in the variable
					break;

			case 'n':
					projadd->n_option_status = 1;
					break;
		
			case 'o':
					projadd->o_option_status = 1;
					break;

			case 'K':
					new     = (K_argument*)malloc(sizeof(K_argument));	//creating new structure
					new->next = NULL;				
					if(count_of_K == 0)					
					{
						projadd->head = new;		//if first of LL then point head to it
						strcpy(new->data,optarg);	
						temp = new;
					}
					else				//if not first the add new node and point
					{				//previous next to it.
						strcpy(new->data,optarg);
						temp->next = new;
						temp = new;
					}
					count_of_K++;		// increase the count variable to track the no of nodes
					break;

			
			case '?':
					unknown_option_error_print(optopt);
					break;

			case ':':
					missing_argument_error_print(optopt);
					break;
					
			
		}	

	}
	


	//
	// check for option p if option o is present
	if(projadd->o_option_status == 1 && projadd->p_argument == -1)
		option_o_without_p_error_print();


	if( (temp_argc == 1 || (optind+1) > temp_argc))
	{
		invalid_cmdline_arg_error_print();
		goto label;	
	}
	else
		strcpy(projadd->project_name,argv[optind]);


	
	if(strcmp(projadd->f_argument,"\0") == 0)
		strcpy(projadd->f_argument,"/etc/project");
	
	
	project_file_checker(projadd->f_argument);


	projadd_fn(projadd);
	



label:

	free(projadd);
	if(count_of_K > 0)	
	{
		free(new);
		free(temp);
	}
	return 0;



}

