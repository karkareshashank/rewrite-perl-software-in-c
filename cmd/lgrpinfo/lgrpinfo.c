#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "./headers/lgrp_structure.h"
#include "./headers/lgrp_error_msg.h"
#include "./headers/lgrpinfo_functions.h"


int main(int argc,char** argv)
{
	lgrp_options* lgrpopt;
	
	
	extern int optind;
	int optch;
	int temp_argc;
	int count = 0;
	int count1 = 0;
	int flag = 0;	

	char optstring[] = ":acCeGhIlLmPrtTu:";
	char* lgrp_arg;

	// Allocating space for the structures
	lgrpopt = (lgrp_options*)malloc(sizeof(lgrp_options)*1);
	lgrp_arg = (char*)malloc(sizeof(char)*100);
	
	// Initializing the sturcture
	init_structure(lgrpopt);
	optind = 1;
	temp_argc = argc;

	while((optch = getopt(argc,argv,optstring)) != -1)
	{	
		switch(optch)
		{
			case 'a':
					lgrpopt->a_option_status = 1;
					// For -t
					lgrpopt->t_option_status = 1;

					//For -c
					lgrpopt->c_option_status = 1;

					// For -e
					lgrpopt->e_option_status = 1;

					// For -m
					lgrpopt->m_option_status = 1;

					// For -r
					lgrpopt->r_option_status = 1;
		
					// For -l
					lgrpopt->l_option_status = 1;

					// For -L
					lgrpopt->L_option_status = 1;

					flag = 1;
					break;
			case 'c':
					lgrpopt->c_option_status = 1;
					flag = 1;
					break;
			case 'C':
					if(lgrpopt->P_option_status == 1  || lgrpopt->T_option_status == 1)
					{
						mutually_exclusive_error_print();
						goto exit;
					}
					else
						lgrpopt->C_option_status = 1;
					break;
			case 'e':
					lgrpopt->e_option_status = 1;
					flag = 1;
					break;
			case 'G':
					lgrpopt->G_option_status = 1;
					break;
			case 'h':
					lgrp_help_message();
					goto exit;
			case 'I':
					if(lgrpopt->L_option_status == 1)
					{
						cannot_be_used_with_error_print(optch,"L");
						goto exit;
					}
					else if(lgrpopt->T_option_status == 1)
					{
						cannot_be_used_with_error_print('T',"I,t");
						goto exit;
					}
					else
						lgrpopt->I_option_status = 1;
					break;
			case 'l':
					lgrpopt->l_option_status = 1;
					flag = 1;
					break;
			case 'L':
					if(lgrpopt->I_option_status == 1)
					{
						cannot_be_used_with_error_print(optch,"I");
						goto exit;
					}
					else
						lgrpopt->L_option_status = 1;
					break;
			case 'm':
					lgrpopt->m_option_status = 1;
					flag = 1;
					break;
			case 'P':
					if(lgrpopt->C_option_status == 1 || lgrpopt->T_option_status == 1)
					{
						mutually_exclusive_error_print();
						goto exit;
					}
					else
						lgrpopt->P_option_status = 1;
					break;
			case 'r':
					lgrpopt->r_option_status = 1;
					flag = 1;
					break;
			case 't':
					if(lgrpopt->T_option_status == 1)
					{
						cannot_be_used_with_error_print('T',"I,t");
						goto exit;
					}
					else
						lgrpopt->t_option_status = 1;
					flag = 1;
					break;
			case 'T':
					if(lgrpopt->C_option_status == 1 || lgrpopt->P_option_status == 1)
					{
						mutually_exclusive_error_print();
						goto exit;
					}
					else if(lgrpopt->I_option_status == 1 || lgrpopt->t_option_status == 1)
					{
						cannot_be_used_with_error_print('T',"I,t");
						goto exit;
					}
					else
						lgrpopt->T_option_status = 1;
					break;
			case 'u':
					sscanf(optarg,"%c",lgrpopt->u_option_argument);
					break;
			case '?':
					printf("lgrpinfo : Unknown option '%c'\n",optopt);
					lgrp_usage_error_print();
					exit(1);
			
		}

	}


	// Finding out the argument to the command
	 if(optind == 1 || (flag == 0 && lgrpopt->I_option_status == 0 && lgrpopt->T_option_status == 0))	// If no options are used then by 
	{									//default c,e,l,m,r,t are used
		lgrpopt->c_option_status = 1;
		lgrpopt->e_option_status = 1;
		lgrpopt->l_option_status = 1;
		lgrpopt->m_option_status = 1; 
		lgrpopt->r_option_status = 1;
		lgrpopt->t_option_status = 1;
	}	
	
	if(temp_argc > optind)
	{
		for(count = optind;count < temp_argc;count++)
		{
			if(strcmp(argv[count],"all") == 0)
				lgrpopt->lgrp_argument[0] = -1;
			else if(strcmp(argv[count],"intermediate") == 0)
				lgrpopt->lgrp_argument[0] = -2;
			else if(strcmp(argv[count],"leaves") == 0)
				lgrpopt->lgrp_argument[0] = -3;
			else if(strcmp(argv[count],"root") == 0)
				lgrpopt->lgrp_argument[0] = -4;
			else
			{
				lgrpopt->lgrp_argument[count1++] = atoi(argv[count]);
			}
			
		}

	}
	else if(temp_argc == optind)
		lgrpopt->lgrp_argument[0] = -1;



//	for(count = 0;lgrpopt->lgrp_argument[count] != -5;count++)
//		printf("%d\t",lgrpopt->lgrp_argument[count]);
	
	
	lgrpinfo_fn(lgrpopt);


exit:	
	
	free(lgrpopt);
	free(lgrp_arg);
	return 0;


}
