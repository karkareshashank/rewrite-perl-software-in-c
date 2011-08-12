// Including pre-defined header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <kstat.h>
#include <time.h>
#include <inttypes.h>

// Including user-defined header files
#include "kstat_functions.h"
#include "kstat_structure.h"


// It prints all the statistics related to a particular kstat
// If -l,-p option is used then it prints in different form 
// using the prefix provided as an argument to the function
void kstat_data_look(kstat_t *ksp,char* prefix,int opt)
{
        int i, size;
        char *namep;
	char *datap;
	kstat_named_t* knp;

        switch (ksp->ks_type) {

        case KSTAT_TYPE_NAMED:
                size = sizeof (kstat_named_t);
                namep = KSTAT_NAMED_PTR(ksp)->name;
                break;

        case KSTAT_TYPE_TIMER:
                size = sizeof (kstat_timer_t);
                namep = KSTAT_TIMER_PTR(ksp)->name;
                break;

        default:
                errno = EINVAL;
                return;
        } 

	
	
        datap = ksp->ks_data;
        for (i = 0; i < ksp->ks_ndata; i++) 
	{
		
		knp = kstat_data_lookup(ksp,namep);
		if(strcmp(prefix,"\0") == 0)
		{
			if(knp->data_type == KSTAT_DATA_CHAR)
				printf("        %-30.30s            %-.30s\n",namep,knp->value.c);
			else if(knp->data_type == KSTAT_DATA_INT32)
				printf("        %-30.30s            %d\n",namep,knp->value.i32);
			else if(knp->data_type == KSTAT_DATA_UINT32)
				printf("        %-30.30s            %d\n",namep,knp->value.ui32);
			else if(knp->data_type == KSTAT_DATA_INT64)
				printf("        %-30.30s            %d\n",namep,knp->value.i64);			
			else if(knp->data_type == KSTAT_DATA_UINT64)
				printf("        %-30.30s            %d\n",namep,knp->value.ui64);
			else
				printf("        %-30.30s            %s\n",namep,knp->value);
		}
		else
		{
			printf("%s",prefix);
			if(opt == 1)
				printf("%s\n",namep);
			if(opt == 2)
			{
				if(knp->data_type == KSTAT_DATA_CHAR)
					printf("%s\t%s\n",namep,knp->value.c);
				else if(knp->data_type == KSTAT_DATA_INT32)
					printf("%s\t%d\n",namep,knp->value.i32);
				else if(knp->data_type == KSTAT_DATA_UINT32)
					printf("%s\t%d\n",namep,knp->value.ui32);
				else if(knp->data_type == KSTAT_DATA_INT64)
					printf("%s\t%d\n",namep,knp->value.i64);			
				else if(knp->data_type == KSTAT_DATA_UINT64)
					printf("%s\t%d\n",namep,knp->value.ui64);
				else
					printf("%s\t%s\n",namep,knp->value);
			}
		}



                namep += size;
                datap += size;
        }
        errno = ENOENT;
        return;
}




// This functions matches a given string to the given pattern 
// and return 1 if string satisfies the pattern else 0
// This pattern can contain "*" , "?" or "[1-5]" wildcard
int pattern_match(char pattern[] , char string[])
{
	int i 		= 0;
	int j 		= 0;
	int k 		= 0;
	int l		= 0;
	int m		= 0;

	int count	= 0;
	int com_count	= 0;
	int from	= 0;
	int to		= 0;
	int flag 	= 0;
	int str_j	= 0;
	int match_flag  = 1;
	int final_flag  = 0;

	char* numerals;
	char* num;
	char* chr_from;
	char* chr_to;

	numerals = (char*)malloc(sizeof(char)*100);
	num	 = (char*)malloc(sizeof(char)*20);
	chr_from = (char*)malloc(sizeof(char)*20);
	chr_to	 = (char*)malloc(sizeof(char)*20);


	// Scan every element of the pattern and act according to * ,? ,[] found
	while(pattern[i] != '\0')
	{
		if(pattern[i] == '?')				// If ? found in the pattern we increase the 
		{						// pattern and string count by 1 since ? can have 
			i++;					// character in it
			j++;
		}
		else if(pattern[i] == '*')			// If * is found in the pattern then we increase the
		{						// pattern count and continue to match the new pattern character
			i++;					// till we find a match to it in string or we reach end or []
			if(pattern[i] == '[')
			{
				for(;string[j] != '\0';j++)
				{
					if(string[j] >= '0' && string[j] <= '9')
						break;
					else
						continue;
				}					
				if(string[j] == '\0')
					return 0;
				else
					continue;

			}

			for(;string[j] != pattern[i];j++)
			{
				if(string[j] == '\0')
					return 0;
				else if(pattern[i] == '?')
					break;
			}

		}
		else if(pattern[i] == '[')				// If [ is found then we extract all the numbers
		{							// between [ and ] and we match each of them
			str_j = j;					// to the string numbers
			i++;
			while(pattern[i] != ']')
			{
				numerals[k] = pattern[i];
				i++;
				k++;
			}
			i++;
			numerals[k] = '\0';

			for(count=0;numerals[count] != '\0';count++)
				if(numerals[count] == ',')
					com_count++;

			for(count=0;count <= com_count;count++)
			{
				num = strsep(&numerals,",");

				for(l=0;l<strlen(num);l++)
					if(num[l] == '-')
						flag = 1;
				
				if(flag == 1)
				{
					for(l=0;num[l] != '-';l++)
						from = from * 10 + (num[l] - '0');

					l++;
					for(;num[l] != '\0';l++)
						to = to * 10 + (num[l] - '0');

				}
				else
				{
					for(l = 0;num[l] != '\0';l++)
						from = from * 10 + (num[l] - '0');
					to = from;
				}

				
				sprintf(chr_from,"%d",from);
				sprintf(chr_to  ,"%d",to);


				if(to <= from)
				{
					for(l = 0; l < strlen(chr_from);l++)
					{
						if(string[str_j] != chr_from[l]  || string[str_j] == '\0')
							match_flag = 0;
						str_j++;
					}
					if(match_flag  == 1)
					{
						final_flag = 1;
						j = str_j;
						break;
					}


					str_j = j;
					for(l = 0;l < strlen(chr_to);l++)
					{
						if(string[str_j] != chr_to[l]  || string[str_j] == '\0')
							match_flag = 0;
						str_j++;

					}
					if(match_flag  == 1)
					{
						final_flag = 1;
						j = str_j;
						break;
					}

	
				}
				else
				{
					for(l = from;l <= to;l++)
					{
						match_flag = 1;
						str_j = j;
						sprintf(chr_from,"%d",l);
						
						for(m = 0;m < strlen(chr_from) ;m++)
						{
							
							if(string[str_j] != chr_from[m]  || string[str_j] == '\0')
							{
								match_flag = 0;
								break;
							}
							str_j++;
						}
						if(match_flag == 1)
							break;
												
					}
					if(match_flag  == 1)
					{
						final_flag = 1;
						j = str_j;
					}
				}



				flag = 0;
				from = 0;
				to   = 0;
			}
			
		if(final_flag == 0)
			return 0;

		}
		else
		{
			if(string[j] == pattern[i])
			{
				i++;
				j++;
			}
			else
				return 0;
		}

	}
	if(string[j] != '\0')
		return 0;
	


	return 1;


}







// This function checks if the string provided only conatains digits or not
int all_digit(char* string)
{
	int i;

	for(i = 0; i < strlen(string) ; i++)
	{
		if(string[i] >= '0' && string[i] <= '9')
			continue;
		else
			return 0;
	}
	
	return 1;
}





// This function recieve the commandline pattern argument and a mins structure and fills it
// with respective strings extracted from the m:i:n:s pattern
void parse_pattern(mins_arg* new , char pattern[])
{
	int count	= 0;
	int count1	= 0;
	int col_count	= 0;

	char* temp_buf;
	char* entry;

	temp_buf = (char*)malloc(sizeof(char)*1024);
	entry    = (char*)malloc(sizeof(char)*200);
	
	strcpy(temp_buf,pattern);

	for(count = 0;count < strlen(pattern) ; count++)
		if(pattern[count] == ':')
			col_count++;
	
	if(temp_buf != NULL)
	{
		entry = strsep(&temp_buf,":");
		strcpy(new->m_argument,entry);
	}
	else
		strcpy(new->m_argument,"*");



	if(temp_buf != NULL)
	{
		entry = strsep(&temp_buf,":");
		strcpy(new->i_argument,entry);	
	}
	else
		strcpy(new->i_argument,"*");

	if(temp_buf != NULL)
	{
		entry = strsep(&temp_buf,":");
		strcpy(new->n_argument,entry);	
	}
	else
		strcpy(new->n_argument,"*");


	if(temp_buf != NULL)
	{
		entry = strsep(&temp_buf,":");
		strcpy(new->s_argument,entry);	
	}
	else
		strcpy(new->s_argument,"*");



	if(temp_buf != NULL)
		free(temp_buf);

	if(entry != NULL)
		free(entry);

	return;


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




// This functions open kstat  , travers through the kstat to find which one to display .
int kstat_fn(kstat_options* kopt)
{
	// Structures retaled to kstat
	kstat_ctl_t*	kc;
	kstat_t*	ksp;
	kstat_t*	temp_kc;
	kstat_t*	temp_ksp;
	kstat_named_t*	knp;

	// Structure for storing m,i,n,s options argument
	mins_arg*	mins;

	int 		count	= 0;
	int 		size;
	int		count1	= 0;
	int		match_flag = 0;

	char*		instance;
	char*		namep;
	char*		datap;	
	char*		prefix;

	time_t		timestamp;


	// Allocating space for variables
	instance = (char*)malloc(sizeof(char)*10);
	prefix	 = (char*)malloc(sizeof(char)*100);

	count1 = kopt->count;

	// Loop for kopt->count
	while(count1 != 0)
	{
		
			
		if(kopt->T_argument == 'd' || kopt->T_argument == 'u')
		{
			timestamp = time(0);
			if(kopt->T_argument == 'u')
				printf("%d\n",timestamp);
			else
				printf("%s\n",ctime(&timestamp));
		}
		// Open kstat link to access the data
		kc = kstat_open();
		error_check_kstat_ctl_t(kc);
		temp_kc = kc->kc_chain;
	
		mins = kopt->head;

		// Loop till the end of the mins link list
		while(mins != NULL)
		{
			ksp  = kc->kc_chain;
	
	
			// Scanning through the kstat chain
			while(ksp != NULL)
			{
				if(pattern_match(kopt->c_arg,ksp->ks_class) == 1)
				{
					if(pattern_match(mins->m_argument,ksp->ks_module) == 1)
					{
						sprintf(instance,"%d",ksp->ks_instance);
						if(pattern_match(mins->i_argument,instance) == 1)
						{
							if(pattern_match(mins->n_argument,ksp->ks_name) == 1)
							{
								if(kopt->l_option_status == 0 && kopt->p_option_status == 0 
								&& kopt->q_option_status == 0)
								{
									printf("module: %-30.30s  instance: %-6d\n",ksp->ks_module,ksp->ks_instance);
									printf("name:   %-30.30s  class:    %-.30s\n",ksp->ks_name,ksp->ks_class);
									temp_ksp = kstat_lookup(kc,ksp->ks_module,ksp->ks_instance,ksp->ks_name);
									kstat_read(kc,temp_ksp,NULL);
									kstat_data_look(temp_ksp,"\0",0);
									printf("\n\n");
								}
								else if((kopt->l_option_status == 1 || kopt->p_option_status == 1) || (kopt->l_option_status == 1 && kopt->p_option_status == 1))
								{
									sprintf(prefix,"%s:%s:%s:",ksp->ks_module,instance,ksp->ks_name);
									temp_ksp = kstat_lookup(kc,ksp->ks_module,ksp->ks_instance,ksp->ks_name);
									kstat_read(kc,temp_ksp,NULL);
									if(kopt->l_option_status == 1)
										kstat_data_look(temp_ksp,prefix,1);
									else if(kopt->p_option_status == 1)
										kstat_data_look(temp_ksp,prefix,2);
								}
								else if(kopt->q_option_status == 0)
									match_flag = 1;
								
							}
						}
					}
				}
				next:
					ksp = ksp->ks_next;
					
			}
			kc->kc_chain = temp_kc;
			mins = mins->next;
		}

		sleep(kopt->interval);	
	
		// close the kstat link
		kstat_close(kc);
		count1--;
		if(kopt->interval == 0)
			break;
	}

	if(match_flag == 1)
		return 0;
	else
		return 1;

}


