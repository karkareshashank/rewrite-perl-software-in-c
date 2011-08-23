#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <kstat.h>

#include "./pginfo_functions.h"
#include "./pginfo_structure.h"




// This function will fill the space in the relationship with '_'
void fill_space(char* relation)
{
	int len	= 0;
	int i	= 0;

	len = strlen(relation);
	
	for(i = 0 ;i < len ; i++)
	{
		if(relation[i] == ' ')
			relation[i] = '_';
	}


}







// This function will checke if a given relationship name exists in the argument provided by r, R
int match_rel(char* argument , char* shname)
{
	int com_count	= 0;			// Counts no of ',' in the string
	int count1	= 0;			
	int count2	= 0;
	int flag 	= 0;			// 1 - if relationship name matches else 0
	
	char* temp_shname;			// To store relationship extracted from argument variable
	char* temp_arg;				// To store argument string given by user

	// Allocating space for the variables
	temp_arg 	= (char*)malloc(sizeof(char)*100);
	temp_shname 	= (char*)malloc(sizeof(char)*50 );

	// Copying argument string into some temporary variable
	strcpy(temp_arg,argument);

	// Counting no of ,'s in the string
	for(count1 =0 ;argument[count1] != '\0';count1++)
		if(argument[count1] == ',')
			com_count++;

	// Matching each relationship name given by the user with the other string
	for(count1 = 0 ; count1 <= com_count ; count1++)
	{
		temp_shname  = strsep(&temp_arg,",");
		if(strcmp(temp_shname , shname) == 0)
			flag = 1;
	}


	return flag;
}








// This function will match pg_arg_list to pg_id of a particular PG
int match_id(int pg_arg_list[] , int pg_id)
{
	int count1	= 0;
	int flag	= 0;

	for(count1 = 0; pg_arg_list[count1] != -1;count1++)
	{
		if(pg_arg_list[count1] == pg_id)
			flag = 1;
	}

	return flag;

}


// This function will match cpu_arg_list to cpu_list of a particular PG
int match_list(int cpu_arg_list[] , int cpu_list[],int ncpus)
{
	int count1	 = 0;
	int count2	 = 0;
	int flag	 = 0;

	for(count1 = 0; cpu_arg_list[count1] != -1;count1++)
	{
		for(count2 = 0;count2 < ncpus;count2++)
		{

			if(cpu_arg_list[count1] == cpu_list[count2])
				return 1;
		}
	}

	return flag;

}



// Function for qsort
static int intcompare(const void *p1, const void *p2)
{
           int i = *((int *)p1);
           int j = *((int *)p2);

           if (i > j)
               return (1);
           if (i < j)
               return (-1);
           return (0);
}





// returns the specific interger value from knp data structure
int selected_value(kstat_named_t* knp)
{

	if(knp->data_type == KSTAT_DATA_INT32)
		return knp->value.i32;
	else if(knp->data_type == KSTAT_DATA_UINT32)
		return knp->value.ui32;
	else if(knp->data_type == KSTAT_DATA_INT64)
		return knp->value.i64;
	else if(knp->data_type == KSTAT_DATA_UINT64)
		return knp->value.ui64;

}





// This function will fill the given empty array with the cpus list provided as a string
void cpu_list_fill(char* cpu_list , int* cpus)
{

	int 	com_count 	= 0;		// Counts the no of ,'s in the string
	int	count1		= 0;
	int	count2		= 0;
	int	hy_flag		= 0;		// 1 - if hyphen exists in the string ,  else 0
	int	cpu_count	= 0;		// Counts no of cpus
	int	to		= 0;		// min_cpu id 
	int	from		= 0;		// max_cpu id

	char*	temp_cpu_list;			// Stores the given cpu_list string
	char*	cpu_num;			// Stores the cpu_ids between two ,'s

	// Allocate the memory for the variables
	temp_cpu_list	= (char*)malloc(sizeof(char)*1024);
	cpu_num		= (char*)malloc(sizeof(char)*10);

	// Copying the cpu_list into the temporary variable to work upon
	strcpy(temp_cpu_list,cpu_list);
	
	// Counting no of ,'s in the string
	for(count1 = 0 ; count1 < strlen(temp_cpu_list) ; count1++)
		if(temp_cpu_list[count1] == ',')
			com_count++;


	// For each of the string tokenized by , will be processed to extract the cpu_ids from it and store it in the 
	// cpus arrary as an integer
	for(count1 = 0; count1 <= com_count;count1++)
	{
		cpu_num = strsep(&temp_cpu_list,",");
		for(count2 = 0;count2 < strlen(cpu_num);count2++)
			if(cpu_num[count2] == '-')
				hy_flag = 1;		
			
		if(hy_flag == 0)
		{
			cpus[cpu_count] = atoi(cpu_num);
			cpu_count++;
		}
		else if(hy_flag == 1)
		{
			from 	= atoi(strsep(&cpu_num,"-"));
			to	= atoi(cpu_num);

			for(count2 = from; count2 <= to;count2++)
			{
				cpus[cpu_count] = count2;
				cpu_count++;
			}


		}
		
		hy_flag = 0;
	}


	if(temp_cpu_list != NULL)
		free(temp_cpu_list);
	
	if(cpu_num != NULL)
		free(cpu_num);

}






//
// Fucntion to check whether kc is successfully returned or not
void error_check_kstat_ctl_t(kstat_ctl_t *kc)
{
	if(kc == NULL)
	{
		if(errno == ENOMEM)
			fprintf(stderr,"Insufficient storage space is available\n");
		else if(errno == EAGAIN)
			fprintf(stderr,"The kstat was temporarily unavailable for reading or writing\n");
		else if(errno == ENXIO)
			fprintf(stderr,"The given kstat could not be located for reading\n");
		else if(errno == EOVERFLOW)
			fprintf(stderr,"The data for the given kstat was too large to be stored in the structure.\n");
		exit(1);
	}

}



//
// Function to check whether ksp is successfully returned or not
void error_check_kstat_t(kstat_t *ksp)
{
	if(ksp == NULL)
	{
		if(errno == EINVAL)
			fprintf(stderr,"An attempt was made to look up data for a kstat that was not of\
					type KSTAT_TYPE_NAMED or KSTAT_TYPE_TIMER\n");
		else if(errno == ENOENT)
			fprintf(stderr,"ksp : The request kstat could not be found\n");

		exit(1);
	}


}






//
// Function to check whether knp is successfully returned or not
void error_check_kstat_named_t(kstat_named_t *knp)
{
	if(knp == NULL)
	{
		if(errno == EINVAL)
			fprintf(stderr,"An attempt was made to look up data for a kstat that was not of\
					type KSTAT_TYPE_NAMED or KSTAT_TYPE_TIMER\n");
		else if(errno == ENOENT)
			fprintf(stderr,"knp : The request kstat could not be found\n");

		exit(1);
	}


}




pg_ks* create_pg_from_kstat(pg_ks* head)
{
	kstat_ctl_t*	kc;
	kstat_t*	ksp;
	kstat_t*	kc_temp;
	kstat_named_t*	knp;

	pg_ks*		new;
	pg_ks*		temp;

	int		count1	= 0;

	char*		cpus_char;


	kc_temp = kc->kc_chain;
	cpus_char = (char*)malloc(sizeof(char)*50);
	new = (pg_ks*)malloc(sizeof(pg_ks));
	new->next = NULL;	

	// Open the kstat to access the kstat datas
	kc = kstat_open();
	error_check_kstat_ctl_t(kc);

	// Reach to the pg_hw_perf kstat to access the data
	ksp = kstat_lookup(kc,"pg_hw_perf",-1,NULL);
	error_check_kstat_t(ksp);

	while(ksp != NULL)
	{
		// look for specific data in the given kstat 
		kstat_read(kc,ksp,NULL);

		
		// For PG id 
		knp = kstat_data_lookup(ksp,"pg_id");
		error_check_kstat_named_t(knp);
		new->pg_id = selected_value(knp);

		// For parent PG id
		knp = kstat_data_lookup(ksp,"parent_pg_id");
		error_check_kstat_named_t(knp);
		new->parent_pg_id = selected_value(knp);

		// For ncpus
		knp = kstat_data_lookup(ksp,"ncpus");
		error_check_kstat_named_t(knp);
		new->ncpus = selected_value(knp);

		// For Relationship
		knp = kstat_data_lookup(ksp,"relationship");
		error_check_kstat_named_t(knp);
		sprintf(new->relationship,"%s",knp->value);
		fill_space(new->relationship);

		// For CPUs
		knp = kstat_data_lookup(ksp,"cpus");
		error_check_kstat_named_t(knp);
		sprintf(cpus_char,"%s",knp->value);
		sprintf(new->char_cpus,"%s",knp->value);
		cpu_list_fill(cpus_char, new->cpus);

		
		if(count1 == 0)
		{
			head = new;
			temp = new;
		}
		else
		{
			temp->next = new;
			temp	   = new;
		}


		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"pg_hw_perf",-1,NULL);
		new = (pg_ks*)malloc(sizeof(pg_ks));
		new->next = NULL;
		count1++;
	}
	

	if(new != NULL)
		free(new);

	kstat_close(kc);

	return head;

}




// This function will create root PG and add it to the top of the list
void create_root_pg(pginfo_options* pgopt)
{
	kstat_ctl_t*	kc;
	kstat_t*	ksp;
	kstat_t*	kc_temp;
	kstat_named_t*	knp;

	pg_ks*	new;
	pg_ks* 	temp;
	
	int	ncpus	= 0;
	int	count	= 0;

	// Allocating space
	new = (pg_ks*)malloc(sizeof(pg_ks));
	new->next = NULL;

	// PG id , parent_pg ID , relationship
	new->pg_id = 0;
	new->parent_pg_id = -1;
	sprintf(new->relationship,"%s","System");

	// For CPUs
	for(count = 0;count < 1024;count++)
		new->cpus[count] = -1;

		// Opening kstat
	kc = kstat_open();
	error_check_kstat_ctl_t(kc);
	

		// looking for "pg_hw_perf_cpu" kstat
	ksp = kstat_lookup(kc,"pg_hw_perf_cpu",-1,NULL);
	error_check_kstat_t(ksp);

	while(ksp != NULL)
	{
		kstat_read(kc,ksp,NULL);

		knp = kstat_data_lookup(ksp,"cpu_id");
		error_check_kstat_named_t(knp);
		new->cpus[ncpus++] = selected_value(knp);

		
		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"pg_hw_perf_cpu",-1,NULL);
	}

	sprintf(new->char_cpus,"%d-%d",new->cpus[0],new->cpus[ncpus-1]);

	kstat_close(kc);

	// Changing the parent id before adding root to the list
	temp = pgopt->pg_ks_head;
	while(temp != NULL)
	{
		if(temp->parent_pg_id == -1)
			temp->parent_pg_id = 0;
		temp = temp->next;
	}

	// ncpus
	new->ncpus = ncpus;

	
	// Adding it in the linklist
	new->next = pgopt->pg_ks_head;
	pgopt->pg_ks_head = new;


}





// This will remove all the duplicate and also will sort them in ascending order
void sort_and_remove_dup(pginfo_options* pgopt)
{
	int count1	= 0;
	int count2	= 0;
	int count3	= 0;
	int max		= 0;
	int num		= 0;
	
	for(count1 = 0;pgopt->argument[count1] != -1;count1++);
	
	// Sorting the array of argument 
	qsort((void*)(pgopt->argument),count1,sizeof(int),intcompare);


	// Finding max id
	for(count2 = 0; count2 < count1 ; count2++)
	{
		if(pgopt->argument[count2] > max)
			max = pgopt->argument[count2];
	}

	// replacing the duplicate with the max+10 value so that when we sort the array it goes to the end
	num = pgopt->argument[0];
	for(count2 = 1;count2 < count1 ;count2++)
	{
		if(pgopt->argument[count2] != num)
			num = pgopt->argument[count2];
		else
			pgopt->argument[count2] = max+10;
	}	

	// Sorting the array of argument 
	qsort((void*)(pgopt->argument),count1,sizeof(int),intcompare);

	// Removing all the argument greater than max because they are the repeated ones
	count3 = count1;
	for(count2 = 0;count2 < count1;count2++)
	{
		if(pgopt->argument[count2] > max)
		{
			pgopt->argument[count2] = -1;
			count3--;
		}
	}
	count1 = count3;

	
}




// This functions will check all the arguments provided and give error and remove those which are invalid
void check_arg_for_error_remove(pginfo_options* pgopt)
{
	pg_ks*	temp;

	int max		= 0;
	int count1	= 0;
	int count3	= 0;
	int count	= 0;
	int count2	= 0;
	int match_flag	= 0;
	int invalid[1024];

	for(count1 = 0;count1 < 1024;count1++)
		invalid[count1] = -1;

	temp = pgopt->pg_ks_head;

	for(count1 = 0; pgopt->argument[count1] != -1;count1++)
	{
		if(pgopt->argument[count1] == -10)
			continue;
		if(pgopt->c_option_status == 1)
		{
			for(count2= 0; count2 < temp->ncpus ;count2++)
				if(temp->cpus[count2] == pgopt->argument[count1])
					match_flag = 1;

			if(match_flag == 0)
				invalid[count++] = pgopt->argument[count1];
			match_flag = 0;
		}
		else
		{
			while(temp != NULL)
			{
				if(temp->pg_id == pgopt->argument[count1])
					match_flag = 1;

				temp = temp->next;
			}			
			if(match_flag == 0)
				invalid[count++] = pgopt->argument[count1];
			match_flag = 0;
			temp = pgopt->pg_ks_head;

		}
	}

	// Find the maximum
	for(count1 = 0;pgopt->argument[count1] != -1;count1++)
		if(pgopt->argument[count1] > max)
			max = pgopt->argument[count1];


	// changing the invalid ids from the argument list
	for(count1 = 0;count1 < count ;count1++)
	{
		for(count2 = 0 ;pgopt->argument[count2] != -1;count2++)
		{
			if(pgopt->argument[count2] == invalid[count1])
			{
				pgopt->argument[count2] = max+10;
				count3++;
			}
		}
	}

	// sorting the array
	qsort((void*)pgopt->argument,count1,sizeof(int),intcompare);
	
	// Removing the invalid argument
	for(count1 = 0;pgopt->argument[count1] != -1;count1++)
		if(pgopt->argument[count1] > max)
			pgopt->argument[count1] = -1;
	
	// Print the error
	if(invalid[0] != -1)
	{
		if(pgopt->c_option_status == 1)
			printf("pginfo: Invalid Processor IDs ");
		else
			printf("pginfo: Invalid PG IDs ");
		for(count1 =0 ;invalid[count1] != -1;count1++)
			printf("%d ",invalid[count1]);
		printf("\n");
	
		if(pgopt->argument[0] == -1)
		{
			printf("pginfo: no processor groups matching command line arguments\n");
			exit(1);
		}
	}
}







// This function will keep only thos pg node whose cpu id matches th argument
void c_option_function(pginfo_options* pgopt)
{
	pg_ks* 	temp;
	pg_ks*	pre_temp;

	int	count	= 0;
	
	temp 		= pgopt->pg_ks_head;
	pre_temp 	= pgopt->pg_ks_head;

	// Checking whether there is -10 at argument[0]
	if(pgopt->argument[0] == -10)
		return;

	// If -10 is not at argument[0]
	while(temp != NULL)
	{
		if(count == 0)
		{
			if(match_list(pgopt->argument,temp->cpus,temp->ncpus) == 0)
			{
				pgopt->pg_ks_head = temp->next;
				temp 	 = pgopt->pg_ks_head;
				pre_temp = pgopt->pg_ks_head;
				count--;
			}
			else
			{
				temp = temp->next;
				pre_temp = pgopt->pg_ks_head;
			}
		}
		else
		{
			if(match_list(pgopt->argument,temp->cpus,temp->ncpus) == 0)
			{
				pre_temp->next  = temp->next;
				temp 		= pre_temp->next;
			}
			else
			{	
				temp = temp->next; 
				pre_temp = pre_temp->next;
			}

		}


		count++;
	}

}






// This function will remove all those PGs whose pg_id does not match the given ids
void noc_option_function(pginfo_options* pgopt)
{
	pg_ks*	temp;
	pg_ks*	pre_temp;

	int	count	= 0;

	temp 	 = pgopt->pg_ks_head;
	pre_temp = pgopt->pg_ks_head;

	// Checking whether there is -10 at argument[0]
	if(pgopt->argument[0] == -10)
		return;


	while(temp != NULL)
	{
		if(count == 0)
		{
			if(match_id(pgopt->argument,temp->pg_id) == 0)
			{
				pgopt->pg_ks_head = temp->next;
				temp 	 = pgopt->pg_ks_head;
				pre_temp = pgopt->pg_ks_head;
				count--;
			}
			else
			{
				temp = temp->next;
				pre_temp = pgopt->pg_ks_head;
			}
		}
		else
		{
			if(match_id(pgopt->argument,temp->pg_id) == 0)
			{
				pre_temp->next  = temp->next;
				temp 		= pre_temp->next;
			}
			else
			{	
				temp = temp->next; 
				pre_temp = pre_temp->next;
			}

		}
	

		count++;
	}


}







// This function will keep  the PGs whos Relationship does not match the argument of R option
void R_option_function(pginfo_options* pgopt)
{
	pg_ks*	temp;
	pg_ks*	pre_temp;

	int	count	= 0;

	temp 	 = pgopt->pg_ks_head;
	pre_temp = pgopt->pg_ks_head;

	

	while(temp != NULL)
	{
		if(count == 0)
		{
			if(match_rel(pgopt->R_argument,temp->relationship) == 0)
			{
				pgopt->pg_ks_head = temp->next;
				temp 	 = pgopt->pg_ks_head;
				pre_temp = pgopt->pg_ks_head;
				count--;
			}
			else
			{
				temp = temp->next;
				pre_temp = pgopt->pg_ks_head;
			}
		}
		else
		{
			if(match_rel(pgopt->R_argument,temp->relationship) == 0)
			{
				pre_temp->next  = temp->next;
				temp 		= pre_temp->next;
			}
			else
			{	
				temp = temp->next; 
				pre_temp = pre_temp->next;
			}

		}
	

		count++;
	}

}








// This function will keep  the PGs whos Relationship  match the argument of r option
void r_option_function(pginfo_options* pgopt)
{
	pg_ks*	temp;
	pg_ks*	pre_temp;

	int	count	= 0;

	temp 	 = pgopt->pg_ks_head;
	pre_temp = pgopt->pg_ks_head;

	while(temp != NULL)
	{
		if(count == 0)
		{
			if(match_rel(pgopt->r_argument,temp->relationship) == 1)
			{
				pgopt->pg_ks_head = temp->next;
				temp 	 = pgopt->pg_ks_head;
				pre_temp = pgopt->pg_ks_head;
				count--;
			}
			else
			{
				temp = temp->next;
				pre_temp = pgopt->pg_ks_head;
			}
		}
		else
		{
			if(match_rel(pgopt->r_argument,temp->relationship) == 1)
			{
				pre_temp->next  = temp->next;
				temp 		= pre_temp->next;
			}
			else
			{	
				temp = temp->next; 
				pre_temp = pre_temp->next;
			}

		}
	

		count++;
	}

}




// This function will display the output of the pginfo 
void pginfo_display_function(pginfo_options* pgopt)
{
	int	count	= 0;
	int	count1	= 0;
	int	max	= 0;
	int	num	= 0;
	int	pg_id[1024];
	int	cpus[1024];

	pg_ks*	temp;

	// Initializing the array to -1
	for(count = 0;count < 1024;count++)
		pg_id[count] = cpus[count] = -1;

	
	temp = pgopt->pg_ks_head;

	// If -I option is used
	if(pgopt->I_option_status == 1)
	{
		count = 0;
		while(temp != NULL)
		{
			pg_id[count++] = temp->pg_id;
			temp = temp->next;
		}
		qsort((void*)pg_id,count,sizeof(int),intcompare);
		for(count = 0;pg_id[count] != -1; count++)
			printf("%d ",pg_id[count]);
		printf("\n");
	}
	else if(pgopt->C_option_status == 1)
	{
		count = 0;
		temp = pgopt->pg_ks_head;
		while(temp != NULL)
		{
			for(count1 = 0; count1 < temp->ncpus ;count1++)
				cpus[count++] = temp->cpus[count1];
			temp = temp->next;
		}

		// Sorting 
		qsort((void*)cpus,count,sizeof(int),intcompare);
		// Finding maximum cpu id
		for(count1 =0 ;count1 < count;count1++)
			if(cpus[count1] > max)
				max = cpus[count1];

		// Replacing all duplicate cpus ids with max+10
		num  = cpus[0];
		for(count1 = 1 ; count1 < count; count1++)
		{
			if(cpus[count1] == num )
				cpus[count1] = max + 10;
			else
				num = cpus[count1];

		}

		// Again sort and then  replace all the values greater than max with -1
		qsort((void*)cpus,count,sizeof(int),intcompare);
		for(count1 = 0;count1 < count ; count1++)
			if(cpus[count1] > max)
				cpus[count1] = -1;

		for(count1 = 0; cpus[count1] != -1;count1++)
			printf("%d ",cpus[count1]);
		printf("\n");

	}
	else if(pgopt->s_option_status == 1)
	{
		count 	 = 0;
		temp	 = pgopt->pg_ks_head;

		while(temp != NULL)
		{
			printf("%s\n",temp->relationship);
			temp = temp->next;
		}
	}
	else if(pgopt->p_option_status == 1 && pgopt->T_option_status == 0)
	{
		count	 = 0;
		temp	 = pgopt->pg_ks_head;

		
	}
	else if(pgopt->p_option_status == 1 && pgopt->T_option_status == 1)
	{

	}
	else if(pgopt->p_option_status == 0 && pgopt->T_option_status == 1)
	{


	}
	else
	{
		

	}


}








// This function will call all other functions to do the process
void pginfo_function(pginfo_options* pgopt)
{
	int count1	= 0;

	pg_ks*		temp;

	
	// This will create the PG linklist from the kstat data
	pgopt->pg_ks_head = create_pg_from_kstat(pgopt->pg_ks_head);
	
	// This will create the root PG and add it to the top of the PG linklist
	create_root_pg(pgopt);

	// This will sort and remove the duplicate ids given in the argument
	sort_and_remove_dup(pgopt);

	// This will check each id , if it is valid or not if not it will report an error and remove it from the argument list
	check_arg_for_error_remove(pgopt);

	// If -c option is used then the given ids will be treated as CPU IDs else as PG IDs
	if(pgopt->c_option_status == 1)
		c_option_function(pgopt);
	else
		noc_option_function(pgopt);

	// If -R options is used then we remove those PG whose relationship name does not matches the -R argument
	if(strcmp(pgopt->R_argument,"\0") != 0)
		R_option_function(pgopt);

	// If -r options is used then we remove those PG whose relationship name  matches the -r argument
	if(strcmp(pgopt->r_argument,"\0") != 0)
		r_option_function(pgopt);

	
	pginfo_display_function(pgopt);	



	temp = pgopt->pg_ks_head;

	printf("REsults \n");
	while(temp != NULL)
	{
		printf("%d -- %d -- %d -- %s -- %s \n",temp->pg_id,temp->parent_pg_id,temp->ncpus,temp->char_cpus,temp->relationship);
		temp = temp->next;
	}
	printf("\n");	

}
