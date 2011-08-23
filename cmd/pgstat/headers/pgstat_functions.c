#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <kstat.h>
#include <time.h>

#include "pgstat_structure.h"
#include "pgstat_functions.h"


#define VERSION 	1.1
#define	HWLOAD_UNKNOWN	-1
#define	HWLOAD_UNDEF	-2
#define LEVEL_OFFSET	1 
#define RETRY_DELAY	0.25

int	DEFAULT_INTERVAL = 1;
int	DEFAULT_COUNT	 = 1;
int	RETRY_COUNT	 = 4;




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
int search_rel(char* shname , char* argument)
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




// This function will check if a given number exits in the integer array
int search_num(int num , int list[] )
{
	int i =0;
	int flag = 0;			// 1 -  if match found ,0 - if not found

	// Checks for the number in the array
	for(i =0; list[i] != -1;i++)
	{
		if(num == list[i])
			flag = 1;		
	}

	return flag;
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
			fprintf(stderr,"The request kstat could not be found\n");

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
			fprintf(stderr,"The request kstat could not be found\n");

		exit(1);
	}


}








// This functions fills the pg_ks structure with the data taken from the kstat "pg_hw_perf"
pg_ks* pg_create_from_kstat(pg_ks* head)
{
	kstat_ctl_t*	kc;		
	kstat_t*	ksp;
	kstat_named_t*	knp;
	
	pg_ks*		pg;
	pg_ks*		temp;

	char		cpu_list[1024];
	char		cpu_id[10];

	int		count1		= 0;


	// Opening kstat to access the data
	kc = kstat_open();
	error_check_kstat_ctl_t(kc);

	// looking for pg_hw_perf kstat
	ksp = kstat_lookup(kc,"pg_hw_perf",-1,NULL);
	error_check_kstat_t(ksp);

	pg	= (pg_ks*)malloc(sizeof(pg_ks));

	pg->next = NULL;
	
	while(ksp != NULL)
	{
		kstat_read(kc,ksp,NULL);

		// Finding pg_id
		knp = kstat_data_lookup(ksp,"pg_id");
		error_check_kstat_named_t(knp);
		pg->pg_id = selected_value(knp);

		// Finding ncpus
		knp = kstat_data_lookup(ksp,"ncpus");
		error_check_kstat_named_t(knp);
		pg->ncpus = selected_value(knp);

		// Finding cpus
		knp = kstat_data_lookup(ksp,"cpus");
		error_check_kstat_named_t(knp);	
		sprintf(cpu_list,"%s",knp->value);
		cpu_list_fill(cpu_list,pg->cpus);

		// Finding hw_util
		knp = kstat_data_lookup(ksp,"hw_util");
		error_check_kstat_named_t(knp);
		pg->hw_util = selected_value(knp);

		// Finding hw_util_rate
		knp = kstat_data_lookup(ksp,"hw_util_rate");
		error_check_kstat_named_t(knp);
		pg->hw_util_rate = selected_value(knp);

		// Finding hw_util_rate_max
		knp = kstat_data_lookup(ksp,"hw_util_rate_max");
		error_check_kstat_named_t(knp);
		pg->hw_util_rate_max = selected_value(knp);

		// Findind hw_util_time_running
		knp = kstat_data_lookup(ksp,"hw_util_time_running");
		error_check_kstat_named_t(knp);
		pg->hw_util_time_running = selected_value(knp);

		// Finding hw_util_time_stopped
		knp = kstat_data_lookup(ksp,"hw_util_time_stopped");
		error_check_kstat_named_t(knp);	
		pg->hw_util_time_stopped = selected_value(knp);

		// Finding snaptime
//		knp = kstat_data_lookup(ksp,"snaptime");
//		printf("eer\n");
//		error_check_kstat_named_t(knp);	
//		pg->snaptime	= selected_value(knp);

		// Finding generation
		knp = kstat_data_lookup(ksp,"generation");
		error_check_kstat_named_t(knp);
		pg->generation	 = selected_value(knp);

		// Finding relationship / shared relation
		knp = kstat_data_lookup(ksp,"relationship");
		error_check_kstat_named_t(knp);
		sprintf(pg->relationship,"%s",knp->value);

		// Finding parent_pg_id
		knp = kstat_data_lookup(ksp,"parent_pg_id");
		error_check_kstat_named_t(knp);
		pg->parent_pg_id = selected_value(knp);


		if(count1 == 0)
		{
			temp = pg;
			head = temp;
		}
		else
		{
			temp->next = pg;
			temp 	   = pg;
		}

		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"pg_hw_perf",-1,NULL);
		pg  = (pg_ks*)malloc(sizeof(pg_ks));
		pg->next = NULL;
		count1++;
	}

	kstat_close(kc);

	
	if(pg != NULL)
		free(pg);
	
	return head;
}





void version_print()
{

}


//
// This function matches the sort key with the pre-defined keys and if keys matches one of them then 
// it returns a number (1-7) else returns -1 to indicate error
int match_key(char* string)
{
	if(strcmp(string,"pg") == 0)
		return 1;
	else if(strcmp(string,"hwload") == 0)
		return 2;
	else if(strcmp(string,"swload") == 0)
		return 3;
	else if(strcmp(string,"user")	== 0)
		return 4;
	else if(strcmp(string,"sys")	== 0)
		return 5;
	else if(strcmp(string,"idle")	== 0)
		return 6;
	else if(strcmp(string,"depth")	== 0)
		return 7;
	else if(strcmp(string,"breadth")== 0)
		return 8;
	else
		return -1;

}



// This  function will store cpu_data from the kstat ( cpu::sys:)
cpu_data* cpu_data_create(cpu_data* head)
{

	kstat_ctl_t*	kc;
	kstat_t*	ksp;
	kstat_named_t*	knp;

	cpu_data*	new;
	cpu_data*	temp;

	int		count1	= 0;
	int		count2	= 0;

	new = (cpu_data*)malloc(sizeof(cpu_data));
	new->next = NULL;

	kc = kstat_open();
	error_check_kstat_ctl_t(kc);

	ksp = kstat_lookup(kc,"cpu",-1,"sys");
	error_check_kstat_t(ksp);

	while(ksp != NULL)
	{
		kstat_read(kc,ksp,NULL);

		// Finding cpu_id assumed as instance id
		new->cpu_id	= ksp->ks_instance;
		
		// Finding cpu_ticks_idle
		knp = kstat_data_lookup(ksp,"cpu_ticks_idle");
		error_check_kstat_named_t(knp);
		new->cpu_ticks_idle = selected_value(knp);

		// Finding cpu_ticks_user
		knp = kstat_data_lookup(ksp,"cpu_ticks_user");
		error_check_kstat_named_t(knp);
		new->cpu_ticks_user = selected_value(knp);

		// Finding cpu_ticks_kernel
		knp = kstat_data_lookup(ksp,"cpu_ticks_kernel");
		error_check_kstat_named_t(knp);
		new->cpu_ticks_kernel = selected_value(knp);

		if(count1 == 0)
		{
			temp = new;
			head = new;
		}
		else
		{
			temp->next = new;
			temp	   = new;
		}


		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu",-1,"sys");

		new = (cpu_data*)malloc(sizeof(cpu_data));
		new->next = NULL;
		count1++;
	}

	kstat_close(kc);


	if(new != NULL)
		free(new);

	return head;
}


//
// This function will get the PG_id , Relationship , Cpus from the two pg snapshot
pg* get_pg_rel_cpus(pg_ks* pg_ks_head1 , pg_ks* pg_ks_head2 , pg* pg_head)
{

	pg*	new;
	pg*	temp;

	pg_ks*		pg_ks_temp1;
	pg_ks*		pg_ks_temp2;

	int		count1	= 0;
	int		count2	= 0;
	int		count3  = 0;
	int		from;
	int		to;
	int		cont_flag = 0;

	char*		str_cpu;
	char*		cpu;

	new	= (pg*)malloc(sizeof(pg));
	str_cpu = (char*)malloc(sizeof(char)*2048);
	cpu	= (char*)malloc(sizeof(char)*20);

	init_pg(new);
	pg_ks_temp1 = pg_ks_head1;
	pg_ks_temp2 = pg_ks_head2;

	

	
	while(pg_ks_temp1 != NULL || pg_ks_temp2 != NULL)
	{
		// PG id
		new->pgid   = pg_ks_temp1->pg_id;

		// Parent_pg_id
		new->parent_pg_id = pg_ks_temp1->parent_pg_id;

		// Relationship
		fill_space(pg_ks_temp1->relationship);
		strcpy(new->relationship,pg_ks_temp1->relationship);
		

		// CPUs
		if(pg_ks_temp1->cpus[pg_ks_temp1->ncpus -1] - pg_ks_temp1->cpus[0] == pg_ks_temp1->ncpus-1)
			cont_flag = 1;	

		if(cont_flag == 1)
			sprintf(new->cpus,"%d-%d",pg_ks_temp1->cpus[0],pg_ks_temp1->cpus[pg_ks_temp1->ncpus -1]);
		else
		{
			sprintf(cpu,"%d",pg_ks_temp1->cpus[0]);
			strcpy(str_cpu,cpu);
			for(count2 = 1;count2 < pg_ks_temp1->ncpus ; count2++)
			{
				strcat(str_cpu," ");
				sprintf(cpu,"%d",pg_ks_temp1->cpus[count2]);
				strcat(str_cpu,cpu);
			}
			strcpy(new->cpus,str_cpu);

		}


		if(count1 == 0)
		{
			temp    = new;
			pg_head = new;
		}
		else
		{
			temp->next = new;
			temp	   = new;
		}
		


		pg_ks_temp1 = pg_ks_temp1->next;
		pg_ks_temp2 = pg_ks_temp2->next;
		new = (pg*)malloc(sizeof(pg));
		init_pg(new);
		count1++;
		cont_flag = 0;
	}

	
	if(new != NULL)
		free(new);

	if(str_cpu != NULL)
		free(str_cpu);

	if(cpu != NULL)
		free(cpu);

	return pg_head;

}



// This function will create the "system" PG in pg linklist as well as in pg_ks for future purpose
void create_root_pg(pgstat_options* pgopt)
{
	pg_ks*		pg_ks_root1;
	pg_ks*		pg_ks_root2;
	pg* 		pg_temp;
	pg* 		root;
	cpu_data* 	cpu_temp1;
	cpu_data*	cpu_temp2;

	int		min_cpu = 0;
	int		max_cpu = 0;
	long long int	usr	= 0;
	long long int	sys	= 0;
	long long int	idle	= 0;
	long long int	total   = 0;
	int		count	= 0;

	pg_temp 	= pgopt->pg_head;
	cpu_temp1 	= pgopt->cpu_head1;
	cpu_temp2	= pgopt->cpu_head2;

	root = (pg*)malloc(sizeof(pg));
	pg_ks_root1 = (pg_ks*)malloc(sizeof(pg_ks));
	pg_ks_root2 = (pg_ks*)malloc(sizeof(pg_ks));
	init_pg(root);



	root->pgid = 0;
	root->parent_pg_id = -1;
	root->next = pgopt->pg_head;
	pgopt->pg_head = root;

	strcpy(root->relationship,"System");
	min_cpu = cpu_temp1->cpu_id;
	max_cpu = cpu_temp1->cpu_id;
	cpu_temp1 = cpu_temp1->next;

	// Finding Max and Min cpu_id as well as store them in root pg of pg_ks linklist
	while(cpu_temp1 != NULL)	
	{
		if(cpu_temp1->cpu_id < min_cpu)
			min_cpu = cpu_temp1->cpu_id;

		if(cpu_temp1->cpu_id > max_cpu)
			max_cpu = cpu_temp1->cpu_id;
		
		pg_ks_root1->cpus[count] = cpu_temp1->cpu_id;
		pg_ks_root2->cpus[count] = cpu_temp1->cpu_id;

		cpu_temp1 = cpu_temp1->next;
		count++;
	}

	// Storing data in pg_ks root structure
	pg_ks_root1->ncpus = count;
	pg_ks_root2->ncpus = count;
	pg_ks_root1->pg_id = 0;
	pg_ks_root2->pg_id = 0;
	pg_ks_root1->hw_util = 0;
	pg_ks_root2->hw_util = 0;
	pg_ks_root1->hw_util_rate = 0;
	pg_ks_root2->hw_util_rate = 0;
	pg_ks_root1->hw_util_rate_max = 0;
	pg_ks_root2->hw_util_rate_max = 0;
	pg_ks_root1->hw_util_time_running = 0;
	pg_ks_root2->hw_util_time_running = 0;
	pg_ks_root1->hw_util_time_stopped = 0;
	pg_ks_root2->hw_util_time_stopped = 0;
	pg_ks_root1->snaptime = 0;
	pg_ks_root2->snaptime = 0;
	pg_ks_root1->generation = 0;
	pg_ks_root2->generation = 0;
	pg_ks_root1->parent_pg_id = -1;
	pg_ks_root2->parent_pg_id = -1;
	strcpy(pg_ks_root1->relationship,"System");
	strcpy(pg_ks_root2->relationship,"System");
	pg_ks_root1->next  = pgopt->pg_ks_head1;
	pg_ks_root2->next  = pgopt->pg_ks_head2;
	pgopt->pg_ks_head1 = pg_ks_root1;
	pgopt->pg_ks_head2 = pg_ks_root2;
	


	//  cpus
	sprintf(root->cpus,"%d-%d",min_cpu,max_cpu);


	// Calculating sw_load , usr ,sys ,idle
	cpu_temp1 = pgopt->cpu_head1;
	cpu_temp2 = pgopt->cpu_head2;
	while(cpu_temp1 != NULL && cpu_temp2 != NULL)
	{
		usr = usr + (cpu_temp2->cpu_ticks_user - cpu_temp1->cpu_ticks_user);
		sys = sys + (cpu_temp2->cpu_ticks_kernel - cpu_temp1->cpu_ticks_kernel);
		idle = idle + (cpu_temp2->cpu_ticks_idle - cpu_temp1->cpu_ticks_idle);

		cpu_temp1 = cpu_temp1->next;
		cpu_temp2 = cpu_temp2->next;
	}
	
	total = usr + sys + idle;

	root->usr = ((float)usr/(float)total)*100;
	root->sys = ((float)sys/(float)total)*100;
	root->idle = ((float)idle/(float)total)*100;
	root->sw_load = (((float)sys + (float)usr)/(float)total)*100;
	

	// Calculating hw_load , capacity , utilization ( Yet to be done)

}






// This functions will get USR,SYS,IDLE,SWload from the cpu_data
void get_SW_usr_sys_idle(cpu_data* head1,cpu_data* head2,pg_ks* pg_ks_head1,pg_ks* pg_ks_head2,pg* pg_head)
{

	int	count1 	= 0;
	int	count2 	= 0;
	long long int	usr	= 0;
	long long int	sys	= 0;
	long long int	idle	= 0;
	long long int	total   = 0;

	cpu_data* cpu_temp1;
	cpu_data* cpu_temp2;
	pg_ks*    pg_ks_temp1;
	pg_ks*	  pg_ks_temp2;
	pg*	  pg_temp;


	cpu_temp1 	= head1;
	cpu_temp2	= head2;
	pg_temp  	= pg_head;
	pg_ks_temp1	= pg_ks_head1;
	pg_ks_temp2	= pg_ks_head2;


	while(pg_temp != NULL)
	{

		for(count1 = 0; count1 < pg_ks_temp1->ncpus ; count1++)
		{
			while((cpu_temp1 != NULL || cpu_temp2 != NULL) && (pg_ks_temp1->cpus[count1] != cpu_temp1->cpu_id) )
			{
				cpu_temp1 = cpu_temp1->next;
				cpu_temp2 = cpu_temp2->next;				
			}
			if(cpu_temp1 == NULL || cpu_temp2 == NULL)
			{
				
				cpu_temp1 = head1;
				cpu_temp2 = head2;
				continue;
			}
			else
			{

				sys = sys + (cpu_temp2->cpu_ticks_kernel - cpu_temp1->cpu_ticks_kernel);
				usr = usr + (cpu_temp2->cpu_ticks_user - cpu_temp1->cpu_ticks_user);
				idle = idle + (cpu_temp2->cpu_ticks_idle - cpu_temp1->cpu_ticks_idle);
				cpu_temp1 = head1;
				cpu_temp2 = head2;
			}

		} 
				

		total  = sys + usr + idle;
		if(total == 0)
			total = 1;

		pg_temp->sw_load	= (((float)sys + (float)usr)*100)/(float)total;
		pg_temp->usr		= ((float)usr*100)/(float)total;
		pg_temp->sys		= ((float)sys*100)/(float)total;
		pg_temp->idle		= ((float)idle*100)/(float)total;

		
		pg_temp = pg_temp->next;
		pg_ks_temp1 = pg_ks_temp1->next;
		pg_ks_temp2 = pg_ks_temp2->next;
		usr = sys = idle = 0;
		total = 0;
		
	}
	

}




// This function will calculate HW_utilization , Capacity and Utilization for a particular pg
void get_HW_CAP_UTIL(cpu_data* head,pg_ks* pg_ks_head1,pg_ks* pg_ks_head2,pg* pg_head)
{
	 






}



// This function will only keep those PG in the pg* linklist which matches either the PG id in P argument
// or the PG has the CPU which matches the c options
void c_P_option_function(pgstat_options* pgopt)
{
	int	count 	= 0;
	int	count1 	= 0;
	int	count2	= 0;
	int	p_flag	= 0;
	int	c_flag	= 0;
	int	flag	= 0;

	pg* 	pg_temp;
	pg* 	temp;
	pg_ks*	pg_ks_temp1;
	pg_ks*	pg_ks_temp2;
	pg_ks*  pg_temp1;
	pg_ks*  pg_temp2;


	pg_temp  	= pgopt->pg_head;
	temp		= pg_temp;
	pg_ks_temp1	= pgopt->pg_ks_head1;
	pg_ks_temp2	= pgopt->pg_ks_head2;
	pg_temp1	= pg_ks_temp1;
	pg_temp2	= pg_ks_temp2;

	
	while(pg_temp != NULL)
	{
		if(search_num(pg_temp->pgid , pgopt->P_argument) == 1)
			p_flag = 1;

		for(count1 =0 ;count1 < pg_ks_temp1->ncpus ; count1++)
			if(search_num(pg_ks_temp1->cpus[count1] , pgopt->c_argument) == 1)
				c_flag = 1;

		
		if(c_flag == 0 && p_flag == 0)
		{
			if(count == 0)
			{
				pgopt->pg_head = pg_temp->next;
				pgopt->pg_ks_head1 = pg_ks_temp1->next;
				pgopt->pg_ks_head2 = pg_ks_temp2->next;
				temp = pg_temp->next;
				count = -1;
			}
			else
			{ 
				temp->next = pg_temp->next;
				pg_temp1->next = pg_ks_temp1->next;
				pg_temp2->next = pg_ks_temp2->next;
				flag =1;
			}
		}
		
		if(count > 0 && flag == 0)
		{
			temp = temp->next;
			pg_temp1 = pg_temp1->next;
			pg_temp2 = pg_temp2->next;
		}

		pg_temp 	= pg_temp->next;
		pg_ks_temp1 	= pg_ks_temp1->next;
		pg_ks_temp2	= pg_ks_temp2->next;
		p_flag 		= 0;
		c_flag 		= 0;
		flag		= 0;
		count++;
		
	}



}


// This function will keep  the PGs whos Relationship does not match the argument of R option
void R_option_function(pgstat_options* pgopt)
{
	int	count 	= 0;
	int	count1 	= 0;
	int	count2	= 0;
	int	R_flag	= 0;
	int	flag	= 0;

	pg* 	pg_temp;
	pg* 	temp;
	pg_ks*	pg_ks_temp1;
	pg_ks*	pg_ks_temp2;
	pg_ks*  pg_temp1;
	pg_ks*  pg_temp2;


	pg_temp  	= pgopt->pg_head;
	temp		= pg_temp;
	pg_ks_temp1	= pgopt->pg_ks_head1;
	pg_ks_temp2	= pgopt->pg_ks_head2;
	pg_temp1	= pg_ks_temp1;
	pg_temp2	= pg_ks_temp2;


	while(pg_temp != NULL)
	{
		if(search_rel(pg_temp->relationship , pgopt->R_argument) == 1)
			R_flag = 1;

		
		if(R_flag == 1)
		{
			if(count == 0)
			{
				pgopt->pg_head = pg_temp->next;
				pgopt->pg_ks_head1 = pg_ks_temp1->next;
				pgopt->pg_ks_head2 = pg_ks_temp2->next;
				temp = pg_temp->next;
				count = -1;
			}
			else
			{ 
				temp->next = pg_temp->next;
				pg_temp1->next = pg_ks_temp1->next;
				pg_temp2->next = pg_ks_temp2->next;
				flag =1;
			}
		}
		
		if(count > 0 && flag == 0)
		{
			temp = temp->next;
			pg_temp1 = pg_temp1->next;
			pg_temp2 = pg_temp2->next;
		}
		pg_temp 	= pg_temp->next;
		pg_ks_temp1 	= pg_ks_temp1->next;
		pg_ks_temp2	= pg_ks_temp2->next;
		R_flag 		= 0;
		flag		= 0;
		count++;
	}


	  




}







// This function will keep  the PGs whos Relationship does not match the argument of r option
void r_option_function(pgstat_options* pgopt)
{
	int	count 	= 0;
	int	count1 	= 0;
	int	count2	= 0;
	int	r_flag	= 0;
	int	flag	= 0;

	pg* 	pg_temp;
	pg* 	temp;
	pg_ks*	pg_ks_temp1;
	pg_ks*	pg_ks_temp2;
	pg_ks*  pg_temp1;
	pg_ks*  pg_temp2;


	pg_temp  	= pgopt->pg_head;
	temp		= pg_temp;
	pg_ks_temp1	= pgopt->pg_ks_head1;
	pg_ks_temp2	= pgopt->pg_ks_head2;
	pg_temp1	= pg_ks_temp1;
	pg_temp2	= pg_ks_temp2;


	while(pg_temp != NULL)
	{
		if(search_rel(pg_temp->relationship , pgopt->r_argument) == 1)
			r_flag = 1;

		
		if(r_flag == 0 )
		{
			if(count == 0)
			{
				pgopt->pg_head = pg_temp->next;
				pgopt->pg_ks_head1 = pg_ks_temp1->next;
				pgopt->pg_ks_head2 = pg_ks_temp2->next;
				temp = pg_temp->next;
				count = -1;
			}
			else
			{ 
				temp->next = pg_temp->next;
				pg_temp1->next = pg_ks_temp1->next;
				pg_temp2->next = pg_ks_temp2->next;
				flag =1;
			}
		}
		
		if(count > 0 && flag == 0)
		{
			temp = temp->next;
			pg_temp1 = pg_temp1->next;
			pg_temp2 = pg_temp2->next;
		}
		pg_temp 	= pg_temp->next;
		pg_ks_temp1 	= pg_ks_temp1->next;
		pg_ks_temp2	= pg_ks_temp2->next;
		r_flag 		= 0;
		flag		= 0;
		count++;
	}

}







// This functions will sort the PG list in the ascending order according to the key provided by the user as argument
void S_option_function(pgstat_options* pgopt)
{
	int	i_min;
	int	f_min;
	int	count	= 0;

	pg*	start;
	pg*	pg_temp;
	pg*	temp;
	pg*	min;
	pg*	min_temp;


	pg_temp	= pgopt->pg_head->next;
	temp	= pgopt->pg_head;
	start	= pgopt->pg_head;

	while(start->next != NULL)
	{
		if(count ==0 )
		{
			min	= start;
			min_temp = start;
		}
		else
		{
			min 	= start->next;
			min_temp = start;
		}
		
		temp	= min;
		pg_temp = min->next;
		
		while(pg_temp != NULL)
		{
			switch(pgopt->S_argument)
			{
					case 1:
						if(pg_temp->pgid  < min->pgid)
						{
							min 		= pg_temp;
							min_temp 	= temp;
						}
						break;
					case 2:
						if(pg_temp->hw_load  < min->hw_load)
						{
							min 		= pg_temp;
							min_temp 	= temp;
						}
						break;
					case 3:
						if(pg_temp->sw_load  < min->sw_load)
						{
							min 		= pg_temp;
							min_temp 	= temp;
						}
						break;
					case 4:
						if(pg_temp->usr  < min->usr)
						{
							min 		= pg_temp;
							min_temp 	= temp;
						}
						break;
					case 5:
						if(pg_temp->sys  < min->sys)
						{
							min 		= pg_temp;
							min_temp 	= temp;
						}
						break;
					case 6:
						if(pg_temp->idle  < min->idle)
						{
							min 		= pg_temp;
							min_temp 	= temp;
						}
						break;
			}
			
	
			pg_temp = pg_temp->next;
			temp	= temp->next;
		}	 


		if(count == 0)
		{
			if(min == pgopt->pg_head)
			{
				temp = min;
				pg_temp = min->next;
				min = min->next;
			}
			else
			{
				min_temp->next 	= min->next;
				min->next	= pgopt->pg_head;
				pgopt->pg_head  = min;
				start		= min;
			}
		}
		else
		{	
			min_temp->next = min->next;	
			min->next = start->next;
			start->next = min;
			start	  = min;
			
		}
		count++;
		
	}


}






// This functions will sort the PG list in the descending order according to the key provided by the user as argument
void s_option_function(pgstat_options* pgopt)
{
	int	count	= 0;

	pg*	start;
	pg*	pg_temp;
	pg*	temp;
	pg*	max;
	pg*	max_temp;


	pg_temp	= pgopt->pg_head->next;
	temp	= pgopt->pg_head;
	start	= pgopt->pg_head;

	while(start->next != NULL)
	{
		if(count ==0 )
		{
			max	= start;
			max_temp = start;
		}
		else
		{
			max 	= start->next;
			max_temp = start;
		}
		
		temp	= max;
		pg_temp = max->next;
		
		while(pg_temp != NULL)
		{
			switch(pgopt->s_argument)
			{
					case 1:
						if(pg_temp->pgid  > max->pgid)
						{
							max 		= pg_temp;
							max_temp 	= temp;
						}
						break;
					case 2:
						if(pg_temp->hw_load  > max->hw_load)
						{
							max 		= pg_temp;
							max_temp 	= temp;
						}
						break;
					case 3:
						if(pg_temp->sw_load  > max->sw_load)
						{
							max 		= pg_temp;
							max_temp 	= temp;
						}
						break;
					case 4:
						if(pg_temp->usr > max->usr)
						{
							max 		= pg_temp;
							max_temp 	= temp;
						}
						break;
					case 5:
						if(pg_temp->sys  > max->sys)
						{
							max 		= pg_temp;
							max_temp 	= temp;
						}
						break;
					case 6:
						if(pg_temp->idle  < max->idle)
						{
							max 		= pg_temp;
							max_temp 	= temp;
						}
						break;
			}
			
	
			pg_temp = pg_temp->next;
			temp	= temp->next;
		}	 


		if(count == 0)
		{
			if(max == pgopt->pg_head)
			{
				temp = max;
				pg_temp = max->next;
				max = max->next;
			}
			else
			{
				max_temp->next 	= max->next;
				max->next	= pgopt->pg_head;
				pgopt->pg_head  = max;
				start		= max;
			}
		}
		else
		{	
			max_temp->next = max->next;	
			max->next = start->next;
			start->next = max;
			start	  = max;
			
		}
		count++;
		
	}


}






// This function will only keep top N PGs 
void t_option_function(pgstat_options* pgopt)
{
	int count = 0;
	pg*	pg_temp;

	pg_temp = pgopt->pg_head;

	for(count= 0;count < (pgopt->t_argument-1) && pg_temp != NULL ; count++)
		pg_temp = pg_temp->next;

	if(pg_temp != NULL)
		pg_temp->next = NULL;

}





// This functions will generate all the data which is to be printed 
void create_pg_data(pgstat_options* pgopt)
{

	pg* pg_temp;

	pg_temp = pgopt->pg_head;
	
	// Store pgid , relationship and cpus
	pgopt->pg_head = get_pg_rel_cpus(pgopt->pg_ks_head1 , pgopt->pg_ks_head2 , pgopt->pg_head);
	
	// calculate swload , user , sys ,idle time for each pg
	get_SW_usr_sys_idle(pgopt->cpu_head1,pgopt->cpu_head2,pgopt->pg_ks_head1,pgopt->pg_ks_head2,pgopt->pg_head);

	// calculating hwload , util , capacity for each pg
	get_HW_CAP_UTIL(pgopt->cpu_head1,pgopt->pg_ks_head1,pgopt->pg_ks_head2,pgopt->pg_head);	
	
	pg_temp = pgopt->pg_head;

	

}





// This function will display the PG in printable format
void simple_display(pgstat_options* pgopt)
{
	int	pgid_max_len 	= 0;
	int	rel_max_len	= 0;
	int	max_pgid	= 0;
	int	pgid_len	= 0;
	int	rel_len		= 0;
	int	sw_len		= 0;
	int	hw_len		= 0;
	int	count1		= 0;
	int	count2		= 0;
	int	count3		= 0;

	char	pgid_char[10];
	


	pg*	pg_temp;

	pg_temp = pgopt->pg_head;


	while(pg_temp != NULL)
	{
		if(pg_temp->pgid > max_pgid)
			max_pgid = pg_temp->pgid;

		if(strlen(pg_temp->relationship)  > rel_max_len)
			rel_max_len = strlen(pg_temp->relationship);

		pg_temp = pg_temp->next;
	}

	sprintf(pgid_char,"%d",max_pgid);
	pgid_max_len = strlen(pgid_char);

	printf("PG");
	if(pgid_max_len >= 3 )
	{
		for(count1 = 0; count1 < (pgid_max_len) ; count1++)
			printf(" ");
	}
	else
	{
		printf("  ");
		pgid_max_len = 2;
	}

	printf("Relationship");
	if(rel_max_len >= 13)
	{
		for(count1 = 0; count1 <= (rel_max_len - 13 + 2); count1++)
			printf(" ");
	}
	else
	{
		printf("  ");
		rel_max_len = 12;
	}

	printf(" HW       SW       CPUS\n");



	pg_temp = pgopt->pg_head;
	while(pg_temp != NULL)
	{
		printf("%d",pg_temp->pgid);
		sprintf(pgid_char,"%d",pg_temp->pgid);
		pgid_len = strlen(pgid_char);
		for(count1 = 0; count1 <= (pgid_max_len - pgid_len + 2);count1++)
			printf(" ");

		printf("%s",pg_temp->relationship);
		rel_len = strlen(pg_temp->relationship);
		for(count1 = 0; count1 <= (rel_max_len - rel_len + 2);count1++)
			printf(" ");

		printf("%.1f%%",pg_temp->hw_load);		
		sprintf(pgid_char,"%.1f%%",pg_temp->hw_load);
		hw_len = strlen(pgid_char);
		for(count1 = 0; count1 <= (8 - hw_len) ; count1++)
			printf(" ");

		printf("%.1f%%",pg_temp->sw_load);		
		sprintf(pgid_char,"%.1f%%",pg_temp->sw_load);
		sw_len = strlen(pgid_char);
		for(count1 = 0; count1 <= (8 - sw_len) ; count1++)
			printf(" ");		

		printf("%s",pg_temp->cpus);

		printf("\n");
		pg_temp = pg_temp->next;
	}




}









// This function will display the PG in printable format and more detailed
void v_option_function(pgstat_options* pgopt)
{
	int	pgid_max_len 	= 0;
	int	rel_max_len	= 0;
	int	max_pgid	= 0;
	
	int	len		= 0;
	
	int	count1		= 0;
	int	count2		= 0;
	int	count3		= 0;

	char	pgid_char[10];
	


	pg*	pg_temp;

	pg_temp = pgopt->pg_head;


	while(pg_temp != NULL)
	{
		if(pg_temp->pgid > max_pgid)
			max_pgid = pg_temp->pgid;

		if(strlen(pg_temp->relationship)  > rel_max_len)
			rel_max_len = strlen(pg_temp->relationship);

		pg_temp = pg_temp->next;
	}

	sprintf(pgid_char,"%d",max_pgid);
	pgid_max_len = strlen(pgid_char);

	printf("PG");
	if(pgid_max_len >= 3 )
	{
		for(count1 = 0; count1 < (pgid_max_len) ; count1++)
			printf(" ");
	}
	else
	{
		printf("  ");
		pgid_max_len = 2;
	}

	printf("Relationship");
	if(rel_max_len >= 13)
	{
		for(count1 = 0; count1 <= (rel_max_len - 13 + 2); count1++)
			printf(" ");
	}
	else
	{
		printf("  ");
		rel_max_len = 12;
	}

	printf(" HW      UTIL    CAP     SW      USR     SYS     IDLE    CPUS\n");



	pg_temp = pgopt->pg_head;
	while(pg_temp != NULL)
	{
		printf("%d",pg_temp->pgid);
		sprintf(pgid_char,"%d",pg_temp->pgid);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (pgid_max_len - len + 2);count1++)
			printf(" ");

		printf("%s",pg_temp->relationship);
		len = strlen(pg_temp->relationship);
		for(count1 = 0; count1 <= (rel_max_len - len + 2);count1++)
			printf(" ");

		printf("%.1f%%",pg_temp->hw_load);		
		sprintf(pgid_char,"%.1f%%",pg_temp->hw_load);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");

		printf("%.1f%%",pg_temp->hw_util);		
		sprintf(pgid_char,"%.1f%%",pg_temp->hw_util);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");

		printf("%.1f%%",pg_temp->capacity);		
		sprintf(pgid_char,"%.1f ",pg_temp->capacity);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");


		printf("%.1f%%",pg_temp->sw_load);		
		sprintf(pgid_char,"%.1f%%",pg_temp->sw_load);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");

		printf("%.1f%%",pg_temp->usr);		
		sprintf(pgid_char,"%.1f%%",pg_temp->usr);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		

		printf("%.1f%%",pg_temp->sys);		
		sprintf(pgid_char,"%.1f%%",pg_temp->sys);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");
	
		printf("%.1f%%",pg_temp->idle);		
		sprintf(pgid_char,"%.1f%%",pg_temp->idle);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");

		printf("%s",pg_temp->cpus);

		printf("\n");
		pg_temp = pg_temp->next;
	}




}



// This function will calculate min,max and avg hw_load ,sw_load over a period of time 
pg_total* A_option_function(pgstat_options* pgopt , int count)
{
	pg_total*	new;
	pg_total*	temp;
	pg*		pg_temp;

	int		i	= 0;


	if(pgopt->pg_total_head == NULL)
	{
		
		new	= (pg_total*)malloc(sizeof(pg_total));
		new->next = NULL;
		pg_temp	= pgopt->pg_head;

		while(pg_temp != NULL)
		{
			new->pgid = pg_temp->pgid;

			new->hw_load_min = pg_temp->hw_load;
			new->hw_load_max = pg_temp->hw_load;
			new->hw_load_avg = pg_temp->hw_load;

			new->sw_load_min = pg_temp->sw_load;
			new->sw_load_max = pg_temp->sw_load;
			new->sw_load_avg = pg_temp->sw_load;

			if(i == 0)
			{
				pgopt->pg_total_head 	= new;
				temp 			= new;
			}
			else
			{
				temp->next 	= new;
				temp 		= new;
			}

			new = (pg_total*)malloc(sizeof(pg_total));
			new->next = NULL;
			pg_temp = pg_temp->next;
			i++;
		}
	
	}
	else
	{
		temp	 	= pgopt->pg_total_head;
		pg_temp		= pgopt->pg_head;

		while(pg_temp != NULL)
		{
			// Doing the calculation for HW_load
			if(pg_temp->hw_load > temp->hw_load_max)
				temp->hw_load_max = pg_temp->hw_load;

			if(pg_temp->hw_load < temp->hw_load_min)
				temp->hw_load_min = pg_temp->hw_load;

			temp->hw_load_avg = ((temp->hw_load_avg * (count-1)) + pg_temp->hw_load ) / (float)count;


			// Doing calculation for SW_load
			if(pg_temp->sw_load > temp->sw_load_max)
				temp->sw_load_max = pg_temp->sw_load;

			if(pg_temp->sw_load < temp->sw_load_min)
				temp->sw_load_min = pg_temp->sw_load;

			temp->sw_load_avg = ((temp->sw_load_avg * (count-1)) + pg_temp->sw_load ) / (float)count;



			pg_temp = pg_temp->next;
			temp 	= temp->next;
		}

	}
	
	return pgopt->pg_total_head;


}


// This function will display the output of pg_total structure
void A_option_display(pgstat_options* pgopt)
{
	int	pgid_max_len 	= 0;
	int	rel_max_len	= 0;
	int	max_pgid	= 0;
	int	len		= 0;
	int	count1		= 0;

	char	pgid_char[10];
	


	pg*		pg_temp;
	pg_total* 	pg_total_temp;

	pg_temp 	= pgopt->pg_head;
	pg_total_temp 	= pgopt->pg_total_head;

	printf("\n \t\t SUMMARY: UTILIZATION OVER %d SECONDS\n",(pgopt->count * pgopt->interval));

	while(pg_temp != NULL)
	{
		if(pg_temp->pgid > max_pgid)
			max_pgid = pg_temp->pgid;

		if(strlen(pg_temp->relationship)  > rel_max_len)
			rel_max_len = strlen(pg_temp->relationship);

		pg_temp = pg_temp->next;
	}

	sprintf(pgid_char,"%d",max_pgid);
	pgid_max_len = strlen(pgid_char);


	// For printing headers  " -- hardware-- " and "-- software -- "
	if(pgid_max_len > 2)
		len = pgid_max_len;
	else
		len = 4;
	for(count1 = 0 ;count1 <= len;count1++)
		printf(" ");
	
	if(rel_max_len > 12) 
		len = rel_max_len;
	else
		len = 14;
	for(count1 = 0;count1 <= len ; count1++)
		printf(" ");
	printf(" ------HARDWARE------    ------SOFTWARE------\n");



	printf("PG");
	if(pgid_max_len >= 3 )
	{
		for(count1 = 0; count1 < (pgid_max_len) ; count1++)
			printf(" ");
	}
	else
	{
		printf("  ");
		pgid_max_len = 2;
	}

	printf("Relationship");
	if(rel_max_len >= 13)
	{
		for(count1 = 0; count1 <= (rel_max_len - 13 + 2); count1++)
			printf(" ");
	}
	else
	{
		printf("  ");
		rel_max_len = 12;
	}

	printf(" MIN     AVG     MAX     MIN     AVG     MAX    CPUS\n");



	pg_temp = pgopt->pg_head;
	while(pg_temp != NULL)
	{
		printf("%d",pg_temp->pgid);
		sprintf(pgid_char,"%d",pg_temp->pgid);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (pgid_max_len - len + 2);count1++)
			printf(" ");

		printf("%s",pg_temp->relationship);
		len = strlen(pg_temp->relationship);
		for(count1 = 0; count1 <= (rel_max_len - len + 2);count1++)
			printf(" ");

		printf("%.1f%%",pg_total_temp->hw_load_min);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->hw_load_min);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");

		printf("%.1f%%",pg_total_temp->hw_load_avg);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->hw_load_avg);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		
		
		printf("%.1f%%",pg_total_temp->hw_load_max);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->hw_load_max);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");

		printf("%.1f%%",pg_total_temp->sw_load_min);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->sw_load_min);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		

		printf("%.1f%%",pg_total_temp->sw_load_avg);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->sw_load_avg);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		

		printf("%.1f%%",pg_total_temp->sw_load_max);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->sw_load_max);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		

		printf("%s",pg_temp->cpus);

		printf("\n");
		pg_temp 	= pg_temp->next;
		pg_total_temp 	= pg_total_temp->next;
	}




}







// This function will display the output of pg_total structure with v options used
void Av_option_display(pgstat_options* pgopt)
{
	int	pgid_max_len 	= 0;
	int	rel_max_len	= 0;
	int	max_pgid	= 0;
	int	len		= 0;
	int	count1		= 0;

	char	pgid_char[10];
	


	pg*		pg_temp;
	pg_total* 	pg_total_temp;

	pg_temp 	= pgopt->pg_head;
	pg_total_temp 	= pgopt->pg_total_head;

	printf("\n \t\t SUMMARY: UTILIZATION OVER %d SECONDS\n",(pgopt->count * pgopt->interval));

	while(pg_temp != NULL)
	{
		if(pg_temp->pgid > max_pgid)
			max_pgid = pg_temp->pgid;

		if(strlen(pg_temp->relationship)  > rel_max_len)
			rel_max_len = strlen(pg_temp->relationship);

		pg_temp = pg_temp->next;
	}

	sprintf(pgid_char,"%d",max_pgid);
	pgid_max_len = strlen(pgid_char);


	// For printing headers  " -- hardware-- " and "-- software -- "
	if(pgid_max_len > 2)
		len = pgid_max_len;
	else
		len = 4;
	for(count1 = 0 ;count1 <= len;count1++)
		printf(" ");
	
	if(rel_max_len > 12) 
		len = rel_max_len;
	else
		len = 14;
	for(count1 = 0;count1 <= len+14 ; count1++)
		printf(" ");
	printf(" ------HARDWARE------    ------SOFTWARE------\n");



	printf("PG");
	if(pgid_max_len >= 3 )
	{
		for(count1 = 0; count1 < (pgid_max_len) ; count1++)
			printf(" ");
	}
	else
	{
		printf("  ");
		pgid_max_len = 2;
	}

	printf("Relationship");
	if(rel_max_len >= 13)
	{
		for(count1 = 0; count1 <= (rel_max_len - 13 + 2); count1++)
			printf(" ");
	}
	else
	{
		printf("  ");
		rel_max_len = 12;
	}

	printf(" UTIL   CAP    MIN     AVG     MAX     MIN     AVG     MAX    CPUS\n");



	pg_temp = pgopt->pg_head;
	while(pg_temp != NULL)
	{
		printf("%d",pg_temp->pgid);
		sprintf(pgid_char,"%d",pg_temp->pgid);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (pgid_max_len - len + 1);count1++)
			printf(" ");

		printf("%s",pg_temp->relationship);
		len = strlen(pg_temp->relationship);
		for(count1 = 0; count1 <= (rel_max_len - len + 1);count1++)
			printf(" ");

		printf("%.1f%%",pg_temp->hw_util);
		sprintf(pgid_char,"%.1f%%",pg_temp->hw_util);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len);count1++)
			printf(" ");

		printf("%.1f%%",pg_temp->capacity);
		sprintf(pgid_char,"%.1f%%",pg_temp->capacity);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len);count1++)
			printf(" ");


		printf("%.1f%%",pg_total_temp->hw_load_min);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->hw_load_min);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");

		printf("%.1f%%",pg_total_temp->hw_load_avg);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->hw_load_avg);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		
		
		printf("%.1f%%",pg_total_temp->hw_load_max);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->hw_load_max);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");

		printf("%.1f%%",pg_total_temp->sw_load_min);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->sw_load_min);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		

		printf("%.1f%%",pg_total_temp->sw_load_avg);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->sw_load_avg);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		

		printf("%.1f%%",pg_total_temp->sw_load_max);		
		sprintf(pgid_char,"%.1f%%",pg_total_temp->sw_load_max);
		len = strlen(pgid_char);
		for(count1 = 0; count1 <= (7 - len) ; count1++)
			printf(" ");		

		printf("%s",pg_temp->cpus);

		printf("\n");
		pg_temp 	= pg_temp->next;
		pg_total_temp 	= pg_total_temp->next;
	}




}



void pgstat_fn(pgstat_options* pgopt)
{

	cpu_data* temp1;
	cpu_data* temp2;
	pg*	  pg_temp;
	time_t	  t;

	int	  count	 = 1;
	
	


	if(pgopt->interval == 0)
		pgopt->count = 1;
	else
	{
		if(pgopt->count == -2)
			pgopt->count = -1;
	}


	while(pgopt->count != 0)
	{
	
		
		if(pgopt->T_argument  == 'u')
			printf("%d\n",time(0));
		else if(pgopt->T_argument == 'd')
		{
			t = time(0);
			printf("%s\n",ctime(&t));
		}
		
		pgopt->pg_ks_head1 = pg_create_from_kstat(pgopt->pg_ks_head1);
		pgopt->cpu_head1	= cpu_data_create(pgopt->cpu_head1);

		sleep(2);

		pgopt->pg_ks_head2 = pg_create_from_kstat(pgopt->pg_ks_head2);
		pgopt->cpu_head2	= cpu_data_create(pgopt->cpu_head2);	
		


		create_pg_data(pgopt);
		create_root_pg(pgopt);
		

		// Applying -c and -P options together
		if(pgopt->c_argument[0] != -1 || pgopt->P_argument[0] != -1)
			c_P_option_function(pgopt);

		// Applying -R 
		if(strcmp(pgopt->R_argument, "\0") != 0 )
			R_option_function(pgopt);

		// Applying -r
		if(strcmp(pgopt->r_argument,"\0") != 0)
			r_option_function(pgopt);

		// Applying -S
		if(pgopt->S_argument > 0)
			S_option_function(pgopt);

		// Applying -s
		if(pgopt->s_argument > 0)
			s_option_function(pgopt);

		// Applying -t
		if(pgopt->t_argument > -1)
			t_option_function(pgopt);			

		// Applying -v
		if(pgopt->v_option_status == 1)
			v_option_function(pgopt);
		else if(pgopt->v_option_status == 0)
			simple_display(pgopt);

		// Applying -A
		if(pgopt->A_option_status == 1)
			pgopt->pg_total_head = A_option_function(pgopt,count);


		pgopt->count--;
		count++;
		sleep(pgopt->interval);
		pgopt->pg_ks_head1 = NULL;
		pgopt->pg_ks_head2 = NULL;
		pgopt->cpu_head1   = NULL;
		pgopt->cpu_head2   = NULL;
	}

	

	if(pgopt->A_option_status == 1)
	{
		if(pgopt->v_option_status == 0)
			A_option_display(pgopt);
		else
			Av_option_display(pgopt);
	}



}
