#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/lgrp_user.h>
#include <sys/types.h>
#include <kstat.h>
#include <search.h>

#include "lgrp_structure.h"




// Declaring global variable
int* lgrp_ids;
int  id_count = 0;
int level = 0;


//
// This function is used in qsort function
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



// Declaring recursive function to walk through the lgroup hierarchy 
/*
 * Walk and print lgroup hierarchy from given lgroup
 * through all its descendants
 */
int
lgrp_walk(lgrp_cookie_t cookie, lgrp_id_t lgrp, lgrp_content_t content)
{
	
	lgrp_id_t          *children;
	int                i;
	int                nchildren;
	

	if(id_count == 0)
		lgrp_ids[id_count] = lgrp;

	/*
	 * Get children
	 */
	nchildren = lgrp_children(cookie, lgrp, NULL, 0);
	if (nchildren == -1) {
		perror("lgrp_children");
		return (-1);
	} else if (nchildren > 0) {
		children = malloc(nchildren * sizeof (lgrp_id_t));
		nchildren = lgrp_children(cookie, lgrp, children, nchildren);
               	if (nchildren == -1) {
			free(children);
                        perror("lgrp_children");
			return (-1);
               	}
	
		for (i = 0; i < nchildren; i++)
		{
			id_count++;
			lgrp_ids[id_count] = children[i];
			lgrp_walk(cookie, children[i], content);
		}

		free(children);
	}
	
	return (0);
}





//
// Options specific functions 
//

void t_option_fn(lgrp_cookie_t cookie,lgrp_id_t lgrp)
{
	lgrp_id_t* children;
	lgrp_id_t* parents;

	int nchildren;
	int nparents;
	int i;

	
	// Get children
	nchildren = lgrp_children(cookie, lgrp, NULL, 0);

	printf("\tChildren: ");
	if(nchildren == 0)
		printf("none ");
	

	if (nchildren == -1)
        {
		perror("lgrp_children");
		exit(1);
	}
	else if (nchildren > 0) 
	{
		children = (lgrp_id_t*)malloc(nchildren * sizeof (lgrp_id_t));
		nchildren = lgrp_children(cookie, lgrp, children, nchildren);
               	if (nchildren == -1) 
		{
			free(children);
                        perror("lgrp_children");
			exit(1);
               	}
		for (i = 0; i < nchildren; i++)
			printf("%ld ", children[i]);
		

		
	}
	

	// Get Parents
	nparents = lgrp_parents(cookie, lgrp, NULL, 0);
	
	if (nparents == -1) 
	{
		perror("lgrp_parents");
		exit(1);
	}
	else if (nparents > 0) 
	{
		printf(", Parents: ");
		parents = (lgrp_id_t*)malloc(nparents * sizeof (lgrp_id_t));
		nparents = lgrp_parents(cookie, lgrp, parents, nparents);
               	if (nparents == -1) {
			free(parents);
                        perror("lgrp_parents");
			exit(1);
               	}
		for (i = 0; i < nparents; i++)
			printf("%ld ", parents[i]);
	
	}
	printf("\n");





//	free(children);
//	free(parents);
}

// end of t_option_fn() //




//
// Function to show resources of the the given lgroup
void r_option_fn(lgrp_cookie_t cookie, lgrp_id_t lgrp,int T_flag)
{
	lgrp_id_t* 	lgrpids;
	uint_t		count;
	int   		lgrp_count;
	int 		i;

	count = lgrp_nlgrps(cookie);
	
	lgrpids = (lgrp_id_t*)malloc(sizeof(lgrp_id_t)*count);
	
	// CPU resources
	if(T_flag != 1)
		printf("\t");
	printf("Lgroup resources: ");
	lgrp_count = lgrp_resources(cookie,lgrp,lgrpids,count,LGRP_RSRC_CPU);
	for(i = 0;i < lgrp_count;i++)
		printf("%ld ",lgrpids[i]);
	printf("(CPU); ");

	// Memory resoruces 
	lgrp_count = lgrp_resources(cookie,lgrp,lgrpids,count,LGRP_RSRC_MEM);
	for(i = 0;i < lgrp_count;i++)
		printf("%ld ",lgrpids[i]);
	printf("(memory)");
	printf("\n");
	

	free(lgrpids);

}
// 	End of r_option_fn()	//
//				//



//
// this function will return the unit character which is to be added
// to the end of the size of the memory
char gives_unit_character(int unit)
{
	switch(unit)
		{
			case 0:
				return 'B';
			case 1:
				return 'K';
			case 2:
				return 'M';
			case 3:
				return 'G';
			case 4:
				return 'T';
			case 5:
				return 'P';
			case 6:
				return 'E';
		}	
}





//
// Function to show the memory allocated 
void m_option_fn(lgrp_cookie_t cookie,lgrp_id_t lgrp,int unit,int T_flag)
{	
	lgrp_mem_size_t inst_size;
	lgrp_mem_size_t free_size;
	lgrp_mem_size_t allo_size;
	lgrp_content_t	content;

	float 		inst_size_float;
	float		free_size_float;
	float 		allo_size_float;

	char	 abb_unit;
	int	 count;
	

	if(lgrp == lgrp_root(cookie))
		content = LGRP_CONTENT_ALL;
	else
		content = LGRP_CONTENT_DIRECT;

	//
	// Get Installed memory size
	if(T_flag != 1)
		printf("\t");
	printf("Memory: ");
	inst_size = lgrp_mem_size(cookie, lgrp, LGRP_MEM_SZ_INSTALLED,content);
	if (inst_size == -1) {
		perror("lgrp_mem_size");
		exit(2);
	}
	
	//
	// Get free memory size
	free_size = lgrp_mem_size(cookie, lgrp, LGRP_MEM_SZ_FREE,content);
        if (free_size == -1) {
		perror("lgrp_mem_size");
		exit(2);
	}
	
	// Calculate allocated memory size
	allo_size = inst_size - free_size;

	// According to the agrument to the u option unit is provided
	if(unit >= 0)
	{
		abb_unit = gives_unit_character(unit);
	}
	else if(unit == 0)
		abb_unit = 'N';

	
	// Convert the memory size to appropriate unit
	if(unit >= 0)
	{
		inst_size_float = (float)inst_size;
		allo_size_float = (float)allo_size;
		free_size_float = (float)free_size;
		for(count = 0;count < unit;count++)
		{
			inst_size_float = inst_size_float/1024;
			allo_size_float = allo_size_float/1024;
			free_size_float = free_size_float/1024;
			inst_size = (int)inst_size_float;
			allo_size = (int)allo_size_float;
			free_size = (int)free_size_float;
		}
		if(inst_size > 10)
			printf("installed %d %c ",inst_size,abb_unit);		
		else
			printf("installed %.2f %c ",inst_size_float,abb_unit);	

		if(allo_size > 10)
			printf(", allocated %d %c ",allo_size,abb_unit);	
		else
			printf(", allocated %.2f %c ",allo_size,abb_unit);	

		if(free_size > 10)
			printf(", free %d %c \n",free_size,abb_unit);	
		else
			printf(", free %.2f %c \n",free_size_float,abb_unit);	
		


	}
	else
	{
		// For installed memory
		count = 0;
		inst_size_float = (float)inst_size;
		while(inst_size >= 1024)
		{
			inst_size_float = inst_size_float/1024;
			inst_size = (int)inst_size_float;
			count++;
		}
		abb_unit = gives_unit_character(count);
		if(inst_size > 10)
			printf("installed %d ",inst_size);
		else
			printf("installed %.2f ",inst_size_float);
		printf("%c ,",abb_unit);
		


		// For allocated memory
		count = 0;
		allo_size_float = (float)allo_size;
		while(allo_size >= 1024)
		{
			allo_size_float = allo_size_float/1024;
			allo_size = (int)allo_size_float;
			count++;
		}
		abb_unit = gives_unit_character(count);
		if(allo_size > 10)
			printf("allocated %d ",allo_size);
		else
			printf("allocated %.2f ",allo_size_float);
		printf("%c ,",abb_unit);



		// For free memory
		count = 0;
		free_size_float = (float)free_size;
		while(free_size >= 1024)
		{
			free_size_float = free_size_float/1024;
			free_size = (int)free_size_float;
			count++;
		}
		abb_unit = gives_unit_character(count);
		if(free_size > 10)
			printf("free %d ",free_size);
		else
			printf("free %.2f ",free_size_float);
		printf("%c \n",abb_unit);


	}


}
//	End of the m_option_fn()	//
//					//





// 
// This function will display all the CPUs in the given lgroup
void c_option_fn(lgrp_cookie_t cookie , lgrp_id_t lgrp,int T_flag)
{
	int		ncpus;
	int 		i;
	processorid_t*	cpuids;
	lgrp_content_t  content;


	if(lgrp == lgrp_root(cookie))
		content = LGRP_CONTENT_ALL;
	else
		content = LGRP_CONTENT_DIRECT;
	/*
	 * Get CPUs
	 */
	ncpus = lgrp_cpus(cookie, lgrp, NULL, 0, content);
	if(T_flag != 1)
		printf("\t");
	printf("CPUs: ");

	if (ncpus == -1) {
		perror("lgrp_cpus");
		exit(2);
	} else if (ncpus > 0) {
		cpuids = malloc(ncpus * sizeof (processorid_t));
		ncpus = lgrp_cpus(cookie, lgrp, cpuids, ncpus, content);
                if (ncpus == -1) {
			free(cpuids);
               	        perror("lgrp_cpus");
			exit(2);
		}

		qsort(cpuids,ncpus,sizeof(processorid_t),intcompare);
		for (i = 0; i < ncpus; i++)
			printf("%d ", cpuids[i]);
		free(cpuids);
	}
	printf("\n");




}
//	End of the c_option_fn()	//
//					//





//
// This function displays the load of the given lgroup
void e_option_fn(lgrp_cookie_t cookie , lgrp_id_t lgrp,int T_flag)
{
	kstat_ctl_t*	kc;
	kstat_t*	ksp;
	kstat_named_t*	knp;

	int 		load_avg;
	int 		load_scale;

	char*		lgrp_name;
	char*		lgrp_id;	

	lgrp_name = (char*)malloc(sizeof(char)*10);
	lgrp_id	  = (char*)malloc(sizeof(char)*10);

	strcpy(lgrp_name,"lgrp");
	sprintf(lgrp_id,"%d",lgrp);
	strcat(lgrp_name,lgrp_id);


	kc = kstat_open();
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



	// Look the kstat chain for the kstat with module name "lgrp" 
	ksp = kstat_lookup(kc,"lgrp",-1,NULL);
	if(ksp == NULL)
	{
		if(errno == EINVAL)
			fprintf(stderr,"An attempt was made to look up data for a kstat that was not of\
					type KSTAT_TYPE_NAMED or KSTAT_TYPE_TIMER\n");
		else if(errno == ENOENT)
			fprintf(stderr,"The request kstat could not be found\n");

		exit(1);
	}


	while(ksp != NULL)
	{
		kstat_read(kc,ksp,NULL);

		if(strcmp(lgrp_name,ksp->ks_name) == 0)
		{
			knp = kstat_data_lookup(ksp,"load average");	
			load_avg = knp->value.i32;

			knp = kstat_data_lookup(ksp,"loadscale");
			load_scale = knp->value.i32;
		}


		kc->kc_chain = ksp->ks_next;
		ksp = kstat_lookup(kc,"cpu_info",-1,NULL);
	}
	
	
	if(lgrp != lgrp_root(cookie))
	{
		if(T_flag != 1)
			printf("\t");
		printf("Load : %.3f\n",((float)load_avg/(float)load_scale));
	}
	else
	{
		if(lgrp_nlgrps(cookie) == 1)
		{
			if(T_flag != 1)
				printf("\t");
			printf("Load : %.3f\n",((float)load_avg/(float)load_scale));	
		}
	}
	free(lgrp_name);
	free(lgrp_id);



}
//	End of the e_option_fn()	//
//					//




//
// This function will display the latency of the given lgroup
void l_option_fn(lgrp_cookie_t cookie,lgrp_id_t lgrp,int T_flag)
{
	int latency;
	
	latency = lgrp_latency(lgrp,lgrp_root(cookie));
	if(T_flag != 1)
		printf("\t");
	printf("Latency : %d\n",latency);


}
//	End of the l_option_fn()	//
//					//





//
// This function prints the lgroup id and and its category
void lgrpid_category(lgrp_cookie_t cookie,lgrp_id_t lgrp)
{
	int nchildren;
	int nparents;

	printf("Lgroup %d ",lgrp);
	
	nparents = lgrp_parents(cookie, lgrp, NULL, 0); 
	nchildren = lgrp_children(cookie, lgrp, NULL, 0);

	if(nparents == 0)
		printf("(root)");
	else if(nparents > 0 && nchildren == 0)
		printf("(leaf)");
	else if(nparents > 0 && nchildren > 0)
		printf("(intermediate)");
	printf(" :\n");
	

}
// 	End of the lgrpid_category()	//
//					//



//
// This function will display the latency table between given and all lgropus
void L_option_fn(lgrp_cookie_t cookie , lgrp_id_t *lgrp , int len)
{
	int max_latency 	= 0;
	int latency;
	int temp_lgrp_id;
	int max_lgrp_id;
	int space_len;
	int lgrp_len;
	int latency_len;
	int final_space_len;
	int temp_latency;

	int count 		= 0;
	int count1 		= 0;
	int count2		= 0;



	// We find here the maximum latency in all lgroup
	for(count = 0;count < len;count++)
	{
		for(count1 = 0;count1 <= id_count;count1++)
		{
			latency = lgrp_latency(lgrp[count] , lgrp_ids[count1]);
			if(latency > max_latency)
				max_latency = latency;
		}
	}

	// We calculate the length of the maximum latency
	for(count=0; max_latency > 0;count++)
		max_latency = max_latency / 10;

	space_len = count;
	
	
	// We find here the length of the maximum lgroup ID
	temp_lgrp_id = lgrp_ids[id_count];
	if(temp_lgrp_id > 0)
	{
		for(count=0;temp_lgrp_id > 0;count++)
			temp_lgrp_id = temp_lgrp_id / 10;
	}
	else
		count = 1;
	lgrp_len = count;


	if(lgrp_len > space_len)
		final_space_len = lgrp_len + 1;
	else 
		final_space_len = space_len + 1;


	
	// Print top line in header
	for(count = 1;count <= (final_space_len*(id_count+1))+(lgrp_len + 2); count++)
		printf("-");
	printf("\n");

	// Print the lgroup ID display line in header
	for(count= 0 ;count < (lgrp_len +1);count++)
		printf(" ");
	printf("|");
	for(count = 0;count <= id_count ; count++)
	{
		temp_lgrp_id = lgrp_ids[count];
		if(temp_lgrp_id > 0)
			for(count1 = 0;temp_lgrp_id > 0;count1++)
				temp_lgrp_id = temp_lgrp_id / 10;	
		else
			count1 = 1;
		
		for(count2 = 1;count2 <= (final_space_len - count1);count2++)
			printf(" ");
		printf("%d",lgrp_ids[count]);
	}
	printf("\n");
	
	// Print bottom line of header
	for(count = 1;count <= (final_space_len*(id_count+1))+(lgrp_len+2) ; count++)
		printf("-");
	printf("\n");



	// Now displaying the latency for each combinations
	for(count = 0;count < len;count++)
	{
		printf("%d |",lgrp[count]);
		for(count1 = 0;count1 <= id_count;count1++)
		{
			latency = lgrp_latency(lgrp[count],lgrp_ids[count1]);
			temp_latency = latency;
			if(temp_latency > 0)
				for(latency_len = 0;temp_latency > 0;latency_len++)
					temp_latency = temp_latency / 10;
			else
				latency_len = 1;
			for(count2=0;count2 < (final_space_len - latency_len);count2++)
				printf(" ");
			printf("%d",latency);
		}
		printf("\n");
	}

	// Print bottom line of header
	for(count = 1;count <= (final_space_len*(id_count+1))+(lgrp_len+2) ; count++)
		printf("-");
	printf("\n");




}
//	End of the L_option_fn()	//
//					//


//
// This function prints all the CPU related to the given lgroup IDs list
void Ic_option_fn(lgrp_cookie_t cookie, lgrp_id_t* lgrp)
{
	int 		ncpus;
	int 		count 		= 0;
	int		count1		= 0;
	int		count2		= 0;
	int 		flag  		= 0;
	int		match_flag 	= 0;
	int		cpu_count	= 0;

	processorid_t*  cpuids;
	processorid_t*	temp_cpuids;
	lgrp_content_t	content;
	lgrp_id_t	rootid;



	if(lgrp[0] == lgrp_root(cookie))
		content = LGRP_CONTENT_ALL;
	else
		content = LGRP_CONTENT_DIRECT;

	rootid = lgrp_root(cookie);

	// Calculating maximum possible cpus and then allocating space for cpuids according to it	
	ncpus 		= lgrp_cpus(cookie,lgrp_root(cookie),NULL,0,LGRP_CONTENT_ALL);
	cpuids		= (processorid_t*)malloc(sizeof(processorid_t)*ncpus);
	temp_cpuids 	= (processorid_t*)malloc(sizeof(processorid_t)*ncpus);

	
	// Initializing cpuids and temp_cpuids to -1
	for(count = 0;count < ncpus;count++)
		cpuids[count] = temp_cpuids[count] = -1;
	

	// Checking if root lgroup ID is present or not
	// If present then we will directly prints its CPUs and exit
	// else would find CPUs for each lgroup ID and display them
	for(count = 0;lgrp[count] != -1;count++)
	{
		if(lgrp[count] == rootid)
		{
			
			flag = 1;
			break;
		}
	}

	if(flag == 1)
	{
		ncpus = lgrp_cpus(cookie,rootid,NULL,0,LGRP_CONTENT_ALL);
		ncpus = lgrp_cpus(cookie,rootid,cpuids,ncpus,LGRP_CONTENT_ALL);
		qsort(cpuids,ncpus,sizeof(processorid_t),intcompare);
		for(count = 0;count < ncpus;count++)
			printf("%d ",cpuids[count]);
		printf("\n");
		goto exit;
	}
	else
	{
		for(count = 0;lgrp[count] != -1;count++)
		{
			ncpus = lgrp_cpus(cookie,lgrp[count],NULL,0,LGRP_CONTENT_DIRECT);
			ncpus = lgrp_cpus(cookie,lgrp[count],temp_cpuids,ncpus,LGRP_CONTENT_DIRECT);
			for(count1 = 0;count1 < ncpus;count1++)
			{
				for(count2 = 0;cpuids[count2] != -1;count2++)
				{
					if(temp_cpuids[count1] == cpuids[count2])
						match_flag = 1;
				}
				if(match_flag == 0)
					cpuids[cpu_count++] = temp_cpuids[count1];
				match_flag = 0;
			}
		}

	}
	
	if(flag == 0)
	{
		qsort(cpuids,cpu_count,sizeof(processorid_t),intcompare);
		for(count = 0;count < cpu_count;count++)
			printf("%d ",cpuids[count]);
		printf("\n");		

	}	


exit:
	free(cpuids);
	free(temp_cpuids);

}
//	End of the Ic_option_fn()	//
//					//



//
// This function will print the lgroup Tree
void T_option_fn(lgrp_cookie_t cookie, lgrp_id_t lgrp, int nchild,lgrp_options* lgrpopt)
{
	
	lgrp_id_t               *children;
	int 	                i;
	int			count		= 0;
	int		        nchildren;
	char*			prefix;	
	char*			connect;
	int			temp_nchildren;	

	prefix  = (char*)malloc(sizeof(char)*100);
	connect = (char*)malloc(sizeof(char)*10);
	


	if(level == 0)
	{
		printf("%d\n",lgrp);
		strcpy(prefix,"|   ");
	}
	else
	{
		if(level == 1)
		{
			if(nchild == 1)
				strcpy(prefix,"`-- ");
			else
				strcpy(prefix,"|-- ");
			
			strcpy(connect,"");
		}
		else if(level > 1)
		{
			strcpy(prefix,"|  ");
			for(i = 0;i < (level-2);i++)
				strcat(prefix,"   ");
			if(nchild == 1)
				strcpy(connect,"`-- ");
			else
				strcpy(connect,"|-- ");

		}
		printf("%s%s%d\n",prefix,connect,lgrp);
			
	
	}

	if(lgrp_nlgrps(cookie) > 1)
	{
		if(level >= 1)
		{
			strcpy(prefix,"|  ");
			for(count=0;count < level-2;count++)
				strcat(prefix,"   ");
			strcat(prefix," ");

			if(lgrpopt->c_option_status == 1)
			{
				printf("%s",prefix);		
				c_option_fn(cookie,lgrp,1);
			}
			if(lgrpopt->m_option_status == 1)
			{
				printf("%s",prefix);
				m_option_fn(cookie,lgrp,-1,1);	
			}
			if(lgrpopt->r_option_status == 1)
			{
				printf("%s",prefix);
				r_option_fn(cookie,lgrp,1);
			}
			if(lgrpopt->e_option_status == 1)
			{
				printf("%s",prefix);
				e_option_fn(cookie,lgrp,1);
			}
			if(lgrpopt->l_option_status == 1)
			{
				printf("%s",prefix);
				l_option_fn(cookie,lgrp,1);
			}
		}
	}
	else
	{
		strcpy(prefix,"|   ");
		if(lgrpopt->c_option_status == 1)
		{
			printf("%s",prefix);
			c_option_fn(cookie,lgrp,1);
		}
		if(lgrpopt->m_option_status == 1)
		{
			printf("%s",prefix);
			m_option_fn(cookie,lgrp,-1,1);	
		}
		if(lgrpopt->r_option_status == 1)
		{
			printf("%s",prefix);		
			r_option_fn(cookie,lgrp,1);
		}
		if(lgrpopt->e_option_status == 1)
		{
			printf("%s",prefix);
			e_option_fn(cookie,lgrp,1);
		}
		if(lgrpopt->l_option_status == 1)
		{
			printf("%s",prefix);
			l_option_fn(cookie,lgrp,1);
		}
		

	}


next:
	/*
	 * Get children
	 */
	nchildren = lgrp_children(cookie, lgrp, NULL, 0);
	if (nchildren == -1) {
		perror("lgrp_children");
		exit(1);
	} else if (nchildren > 0) {
		children = malloc(nchildren * sizeof (lgrp_id_t));
		nchildren = lgrp_children(cookie, lgrp, children, nchildren);
               	if (nchildren == -1) {
			free(children);
                        perror("lgrp_children");
			exit(1);
               	}
		level++;
		temp_nchildren = nchildren;
		for (i = 0; i < nchildren; i++)
		{
			T_option_fn(cookie, children[i],temp_nchildren--,lgrpopt);
		}

		free(children);
	}
	
	return;
}
//	End of the T_option_fn()	//
//					//





//
// This function  works whenever lgrpinfo command is used
void lgrpinfo_fn(lgrp_options* lgrpopt)
{


	// If lgrp version supports then we proccede else we exit	
	if (lgrp_version(LGRP_VER_CURRENT) != LGRP_VER_CURRENT)
	{
		fprintf(stderr, "Built with unsupported lgroup interface %d\n",LGRP_VER_CURRENT);
		exit (1);
	}

	
	// Declaring the variables
	lgrp_cookie_t   cookie;
	lgrp_id_t 	rootid;	
	lgrp_id_t*	lgrp_ids_use;

	int 		count  		= 0;
	int 		count1 		= 0;
	int		count2		= 0;
	int 		match_flag	= 0;
	int		match_count	= 0;
	int		id_count_use	= 0;
	int		nchildren;
	int		nparents;

	lgrp_id_t*	temp_lgrp_ids;
	int		temp_id_count = 0;
	int 		flag = 0;
	lgrp_id_t*	lgrp_array;

	


start:
	// Opening the lgrp for accessing the data
	if(lgrpopt->G_option_status == 1)
		cookie = lgrp_init(LGRP_VIEW_OS);
	else
		cookie = lgrp_init(LGRP_VIEW_CALLER);
	


	// Checking if lgrp is opened successfully
	if(cookie == NULL)
	{
		if(errno == EINVAL)
			fprintf(stderr,"lgrpinfo : View is invalid : %s\n",strerror(errno));
		else if(errno == ENOMEM)
			fprintf(stderr,"lgrpinfo : Insufficient memory : %s\n",strerror(errno));

		exit(1);
 
	}	


	// Checking if cookie is stale or not
	if(lgrp_cookie_stale(cookie) == 1)
	{
		lgrp_fini(cookie);
		goto start;
	}

	// Finding number of lgroups and allocating space for ids according to it
	lgrp_ids	 =       (int*)malloc(sizeof(int)*lgrp_nlgrps(cookie));
	lgrp_ids_use 	 = (lgrp_id_t*)malloc(sizeof(lgrp_id_t)*lgrp_nlgrps(cookie));
	temp_lgrp_ids	 = (lgrp_id_t*)malloc(sizeof(lgrp_id_t)*lgrp_nlgrps(cookie));
	lgrp_array	 = (lgrp_id_t*)malloc(sizeof(lgrp_id_t)*lgrp_nlgrps(cookie));

	// Inititalizing the temp_lgrp_ids to -1
	for(count = 0;count < lgrp_nlgrps(cookie);count++)
		lgrp_ids_use[count]  = temp_lgrp_ids[count] = -1;



	// Finding the root lgroup id
	rootid = lgrp_root(cookie);


	// Storing all the lgroup ids in an array	
	lgrp_walk(cookie,rootid,LGRP_CONTENT_HIERARCHY);
	

	if(lgrpopt->lgrp_argument[0] == -1)
	{
		// If lgrp_argument given by user is "all"
		for(count = 0;count <= id_count;count++)
			lgrp_ids_use[count] = lgrp_ids[count];
		id_count_use = id_count;
	}
	else if(lgrpopt->lgrp_argument[0] == -2)
	{	
		// If lgrp_argument given by user is "intermediate"
		count1 = 0;
		for(count = 0;count <= id_count;count++)
		{
			nchildren = lgrp_children(cookie,lgrp_ids[count],NULL,0);
			nparents  = lgrp_parents(cookie,lgrp_ids[count],NULL,0);
			if(nchildren > 0 && nparents > 0)
				lgrp_ids_use[count1++] = lgrp_ids[count];
					
		}
		id_count_use = count1;
	}
	else if(lgrpopt->lgrp_argument[0] == -3)
	{
		// If lgrp_argument given by user is "leaves"
		count1 = 0;
		for(count = 0;count <= id_count;count++)
		{
			nchildren = lgrp_children(cookie,lgrp_ids[count],NULL,0);
			nparents  = lgrp_parents(cookie,lgrp_ids[count],NULL,0);
			if(nchildren == 0 && nparents > 0)
				lgrp_ids_use[count1++] = lgrp_ids[count];
					
		}
		id_count_use = count1;

	}
	else if(lgrpopt->lgrp_argument[0] == -4)
	{
		count1 = 0;
		// If lgrp_argument given by user is "root"
		lgrp_ids_use[count1] = lgrp_root(cookie);
		id_count_use = 1;
	}
	else if(lgrpopt->lgrp_argument[0] >= 0)
	{
		// If lgrp_argument given by the user is lgrp ids
		count   = 0;
		count1  = 0;
		count2  = 0;
		while(lgrpopt->lgrp_argument[count1] !=  -5)
		{
			for(count = 0;count <= id_count;count++)
			{
				if(lgrpopt->lgrp_argument[count1] == lgrp_ids[count])
				{
					lgrp_ids_use[count2++] = lgrp_ids[count];
					match_flag = 1;
					match_count++;
					break;
				}
			}
			if(match_flag == 0)
				printf("lgrpinfo : Skipping invalid lgroup %d\n",lgrpopt->lgrp_argument[count1]);
				
			
			match_flag = 0;
			count1++;
		}
		id_count_use = count2 -1;
		// If no match found then we would report the error and stop the program
		if(match_count  ==  0)
		{
			fprintf(stderr, " lgrpinfo : No matching lgroup found \n");
			exit(2);
		}
	}

	

/*	printf("lgrp_ids-use = ");
 *	for(count = 0;count <= id_count_use;count++)
 *		printf("%d  ",lgrp_ids_use[count]);
 *	printf("\n");	
 */	
	count =0 ;
	qsort(lgrp_ids , id_count , sizeof(int) , intcompare);
	qsort(lgrp_ids_use , id_count_use , sizeof(int) , intcompare);


	
	// Changing the lgroup IDs according to option provided (i.e.  -C  / -P)
	if(lgrpopt->P_option_status == 1)
	{
		for(count = 0;count <= id_count_use;count++)
		{
			nparents = lgrp_parents(cookie,lgrp_ids_use[count],NULL,0);
			if(nparents > 0)
			{
				nparents = lgrp_parents(cookie,lgrp_ids_use[count],lgrp_array,nparents);
				for(count1 = 0;count1 < nparents ;count1++)
				{
					for(count2 = 0;temp_lgrp_ids[count2] != -1;count2++)
					{
						if(lgrp_array[count1] == temp_lgrp_ids[count2])
							flag = 1;
					}
					if(flag == 0)
						temp_lgrp_ids[temp_id_count++] = lgrp_array[count1];
					flag = 0;
				}
			}
		}
	}
	else if(lgrpopt->C_option_status == 1)
	{
		for(count = 0;count <= id_count_use;count++)
		{
			nchildren = lgrp_children(cookie,lgrp_ids_use[count],NULL,0);
			if(nchildren > 0)
			{
				nchildren = lgrp_children(cookie,lgrp_ids_use[count],lgrp_array,nparents);
				for(count1 = 0;count1 < nchildren ;count1++)
				{
					for(count2 = 0;temp_lgrp_ids[count2] != -1;count2++)
					{
						if(lgrp_array[count1] == temp_lgrp_ids[count2])
							flag = 1;
					}
					if(flag == 0)
						temp_lgrp_ids[temp_id_count++] = lgrp_array[count1];
					flag = 0;
				}
			}
		}


	}	
	

	if(lgrpopt->C_option_status == 1 || lgrpopt->P_option_status == 1)
	{
		lgrp_ids_use = temp_lgrp_ids;
		id_count_use = temp_id_count - 1;
	}



	if(lgrp_ids_use[0] == -1)
	{
		fprintf(stderr, "lgrpinfo : No matching lgroups found !\n");
		exit(2);
	}



	if(lgrpopt->T_option_status == 0)
	{	
		if(lgrpopt->I_option_status == 0)	
		{		
			for(count = 0;count <= id_count_use;count++)
			{
			
				lgrpid_category(cookie,lgrp_ids_use[count]);
				if(lgrpopt->t_option_status == 1)
					t_option_fn(cookie,lgrp_ids_use[count]);
				if(lgrpopt->c_option_status == 1)
					c_option_fn(cookie,lgrp_ids_use[count],0);
				if(lgrpopt->m_option_status == 1)
					m_option_fn(cookie,lgrp_ids_use[count],-1,0);	
				if(lgrpopt->r_option_status == 1)
					r_option_fn(cookie,lgrp_ids_use[count],0);
				if(lgrpopt->e_option_status == 1)
					e_option_fn(cookie,lgrp_ids_use[count],0);
				if(lgrpopt->l_option_status == 1)
					l_option_fn(cookie,lgrp_ids_use[count],0);
			}
		}
		else
		{	
			
			if(lgrpopt->c_option_status == 1 && lgrpopt->a_option_status == 0)
				Ic_option_fn(cookie,lgrp_ids_use);
			else
			{
				
				for(count = 0; count <= id_count_use;count++)
					printf("%d ",lgrp_ids_use[count]);
				printf("\n");
			}
				
		}
	
	}
	else if(lgrpopt->T_option_status == 1)
	{
		if(lgrpopt->lgrp_argument[0] != -1)
			fprintf(stderr,"lgrpinfo:Warning : with '-T' all lgroups on the command line are ignored\n\n");
	
		T_option_fn(cookie,rootid,lgrp_children(cookie,rootid,NULL,0),lgrpopt);

	}

	if(lgrpopt->L_option_status == 1 && lgrpopt->I_option_status == 0)
	{
			printf("\nLgroup Latencies : \n");
			L_option_fn(cookie , lgrp_ids_use , id_count_use+1);
	}

	
	
	
	// Closing the lgrp 
	lgrp_fini(cookie);
	free(lgrp_ids);
	free(lgrp_ids_use);

}














