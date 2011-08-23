#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "./headers/pginfo_error_msg.h"
#include "./headers/pginfo_structure.h"
#include "./headers/pginfo_functions.h"


int main(int argc,char** argv)
{
	int		count	= 0;
	int		count1	= 0;
	int		count2	= 0;
	int		count3	= 0;
	int		count4	= 0;
	int		from	= 0;
	int		to	= 0;
	int		hy_flag	= 0;
	int		com_count = 0;
	int		temp_argc;
	int 		optch;
	extern int 	optind;
	
	char		optstring[] = ":cChIpsTvr:R:";
	char*		num_char;
	char*		num;
	char*		temp;

	pginfo_options*	pgopt;

	pgopt	 = (pginfo_options*)malloc(sizeof(pginfo_options));
	num_char = (char*)malloc(sizeof(char)*50);
	num	 = (char*)malloc(sizeof(char)*20);
	temp	 = (char*)malloc(sizeof(char)*20);

	optind = 1;
	temp_argc = argc;
	pginfo_struct_init(pgopt);

	while((optch = getopt(argc,argv,optstring)) != -1)
	{
		switch(optch)
		{
			case 'c':
				pgopt->c_option_status  = 1;	
				break;

			case 'C':
				if(pgopt->I_option_status == 0 && (pgopt->s_option_status == 0 || pgopt->p_option_status == 0 || pgopt->T_option_status == 0))	
				{
					pgopt->C_option_status	= 1;
					break;
				}
				else
				{
					if(pgopt->s_option_status == 1 || pgopt->p_option_status == 1 || pgopt->T_option_status == 1)
					{	
						ICpsT_error_print();
						exit(1);
					}
					else
					{
						IC_error_print();
						exit(1);
					}

				}
				

			case 'h':
				pginfo_usage_error_print();
				exit(0);

			case 'I':
				if(pgopt->C_option_status == 0 && (pgopt->s_option_status == 0 || pgopt->p_option_status == 0 || pgopt->T_option_status == 0))	
				{
					pgopt->I_option_status = 1;
					break;
				}
				else
				{
					if(pgopt->s_option_status == 1 || pgopt->p_option_status == 1 || pgopt->T_option_status == 1)
					{	
						ICpsT_error_print();
						exit(1);
					}
					else
					{
						IC_error_print();
						exit(1);
					}
				}
				

			case 'p':
				if(pgopt->s_option_status == 0 && pgopt->C_option_status == 0 && pgopt->I_option_status == 0)
					pgopt->p_option_status = 1;
				else
				{
					if(pgopt->I_option_status == 1 || pgopt->C_option_status == 1)
						ICpsT_error_print();
					else
						ps_error_print();
					exit(1);
				}
				break;

			case 'r':
				if(strcmp(pgopt->r_argument,"\0") == 0)
					strcpy(pgopt->r_argument,optarg);
				else
				{
					strcat(pgopt->r_argument,",");
					strcat(pgopt->r_argument,optarg);
				}
				break;

			case 'R':
				if(strcmp(pgopt->R_argument,"\0") == 0)
					strcpy(pgopt->R_argument,optarg);
				else
				{
					strcat(pgopt->R_argument,",");
					strcat(pgopt->R_argument,optarg);
				}
				break;

			case 's':
				if(pgopt->p_option_status == 0 && pgopt->C_option_status == 0 && pgopt->I_option_status == 0 && pgopt->T_option_status == 0)
					pgopt->p_option_status = 1;
				else
				{
					if(pgopt->I_option_status == 1 || pgopt->C_option_status == 1)
						ICpsT_error_print();
					else if(pgopt->p_option_status == 1 && pgopt->T_option_status == 0)
						ps_error_print();
					else
						sT_error_print();
					exit(2);
				}
				break;

			case 'T':
				if(pgopt->C_option_status == 0 && pgopt->I_option_status == 0 && pgopt->s_option_status == 0)
					pgopt->T_option_status = 1;
				else
				{
					if(pgopt->I_option_status == 1 || pgopt->C_option_status == 1)
						ICpsT_error_print();
					else 
						sT_error_print();
					exit(2);
				}
				break;

			case 'v':
				pgopt->v_option_status = 1;
				break;

			case '?':
				invalid_option_error_print(optopt);
				exit(1);

			case ':':
				invalid_opt_arg_error_print(optopt);
				exit(1);
		}

	}


	if(optind == temp_argc)
		pgopt->argument[0] = -10;

	// Extracting pginfo arguments if given
	for(count1 = optind ; count1  < temp_argc ; count1++)
	{	
		if(strcmp(argv[count1],"all") == 0)
			pgopt->argument[count++] = -10;
		else
		{
			com_count = 0;
			strcpy(num_char,argv[count1]);
			for(count2 = 0; num_char[count2] != '\0';count2++)
				if(num_char[count2] == ',')
					com_count++;
	
			for(count2 = 0; count2 <= com_count; count2++)
			{
				num = strsep(&num_char,",");
				for(count3 = 0; count3 < strlen(num);count3++)
					if(num[count3] == '-')
						hy_flag = 1;			
				if(hy_flag == 0)
					pgopt->argument[count++] = atoi(num);
				else
				{
					from = atoi(strsep(&num,"-"));
					to   = atoi(strsep(&num,"-"));
				}
	
				if(hy_flag == 1)
				{
					for(count3 = from ; count3 <= to ; count3++)
						pgopt->argument[count++] = count3;
 				}
	
				hy_flag = 0;
			}
	
			if(num_char == NULL)
				num_char = (char*)malloc(sizeof(char)*50);
		}
	}


	pginfo_function(pgopt);


}
