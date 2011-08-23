#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#include "./headers/pgstat_structure.h"
#include "./headers/pgstat_error_msg.h"
#include "./headers/pgstat_functions.h"





int main(int argc,char** argv)
{
	//
	// Declaring the variables
	//
	int 	optch;
	int	i;
	int	j;
	int	count	= 0;
	int	count1 	= 0;
	int	count2 	= 0;
	int	count3 	= 0;
	int	hy_flag = 0;
	int	from	= 0;
	int	to	= 0;
	int 	com_count = 0;
	int	temp_argc;

	extern int optind;
	
	char	optstring[] = ":ACpvs:S:t:T:r:R:P:c:";
	char*	num;
	char*	optarg_temp;

	num = (char*)malloc(sizeof(char)*20);
	optarg_temp = (char*)malloc(sizeof(char)*100);

	pgstat_options*	pgopt;



	//
	// Allocating space for the structure
	//
	pgopt = (pgstat_options*)malloc(sizeof(pgstat_options));



	//
	// Initializing the values
	//
	temp_argc = argc;
	optind	  = 1;
	pgstat_struct_init(pgopt);



	//
	// Processing the command line argument and options
	//
	while((optch = getopt(argc,argv,optstring)) != -1)
	{
		switch(optch)
		{
			case 'A':
					pgopt->A_option_status = 1;
					break;
			case 'C':
					pgopt->C_option_status = 1;
					break;
			case 'p':
					pgopt->p_option_status = 1;
					break;
			case 'h':
					pgstat_usage_error_print();
					exit(0);
			case 'v':
					pgopt->v_option_status = 1;
					break;
			case 'T':
					if(strcmp(optarg,"u") == 0 && strcmp(optarg,"d") == 0)
					{
						T_arg_error_print(optarg);	
						exit(3);
					}
					else
					{
						pgopt->T_argument = optarg[0];
						break;
					}
			case 't':
					if(atoi(optarg) == 0)
					{
						t_arg_error_print();
						exit(3);
					}
					else
					{
						pgopt->t_argument = atoi(optarg);
						break;
					}
			case 's':
					if(pgopt->S_argument != -1)
					{
						if(match_key(optarg) == -1)
						{
							sS_arg_error_print(optarg);
							exit(3);
						}	
						else
							pgopt->s_argument = match_key(optarg);
					}
					else
					{
						sS_error_print();
						exit(3);
					}
					break;
			case 'S':
					if(pgopt->s_argument != -1)
					{					
						if(match_key(optarg) == -1)
						{
							sS_arg_error_print(optarg);
							exit(3);
						}
						else
							pgopt->S_argument = match_key(optarg);
					}
					else
					{
						sS_error_print();
						exit(3);
					}
					break;
			case 'V':
					version_print();
					exit(0);
			case 'R':
					if(strcmp(pgopt->R_argument,"\0") == 0)
						strcpy(pgopt->R_argument,optarg);
					else
					{
						strcat(pgopt->R_argument,",");
						strcat(pgopt->R_argument,optarg);
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
			case 'c':
					count = 0;
					for(count1 = 0;optarg[count1] != '\0';count1++)
						if(optarg[count1] == ',')
							com_count++;

					strcpy(optarg_temp,optarg);
					for(count3 = 0;count3 <= com_count;count3++)
					{
						num = strsep(&optarg_temp,",");
						for(count1 =0;count1 < strlen(num);count1++)
							if(num[count1] == '-')
								hy_flag = 1;

						if(hy_flag == 1)
						{
							from 	= atoi(strsep(&num,"-"));
							to 	= atoi(strsep(&num,"-"));
						}
						else
                 					from = to = atoi(num);
						for(count2 = from; count2 <= to ; count2++)
							pgopt->c_argument[count++] = count2;
							
						hy_flag = 0;
					}					
					
					break;
			case 'P':
					count = 0;
					for(count1 = 0;optarg[count1] != '\0';count1++)
						if(optarg[count1] == ',')
							com_count++;

					strcpy(optarg_temp,optarg);
					for(count3 = 0;count3 <= com_count;count3++)
					{
						num = strsep(&optarg_temp,",");
						for(count1 =0;count1 < strlen(num);count1++)
							if(num[count1] == '-')
								hy_flag = 1;

						if(hy_flag == 1)
						{
							from 	= atoi(strsep(&num,"-"));
							to 	= atoi(strsep(&num,"-"));
						}
						else
                 					from = to = atoi(num);
						for(count2 = from; count2 <= to ; count2++)
							pgopt->P_argument[count++] = count2;
							
						hy_flag = 0;
					}					
					
					break;
			case ':':
					invalid_opt_arg_error_print(optopt);
					exit(3);
			case '?':
					invalid_option_error_print(optopt);
					exit(3);
		}
		if(num == NULL)
			num = (char*)malloc(sizeof(char)*20);
		if(optarg_temp == NULL)
			optarg_temp = (char*)malloc(sizeof(char)*100);

	}

	

	//
	// If interval and count value present then we would store it
	//
	if(temp_argc  - optind  == 1)
	{
		pgopt->interval = atoi(argv[optind]);
	}
	if(temp_argc - optind == 2)
	{
		pgopt->interval = atoi(argv[optind]);
		pgopt->count	= atoi(argv[optind+1]);
	}



	pgstat_fn(pgopt);




	return 0;

}
