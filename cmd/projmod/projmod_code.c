#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "./headers/projmod_structure.h"
#include "./headers/projmod_error_msg.h"
#include "./headers/projmod_functions.h" 
#include "./headers/project_file_check.h"


int main(int argc,char** argv)
{

	char 	     optstring[] = ":aArsonl:c:f:G:K:p:U:";
	
	extern int   optind;
	int 	     temp_argc   = argc;
	int 	     optch;
	int 	     count_of_K  = 0;
	int	     nfA_flag    = -1;

	K_argument *new 	 = NULL;
	K_argument *temp 	 = NULL;
	projmod_options *projm = NULL;	
	optind			 = 1;

	projm = (projmod_options*)malloc(sizeof(projmod_options));

	init_structures(projm);		// Initializes all variables in the structures to 0 and NULL

	while((optch = getopt(argc,argv,optstring)) != -1)
	{
		switch(optch)
		{
			case 'f':
					
					if(projm->A_option_status == 0)
					{
						strcpy(projm->f_argument,optarg);//copying argument in the variable
						
					}
					else
						mutually_exclusive_Af_error_print();
					if(nfA_flag == -1)
						nfA_flag = 0;

					break;

			case 'c':       
					strcpy(projm->c_argument,optarg);	//copying argument in the variable
					nfA_flag = 1;
					break;
			
			case 'G':	
					strcpy(projm->G_argument,optarg);	//copying argument in the variable
					nfA_flag = 1;
					break;
		
			case 'U':
					strcpy(projm->U_argument,optarg);	//copying argument in the variable
					nfA_flag = 1;
					break;
			
			case 'p':
					
					projm->p_argument = atoi(optarg);	//copying argument in the variable
					nfA_flag = 1;
					break;

			case 'n':
					projm->n_option_status = 1;
					if(nfA_flag == -1)
						nfA_flag = 0;
					break;
		
			case 'o':
					projm->o_option_status = 1;
					nfA_flag = 1;
					break;

			case 'K':
					new     = (K_argument*)malloc(sizeof(K_argument));	//creating new structure
					new->next = NULL;				
					if(count_of_K == 0)					
					{
						projm->head = new;		//if first of LL then point head to it
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
					nfA_flag = 1;
					break;

			case 'a':
					if(projm->r_option_status != 1 && projm->s_option_status != 1)
						projm->a_option_status = 1;
					else
						mutually_exclusive_error_print();
					nfA_flag = 1;
					break;
		
			case 'r':
					if(projm->a_option_status != 1 && projm->s_option_status != 1)
						projm->r_option_status = 1;
					else
						mutually_exclusive_error_print();
					nfA_flag = 1;
					break;

			case 's':
					if(projm->r_option_status != 1 && projm->a_option_status != 1)
						projm->s_option_status = 1;
					else
						mutually_exclusive_error_print();
					nfA_flag = 1;
					break;

			case 'l':
					strcpy(projm->l_argument,optarg);
					nfA_flag = 1;
					break;

			case '?':
					unknown_option_error_print(optopt);
					break;

			case ':':
					missing_argument_error_print(optopt);
					break;
					
			case 'A':
					if(strcmp(projm->f_argument,"\0")  == 0)
						projm->A_option_status = 1;
					else
						mutually_exclusive_Af_error_print();
					if(nfA_flag == -1)
						nfA_flag = 0;
					break;
		}	



	}


	
	//
	// check for option p if option o is present
	if(projm->o_option_status == 1 )
	{
		if(projm->p_argument == -1)
			option_o_without_p_error_print();
	}


	//
	// Checnk for U , G , K if a , r or s is used
	if((projm->a_option_status == 1 || projm->r_option_status == 1 || projm->s_option_status == 1 ))
		if( ( (strcmp(projm->U_argument,"\0") == 0) && (strcmp(projm->G_argument,"\0") == 0) && (projm->head  == NULL)))
			option_ars_without_UGK_error_print(projm->a_option_status,projm->r_option_status,projm->s_option_status);



	if(temp_argc == 1)
	{
		project_file_checker("/etc/project");
		goto end;
	}
	else if(  strcmp(projm->c_argument,"\0") == 0  &&  strcmp(projm->l_argument,"\0") == 0  &&
		  strcmp(projm->G_argument,"\0") == 0  &&  strcmp(projm->U_argument,"\0") == 0  &&
		  projm->head == NULL	       &&  projm->p_argument == -1   && nfA_flag  == 1 )
	{
		missing_option_error_print();
		goto end;
	}
	else if( optind != (temp_argc-1))
	{
		if(nfA_flag == 1)
			missing_option_error_print();
		else if(nfA_flag == 0 && projm->A_option_status == 1)
			strcpy(projm->f_argument,"/etc/project");
	}
	else
		strcpy(projm->project_name,argv[optind]);



	projmod_fn(projm);

end :
	if(projm != NULL)
		free(projm);

	return 0;


}
