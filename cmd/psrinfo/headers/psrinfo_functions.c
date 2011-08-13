#include "psrinfo_functions.h"




//
// Sorting function for the qsort function
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



/*
// This function carries out the psrinfo command when no option is provided with the command
// It checks first whether to use core_id or instance for the comparision and sets the inst_core_flag
// After that if no argument is provided we fill the psropt->arguments with all the instances or core_id according to the inst_core_flag
// Then sorting the array so that the numbers appear in sequence rather than random
// After that we compare each element from the arguments array with the respective core_id or instance no
// If match found then we print their (core_id/instance) , state and state_begin
// If not found we print the error message about invalid argument
*/
void psrinfo_no_opt_fn(psrinfo_options *psropt)
{
	kstat_ctl_t   *kc;			// Pointer to the kstat data
 	kstat_t       *temp_kc;			// Pointer to reset the kc to start state
	kstat_t	      *ksp;			// Pointer to hold kstat of a particular type
	kstat_named_t *knp;			// Pointer to hold named kstat data

	
	time_t  since;				// Stores the time when the processor started
	int 	inst_core_id;
	char    since_format[50];		// Stores the time in the string format
	void    *data;				// Stores that data of the particular name of the kstat returned
	char    *name;				// Type of kstat to be accessed
	int     chk_flag = 0;			// If argument present then 1 else 0
	int     flag  = 0;			// If match found for the given instance 
	int     count = 0;				// Variable for  FOR loop
	int     err_flag = 0;
	char    *err_arg;
	int     ncore;				// Store ncore_per_chip value
	int     ncpu;				// Store ncpu_per_chip  value
	int     inst_core_flag = 1;		// 0 --- if matching to instance no   1 --- if matching to core_id
	size_t  nelems;				// For qsort function
	int 	match_flag = 0;			// 1 --- if match found     0 --- if match not found
	int 	arg_count = 0;			// no of actual arguments in the array
	

	//
	// Allocate the space for the variable
	data  = (void*)malloc(sizeof(void)*100);
	name  = (char*)malloc(sizeof(char)*50);
	err_arg = (char*)malloc(sizeof(char)*10);


	// if argument is given then we set the chk_flag = 1 else it will be 0
	if(psropt->arguments[0] != -1)
		chk_flag  = 1;


	// Open the kstat for searching
	kc = kstat_open();
	error_check_kstat_ctl_t(kc);
	temp_kc  = kc->kc_chain;




	// Look the kstat chain for the kstat with module name "cpu_info" 
	ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	error_check_kstat_t(ksp);

	// Scanning through the kstat chain
	while(ksp != NULL)
	{

		// Check whether instance no or core_id is to be used
		kstat_read(kc,ksp,NULL);
	
		knp = kstat_data_lookup(ksp,"ncore_per_chip");
		error_check_kstat_named_t(knp);
		ncore = knp->value.i32;
	
		knp = kstat_data_lookup(ksp,"ncpu_per_chip");
		error_check_kstat_named_t(knp);
		ncpu = knp->value.i32;
	
		if(ncore < ncpu)
			inst_core_flag = 0;

		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		
	}


	//
	// Repositioning the kc pointer to the head of the chain for next work
	kc->kc_chain = temp_kc;

	//
	// Fill the argument array with the instance no or core_id if user does not provide one
	if(chk_flag != 1)
	{

		//
		// Look the kstat chain for the kstat with module name "cpu_info" 
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		error_check_kstat_t(ksp);
	
	
		while(ksp != NULL)
		{
			kstat_read(kc,ksp,NULL);
		

			if(inst_core_flag == 0)
			{
				inst_core_id = ksp->ks_instance;
			}
			else
			{
				knp = kstat_data_lookup(ksp,"core_id");
				error_check_kstat_named_t(knp);
				inst_core_id = knp->value.i32;
			}
			
			psropt->arguments[count] = inst_core_id;
			kc->kc_chain = ksp->ks_next;
			ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
			count++;			
			
		}
	
	
	}
	count = 0;
	
	// counting arguments in the array
	for(arg_count = 0;psropt->arguments[arg_count] != -1;arg_count++);
	nelems = arg_count;

	
	// Sorting the incoming array of core_id or instances provided by the user
	qsort((void *)(psropt->arguments), nelems, sizeof (int), intcompare);
	
	
	
	//
	// Repositioning the kc pointer to the head of the chain for next work
	kc->kc_chain = temp_kc;
	count = 0;


	// Till all the arguments are used
	while(psropt->arguments[count] != -1)
	{


		//
		// Look the kstat chain for the kstat with module name "cpu_info" 
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		error_check_kstat_t(ksp);


		//
		// Loop till the end of the kstat chain with cpu_info as the module
		while(ksp != NULL)
		{	
			
			kstat_read(kc,ksp,NULL);
			
			if(inst_core_flag == 1)
			{
				knp = kstat_data_lookup(ksp,"core_id");
				error_check_kstat_named_t(knp);
				if(psropt->arguments[count] == knp->value.i32)
				{
					match_flag = 1;
					printf("%d\t",knp->value.i32);
					knp = kstat_data_lookup(ksp,"state");
					printf("%s\t",knp->value.c);
					knp = kstat_data_lookup(ksp,"state_begin");
					since = (time_t)knp->value.i32;
					cftime(since_format,"%D %T",&since);
					printf("since %s\n",since_format);
					psropt->arguments[count] = -1;
					break;
				}
			
			}
			else
			{
				if(psropt->arguments[count] == ksp->ks_instance)
				{
					match_flag = 1;
					printf("%d\t",ksp->ks_instance);
					knp = kstat_data_lookup(ksp,"state");
					printf("%s\t",knp->value.c);
					knp = kstat_data_lookup(ksp,"state_begin");
					since = (time_t)knp->value.i32;
					cftime(since_format,"%D %T",&since);
					printf("since %s\n",since_format);
					psropt->arguments[count] = -1;
					break;
				}
			}
	
		
			kc->kc_chain = ksp->ks_next;
			ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
			flag = 0;
			
		}	
		if(match_flag == 0)
			{}					// Do nothing
		//
		// Repositioning the kc pointer to the head of the chain for next work
		kc->kc_chain = temp_kc;	
		match_flag = 0;
		count++;
	}	
		



	kc->kc_chain = temp_kc;

	
	// close the kstat link
	kstat_close(kc);

	// If no argument matches the cpu id then error message is shown
	for(count = 0;count < 100;count++)
	{
		if(psropt->arguments[count] != -1)
		{
			err_flag = 1;
			sprintf(err_arg,"%d",psropt->arguments[count]);
			invalid_argument_error_print(err_arg);
		}
	}


	free(data);
	if(err_flag == 1)
		exit(1);

}

//						//
//	End of the psrinfo_no_opt_fn()		//
//						//











//			
//	This function will be called when the user gives v  as the option in the psrinfo command
//
void psrinfo_v_opt_fn(psrinfo_options *psropt)
{
	kstat_ctl_t   *kc;			// Pointer to the kstat data
 	kstat_t       *temp_kc;			// Pointer to reset the kc to start state
	kstat_t	      *ksp;			// Pointer to hold kstat of a particular type
	kstat_named_t *knp;			// Pointer to hold named kstat data

	
	time_t  since;				// Stores the time when the processor started
	int 	inst_core_id;
	char    since_format[50];		// Stores the time in the string format
	void    *data;				// Stores that data of the particular name of the kstat returned
	char    *name;				// Type of kstat to be accessed
	int     chk_flag = 0;			// If argument present then 1 else 0
	int     flag  = 0;			// If match found for the given instance 
	int     count = 0;			// Variable for  FOR loop
	int     err_flag = 0;
	char    *err_arg;
	int     ncore;				// Store ncore_per_chip value
	int     ncpu;				// Store ncpu_per_chip  value
	int     inst_core_flag = 1;		// 0 --- if matching to instance no   1 --- if matching to core_id
	size_t  nelems;				// For qsort function
	int 	match_flag = 0;			// 1 --- if match found     0 --- if match not found
	int 	arg_count = 0;			// no of actual arguments in the array
	int 	chip_count = 0;			// Stores no of chips
	int 	chip_id = -1;			// Stores the chi_id in it
	

	//
	// Allocate the space for the variable
	data  = (void*)malloc(sizeof(void)*100);
	name  = (char*)malloc(sizeof(char)*50);
	err_arg = (char*)malloc(sizeof(char)*10);


	if(psropt->arguments[0] != -1)
		chk_flag  = 1;


	//
	// Open the kstat for searching
	kc = kstat_open();
	error_check_kstat_ctl_t(kc);
	temp_kc  = kc->kc_chain;



	//
	// Look the kstat chain for the kstat with module name "cpu_info" 
	ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	error_check_kstat_t(ksp);
	

	while(ksp != NULL)
	{
		//
		// Check whether instance no or core_id is to be used
		kstat_read(kc,ksp,NULL);
	
		knp = kstat_data_lookup(ksp,"ncore_per_chip");
		error_check_kstat_named_t(knp);
		ncore = knp->value.i32;
	
		knp = kstat_data_lookup(ksp,"ncpu_per_chip");
		error_check_kstat_named_t(knp);
		ncpu = knp->value.i32;
	

		if(ncore < ncpu)
			inst_core_flag = 0;

		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		
	}


	//
	// Repositioning the kc pointer to the head of the chain for next work
	kc->kc_chain = temp_kc;

	//
	// Fill the argument array with the instance no or core_id if user does not provide one
	if(chk_flag != 1)
	{

		//
		// Look the kstat chain for the kstat with module name "cpu_info" 
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		error_check_kstat_t(ksp);
	
	
		while(ksp != NULL)
		{
			kstat_read(kc,ksp,NULL);
		

			if(inst_core_flag == 0)
			{
				inst_core_id = ksp->ks_instance;
			}
			else
			{
				knp = kstat_data_lookup(ksp,"core_id");
				error_check_kstat_named_t(knp);
				inst_core_id = knp->value.i32;
			}
			
			psropt->arguments[count] = inst_core_id;
			kc->kc_chain = ksp->ks_next;
			ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
			count++;			
			
		}
	
	
	}

	count = 0;
	

	for(arg_count = 0;psropt->arguments[arg_count] != -1;arg_count++);
	nelems = arg_count;

	//
	// Sorting the incoming array of core_id or instances provided by the user
	qsort((void *)(psropt->arguments), nelems, sizeof (int), intcompare);
	
	
	
	//
	// Repositioning the kc pointer to the head of the chain for next work
	kc->kc_chain = temp_kc;



	count = 0;

	while(psropt->arguments[count] != -1)
	{


		//
		// Look the kstat chain for the kstat with module name "cpu_info" 
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		error_check_kstat_t(ksp);


		//
		// Loop till the end of the kstat chain with cpu_info as the module
		while(ksp != NULL)
		{	
			
			kstat_read(kc,ksp,NULL);
			
			if(inst_core_flag == 1)
			{
				knp = kstat_data_lookup(ksp,"core_id");
				error_check_kstat_named_t(knp);
				if(psropt->arguments[count] == knp->value.i32)
				{
					match_flag = 1;
					printf("State of virtual processor %d ",knp->value.i32);
					knp = kstat_data_lookup(ksp,"state_begin");
					since = (time_t)knp->value.i32;
					cftime(since_format,"%D %T",&since);
					printf("as of : %s\n",since_format);
					knp = kstat_data_lookup(ksp,"state");
					printf("  %s since %s.\n",knp->value.c,since_format);
					
					knp = kstat_data_lookup(ksp,"cpu_type");
					printf("  The ");
					if(knp == NULL)
						printf("unknown ");
					else
						printf("%s ",knp->value.c);  
					printf("processor operates at ");

					knp = kstat_data_lookup(ksp,"clock_MHz");
					if(knp == NULL)
						printf("unknown frequency,\n");
					else
						printf("%d MHz, \n",knp->value.i32);
					
					printf("  \tand has ");
					knp = kstat_data_lookup(ksp,"fpu_type");
					if(knp == NULL)
						printf("an unknown ");
					else
						printf("%s ",knp->value.c);
					printf("floating point processor. \n\n");


					psropt->arguments[count] = -1;
					break;
				}
			
			}
			else
			{
				if(psropt->arguments[count] == ksp->ks_instance)
				{
					match_flag = 1;
					printf("State of virtual processor %d ",ksp->ks_instance);
					knp = kstat_data_lookup(ksp,"state_begin");
					since = (time_t)knp->value.i32;
					cftime(since_format,"%D %T",&since);
					printf("as of : %s\n",since_format);
					knp = kstat_data_lookup(ksp,"state");
					printf("  %s since %s.\n",knp->value.c,since_format);
					
					knp = kstat_data_lookup(ksp,"cpu_type");
					printf("  The ");
					if(knp == NULL)
						printf("unknown ");
					else
						printf("%s ",knp->value.c);  
					printf("processor operates at ");

					knp = kstat_data_lookup(ksp,"clock_MHz");
					if(knp == NULL)
						printf("unknown frequency,\n");
					else
						printf("%d MHz, \n",knp->value.i32);
					
					printf("  \tand has ");
					knp = kstat_data_lookup(ksp,"fpu_type");
					if(knp == NULL)
						printf("an unknown ");
					else
						printf("%s ",knp->value.c);
					printf("floating point processor. \n\n");
					psropt->arguments[count] = -1;
					break;
				}
			}
	
		
			kc->kc_chain = ksp->ks_next;
			ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
			flag = 0;
			
		}	
		if(match_flag == 0)
			{}					// Do nothing
		//
		// Repositioning the kc pointer to the head of the chain for next work
		kc->kc_chain = temp_kc;	
		match_flag = 0;
		count++;
	}	
		



	kc->kc_chain = temp_kc;

	
	// close the kstat link
	kstat_close(kc);

	// If no argument matches the cpu id then error message is shown
	for(count = 0;count < 100;count++)
	{
		if(psropt->arguments[count] != -1)
		{
			err_flag = 1;
			sprintf(err_arg,"%d",psropt->arguments[count]);
			invalid_argument_error_print(err_arg);
		}
	}


	free(data);
	if(err_flag == 1)
		exit(1);


}
//						//
//	End of the psrinfo_v_opt_fn()		//
//						//






//
// This functions prints the detail of all the physical processors attached to the computer
void psrinfo_vp_opt_fn(psrinfo_options* psropt)
{
	kstat_ctl_t 	*kc;
	kstat_t	    	*ksp;
	kstat_t		*temp_kc;
	kstat_named_t   *knp;

	node* head;					// Pointer to the start of the linklist
	node* n1;					// Pointer to scan through the linklist
	node* end;					// Pointer to the end of the link list
	node* begin;					// Pointer to the start position of the sorting
	node* min;					// Pointer to the minimum node in the linklist

	int count = 0;					// Variable for loop or counting
	int count1 = 0;					// Variable for loop or counting
	int count2 = 0;					// Variable for loop or counting
	int chip_ids[100];				// Stores the chip_ids in it
	int flag  = 0;					// Flag for purpose
	int ncpu;					// Stores ncpu_per_chip value in it
	int ncore;					// Stores ncore_per_chip value in it	
	int inst_core_flag = 1;				// 1 -- use core_id  , 0 - use instance    for comparision
	int chip_flag = 0;				// 0 -- chip_id present in chip_ids[]  ,  1 -- chip_id not present in chip_ids[] 

	//
	// Inititalizing chip_ids to -1
	for(count = 0;count < 100;count++)
		chip_ids[count]  = -1;

	//
	// Opening the kstat for accessing the kernal statistics
	kc = kstat_open();
	error_check_kstat_ctl_t(kc);

	temp_kc = kc->kc_chain;	

	
	//
	// Look the kstat chain for the kstat with module name "cpu_info" 
	ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	error_check_kstat_t(ksp);
	
	//
	// Check whether instance no or core_id is to be used
	while(ksp != NULL)
	{
		
		kstat_read(kc,ksp,NULL);
	
		knp = kstat_data_lookup(ksp,"ncore_per_chip");
		error_check_kstat_named_t(knp);
		ncore = knp->value.i32;
	
		knp = kstat_data_lookup(ksp,"ncpu_per_chip");
		error_check_kstat_named_t(knp);
		ncpu = knp->value.i32;
	

		if(ncore < ncpu)
			inst_core_flag = 0;

		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		
	}

	count = 0;

	//
	// Repositioning the kc pointer to the head of the chain for next work
	kc->kc_chain = temp_kc;


	//
	// Storing chip_id , core_id , instance no in an linklist
	ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	error_check_kstat_t(ksp);
	
	while(ksp != NULL)
	{
		kstat_read(kc,ksp,NULL);

		n1 = (node*)malloc(sizeof(node));
		n1->next = NULL;		

		knp = kstat_data_lookup(ksp,"chip_id");
		n1->chip_id = knp->value.i32;

		knp = kstat_data_lookup(ksp,"core_id");
		n1->core_id = knp->value.i32;

		n1->vp_id   = ksp->ks_instance;


		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		
		if(count == 0)
			head = end = n1;
		else
		{
			end->next = n1;
			end       = n1;
		}
		count++;
	}

	// 
	// Restore the position to the initial position
	kc->kc_chain = temp_kc;


	
	//
	// Sorting the linklist
	begin = head;
	min   = head;
	n1    = begin->next;
	while(begin->next != NULL)
	{
		while(n1 != NULL)
		{
			if(n1->chip_id < min->chip_id)
				min = n1;
			else if(n1->chip_id == min->chip_id)
			{
				if(n1->core_id < min->core_id)
					min = n1;
				else if(n1->core_id == min->core_id)
				{
					if(n1->vp_id < min->vp_id)
						min = n1;
				}
			}
			n1 = n1->next;
		}
	
		if(begin == min)
		{
			begin = begin->next;
			goto cont;
			n1 = begin;
		}
		n1 = begin;
		while(n1->next != min)
				n1 = n1->next;
		n1->next = n1->next->next;
		if(begin == head)
		{
			min->next = head;			
			head = min;			
		}
		else
		{
			n1 = head;
			while(n1->next != begin)			
				n1 = n1->next;
			min->next = begin;
			n1->next = min;
		}
		cont:	
			n1 = begin->next;
			min = begin;
	}

	//Restore the position to the initial position
	kc->kc_chain = temp_kc;	
	n1 = head;
	

	// comparing the arguments with the instance or core_id 
	// if match found then copy the chip_id in chip_ids[] if not there in the array
	if(psropt->arguments[0] != -1)
	{
		for(count = 0;psropt->arguments[count] != -1;count++)
		{
			while(n1 != NULL)
			{
				if(inst_core_flag == 0)
				{
					if(psropt->arguments[count] == n1->vp_id)
					{
						flag = 1;
						for(count1 = 0;chip_ids[count1] != -1;count1++)
						{			
							if(n1->chip_id == chip_ids[count1])
								chip_flag = 1;
						}
						if(chip_flag == 0)
							chip_ids[count2++] = n1->chip_id;
						chip_flag = 0;
					}
				
				}
				else if(inst_core_flag == 1)
				{
					if(psropt->arguments[count] == n1->core_id)
					{
						flag = 1;
						for(count1 = 0;chip_ids[count1] != -1;count1++)
						{			
							if(n1->chip_id == chip_ids[count1])
								chip_flag = 1;
						}
						if(chip_flag == 0)
							chip_ids[count2++] = n1->chip_id;
						chip_flag = 0;
					}
				}	
				n1 = n1->next;
			}
			if(flag == 0)
				printf("psrinfo : processor %d : Invalid argument\n",psropt->arguments[count]);
			n1 = head;
			flag = 0;
		}

	}
	else if(psropt->arguments[0] == -1)
	{
		while(n1 != NULL)
		{
			for(count1 = 0;chip_ids[count1] != -1;count1++)
			{			
				if(n1->chip_id == chip_ids[count1])
					chip_flag = 1;
			}
			if(chip_flag == 0)
				chip_ids[count2++] = n1->chip_id;
						
			n1 = n1->next;
			chip_flag = 0;
		}	

	}

	//
	// Sort the chip_ids
	qsort((void*)chip_ids,count2,sizeof(int),intcompare);
	

	// Restore the position to the initial position
	kc->kc_chain = temp_kc;	
	n1 = head;


	//
	// Printing the details of the chip_id present in the chip_ids[] array
	for(count = 0;chip_ids[count] != -1;count++)
	{
		kc->kc_chain = temp_kc;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		error_check_kstat_t(ksp);
		kstat_read(kc,ksp,NULL);
		knp = kstat_data_lookup(ksp,"chip_id");
	
		while(knp->value.i32 != chip_ids[count])
		{
			kc->kc_chain = ksp->ks_next;
			ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
			error_check_kstat_t(ksp);
			kstat_read(kc,ksp,NULL);
			knp = kstat_data_lookup(ksp,"chip_id");
		}
			
		printf("The physical processor has ");
		knp = kstat_data_lookup(ksp,"ncore_per_chip");
		if(knp->value.i32 == 1)
			printf("%d core and ",knp->value.i32);
		else	
			printf("%d cores and ",knp->value.i32);
		ncore = knp->value.i32;
		
		knp = kstat_data_lookup(ksp,"ncpu_per_chip");
		if(knp->value.i32 == 1)
			printf("%d virtual processor ",knp->value.i32);
		else
			printf("%d virtual processors (",knp->value.i32);
		ncpu = knp->value.i32;

		while(n1 != NULL)
		{
			if(n1->chip_id == chip_ids[count])
			{
				if(inst_core_flag == 1)
					printf("%d ",n1->core_id);
				else
					printf("%d ",n1->vp_id);
			}
				
			n1 = n1->next;
		}

		printf("\b)\n");

		if(inst_core_flag == 1)
		{
			knp = kstat_data_lookup(ksp,"implementation");
			printf("  %s\n",knp->value);
			knp = kstat_data_lookup(ksp,"brand");
			printf("  \t%s\n",knp->value);	
		}
		else
		{
			n1 = head;
			for(count1 = 0; count1 < ncore;count1++)
			{	
				printf("  The core has %d virtual prcessors (",ncpu/ncore);
				
				while(n1->chip_id != chip_ids[count])
					n1 = n1->next;
				for(count2 = 0;count2 < ncpu/ncore ;count2++)
				{
					printf("%d ",n1->vp_id);
					n1 = n1->next;
				}
				printf("\b)\n");
			}

			knp = kstat_data_lookup(ksp,"implementation");
			printf("    %s\n",knp->value);
			knp = kstat_data_lookup(ksp,"brand");
			printf("    \t%s\n",knp->value);	

		}


		
		kc->kc_chain = temp_kc;
		n1 = head;

	}







	kstat_close(kc);

}
//						//
//	End of the psrinfo_vp_opt_fn()		//
//						//




//
// This will work only when user gives s as a option
// It will produce 1 if that processor is online else 1
void psrinfo_s_sp_opt_fn(psrinfo_options* psropt)
{
	kstat_ctl_t*	kc;
	kstat_t*	ksp;
	kstat_named_t*  knp;
	kstat_t*	temp_kc;

	int ncore;				// Stores ncore_per_chip value
	int ncpu;				// Stores ncpu_per_chip value
	int inst_core_flag = 1;			// 1 -- use core_id  , 0 -- use instance
	int flag = 0;				


	// Opening the kstat for accessing the data from it
	kc = kstat_open();
	error_check_kstat_ctl_t(kc);
	temp_kc = kc->kc_chain;

	
	// Filling the value in the inst_core_flag accordingly
	ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	error_check_kstat_t(ksp);

	while(ksp != NULL)
	{
		kstat_read(kc,ksp,NULL);
		knp = kstat_data_lookup(ksp,"ncpu_per_chip");
		ncpu = knp->value.i32;
	
		knp = kstat_data_lookup(ksp,"ncore_per_chip");
		ncore = knp->value.i32;

		if(ncpu > ncore)
			inst_core_flag = 0;

		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);

	}

	kc->kc_chain = temp_kc;


	// For the given core_id/instance no check for its chip_id and print 1 if online else 0
	ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	error_check_kstat_t(ksp);

	while(ksp != NULL)
	{
		kstat_read(kc,ksp,NULL);
	
		if(inst_core_flag == 1)
		{
			knp = kstat_data_lookup(ksp,"core_id");
			if(knp->value.i32 == psropt->arguments[0])
			{
				flag = 1;
				knp = kstat_data_lookup(ksp,"chip_id");
				if(p_online(knp->value.i32,P_STATUS) == P_ONLINE)
					printf("1\n");
				else
					printf("0\n");	
			}	
		}
		else if(inst_core_flag == 0)
		{
			if(ksp->ks_instance == psropt->arguments[0])
			{
				flag = 1;
				knp = kstat_data_lookup(ksp,"chip_id");
				if(p_online(knp->value.i32,P_STATUS) == P_ONLINE)
					printf("1\n");
				else
					printf("0\n");
			}
		}
		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);

	}
	if(flag == 0)
		printf("psrinfo : processor %d : Invalid argument\n",psropt->arguments[0]);
		

	kstat_close(kc);

}
//						//
//	End of psrinfo_s_sp_opt_fn()		//
//						//




//
// This function displays the no of physical processor in the system if no argument is provided
// or no of physical processor corresponding to the given argment 
void psrinfo_p_opt_fn(psrinfo_options* psropt)
{
	kstat_ctl_t 	*kc;
	kstat_t	    	*ksp;
	kstat_t		*temp_kc;
	kstat_named_t   *knp;

	node* head;
	node* n1;
	node* end;	
	node* begin;
	node* min;

	int count = 0;			// Temp variable for FOR or counting
	int count1 = 0;			// Temp variable for FOR or counting
	int count2 = 0;			// Temp variable for FOR or counting
	int chip_ids[100];		// Stores the chip_id whose data is to be shown
	int flag  = 0;			// Temp flag variable
	int ncpu;			// Stores ncpu_per_chip value
	int ncore;			// Stores ncore_per_chip value
	int inst_core_flag = 1;		// 1 -- use core_id  , 0 -- use instance no
	int chip_flag = 0;		// 0 -- If chip_id not found , 1 -- if chip_id found   in chip_ids[] array

	//
	// Inititalizing chip_ids to -1
	for(count = 0;count < 100;count++)
		chip_ids[count]  = -1;

	//
	// Opening the kstat for accessing the kernal statistics
	kc = kstat_open();
	error_check_kstat_ctl_t(kc);

	temp_kc = kc->kc_chain;	

	
	//
	// Look the kstat chain for the kstat with module name "cpu_info" 
	ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	error_check_kstat_t(ksp);
	
	
	while(ksp != NULL)
	{
		//
		// Check whether instance no or core_id is to be used
		kstat_read(kc,ksp,NULL);
	
		knp = kstat_data_lookup(ksp,"ncore_per_chip");
		error_check_kstat_named_t(knp);
		ncore = knp->value.i32;
	
		knp = kstat_data_lookup(ksp,"ncpu_per_chip");
		error_check_kstat_named_t(knp);
		ncpu = knp->value.i32;
	

		if(ncore < ncpu)
			inst_core_flag = 0;

		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		
	}

	count = 0;

	//
	// Repositioning the kc pointer to the head of the chain for next work
	kc->kc_chain = temp_kc;


	//
	// Storing chip_id , core_id , instance no in an linklist
	ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	error_check_kstat_t(ksp);
	
	while(ksp != NULL)
	{
		kstat_read(kc,ksp,NULL);

		n1 = (node*)malloc(sizeof(node));
		n1->next = NULL;		

		knp = kstat_data_lookup(ksp,"chip_id");
		n1->chip_id = knp->value.i32;

		knp = kstat_data_lookup(ksp,"core_id");
		n1->core_id = knp->value.i32;

		n1->vp_id   = ksp->ks_instance;


		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
		
		if(count == 0)
			head = end = n1;
		else
		{
			end->next = n1;
			end       = n1;
		}
		count++;
	}

	// 
	// Restore the position to the initial position
	kc->kc_chain = temp_kc;


	
	//
	// Sorting the linklist
	begin = head;
	min   = head;
	n1    = begin->next;
	while(begin->next != NULL)
	{
		while(n1 != NULL)
		{
			if(n1->chip_id < min->chip_id)
				min = n1;
			else if(n1->chip_id == min->chip_id)
			{
				if(n1->core_id < min->core_id)
					min = n1;
				else if(n1->core_id == min->core_id)
				{
					if(n1->vp_id < min->vp_id)
						min = n1;
				}
			}
			n1 = n1->next;
		}
	
		if(begin == min)
		{
			begin = begin->next;
			goto cont;
			n1 = begin;
		}
		n1 = begin;
		while(n1->next != min)
				n1 = n1->next;
		n1->next = n1->next->next;
		if(begin == head)
		{
			min->next = head;			
			head = min;			
		}
		else
		{
			n1 = head;
			while(n1->next != begin)			
				n1 = n1->next;
			min->next = begin;
			n1->next = min;
		}
		cont:	
			n1 = begin->next;
			min = begin;
	}

	//Restore the position to the initial position
	kc->kc_chain = temp_kc;	
	n1 = head;
	
	// If argument is present the only those chip ids will be stored that matches the argument
	// If no argument is present then all the chip_ids will be stored
	if(psropt->arguments[0] != -1)
	{
		for(count = 0;psropt->arguments[count] != -1;count++)
		{
			while(n1 != NULL)
			{
				if(inst_core_flag == 0)
				{
					if(psropt->arguments[count] == n1->vp_id)
					{
						flag = 1;
						for(count1 = 0;chip_ids[count1] != -1;count1++)
						{			
							if(n1->chip_id == chip_ids[count1])
								chip_flag = 1;
						}
						if(chip_flag == 0)
							chip_ids[count2++] = n1->chip_id;
						chip_flag = 0;
					}
				
				}
				else if(inst_core_flag == 1)
				{
					if(psropt->arguments[count] == n1->core_id)
					{
						flag = 1;
						for(count1 = 0;chip_ids[count1] != -1;count1++)
						{			
							if(n1->chip_id == chip_ids[count1])
								chip_flag = 1;
						}
						if(chip_flag == 0)
							chip_ids[count2++] = n1->chip_id;
						chip_flag = 0;
					}
				}	
				n1 = n1->next;
			}
			if(flag == 0)
				printf("psrinfo : processor %d : Invalid argument\n",psropt->arguments[count]);
			n1 = head;
			flag = 0;
		}

	}
	else if(psropt->arguments[0] == -1)
	{
		while(n1 != NULL)
		{
			for(count1 = 0;chip_ids[count1] != -1;count1++)
			{			
				if(n1->chip_id == chip_ids[count1])
					chip_flag = 1;
			}
			if(chip_flag == 0)
				chip_ids[count2++] = n1->chip_id;
						
			n1 = n1->next;
			chip_flag = 0;
		}	

	}

	//
	// Sort the chip_ids
	qsort((void*)chip_ids,count2,sizeof(int),intcompare);
	
	// Printing no of chips 
	for(count = 0;chip_ids[count] != -1;count++);
	if(count > 0)
		printf("%d\n",count);

	kstat_close(kc);




}

//						//
//	End of psrinfo_p_opt_fn()		//
//						//

