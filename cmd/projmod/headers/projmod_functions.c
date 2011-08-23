#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <project.h>
#include <sys/types.h>
#include <sys/task.h>

#include "project_file_check.h"
#include "projmod_structure.h"


void remove_attrib_entry(char* attrib_value,char* rem_attrib_value)
{
	char* temp_attrib_value;
	char* temp_rm_attrib_value;
	char* single_value;
	char* single_rm_value;
	char* final_attrib_value;

	char* last1;
	char* last2;

	int count1 = 0;
	int count2 = 0;
	int count3 = 0;
	int flag   = 0;

	temp_attrib_value    = (char*)malloc(sizeof(char)*500);
	temp_rm_attrib_value = (char*)malloc(sizeof(char)*500);
	single_value	     = (char*)malloc(sizeof(char)*100);
	single_rm_value	     = (char*)malloc(sizeof(char)*100);	
	final_attrib_value   = (char*)malloc(sizeof(char)*500);

	strcpy(temp_attrib_value,attrib_value);
	strcpy(temp_rm_attrib_value,rem_attrib_value);

	single_rm_value = strtok_r(temp_rm_attrib_value,")",&last2);
	while(single_rm_value != NULL)
	{
		count2++;
		single_value = strtok_r(temp_attrib_value,")",&last1);
		while(single_value != NULL)
		{
			count1++;
			//printf("%s----%s\n",single_rm_value,single_value);
			if(strcmp(single_rm_value,single_value) != 0 && count1 == 1)	
			{	
				strcpy(final_attrib_value,single_value);
				strcat(final_attrib_value,"),");
			}
			else if(strcmp(single_rm_value,single_value) != 0 && count1 != 1)
			{
				strcat(final_attrib_value,single_value);
				strcat(final_attrib_value,"),");
			}
			else if(strcmp(single_rm_value,single_value) == 0)
				flag = 1;
			
			last1++;	
			single_value = strtok_r(NULL,")",&last1);
		}	
		if(flag == 0)
		{
			fprintf(stderr,"projmod : Project does not contain \"%s)\" value\n",single_rm_value);
			exit(2);
		}
		count1 = 0;
		last2++;
		single_rm_value = strtok_r(NULL,")",&last2);
		strcpy(temp_attrib_value,final_attrib_value);
	}

	final_attrib_value[strlen(final_attrib_value) - 1] = '\0';
	
	
	strcpy(attrib_value,final_attrib_value);

	free(last1);
	free(last2);
	free(single_value);
	free(single_rm_value);
	free(temp_attrib_value);
	free(temp_rm_attrib_value);
}



void seperate_attrib(char* attribute,char* attrib_name,char* attrib_value)
{
	int count = 0;
	int len   = strlen(attribute);
	int flag= 0;
	int count1 = 0;
	int count2 = 0;

	for(count = 0;count < len;count++)
	{
		if(attribute[count] == '=')
			flag = 1;
		else if(flag == 0)
		{
			attrib_name[count1] = attribute[count];
			count1++;
		}
		else if(flag == 1)
		{
			attrib_value[count2] = attribute[count];
			count2++;
		}
		attrib_value[count2] = '\0';
		attrib_name[count1]  = '\0';
	}
}


void remove_attrib(char* attrib_list,char* remove_attrib_list)
{
	char* temp_a_list;
	char* temp_ra_list;	
	char* final_attrib_list;
	char* attrib_name_value;
	char* rem_attrib_name_value;
	char  attrib_name[100];
	char  attrib_value[100];
	char  rem_attrib_name[100];
	char  rem_attrib_value[100];
	

	char* last1;
	char* last2;

	int flag  = 0;
	int count = 0;


	temp_a_list  	       = (char*)malloc(sizeof(char)*500);
	temp_ra_list	       = (char*)malloc(sizeof(char)*500);
	final_attrib_list      = (char*)malloc(sizeof(char)*500);
	attrib_name_value      = (char*)malloc(sizeof(char)*100);
	rem_attrib_name_value  = (char*)malloc(sizeof(char)*100);

	strcpy(temp_a_list,attrib_list);
	strcpy(temp_ra_list,remove_attrib_list);

	rem_attrib_name_value = strtok_r(temp_ra_list,";",&last2);

	while(rem_attrib_name_value != NULL)	
	{
		seperate_attrib(rem_attrib_name_value,rem_attrib_name,rem_attrib_value);
		attrib_name_value = strtok_r(temp_a_list,";",&last1);

		while(attrib_name_value != NULL)
		{
			seperate_attrib(attrib_name_value,attrib_name,attrib_value);
			printf("%s=%s ------ %s=%s\n",rem_attrib_name,rem_attrib_value,attrib_name,attrib_value);
			
			if(strcmp(attrib_name,rem_attrib_name) != 0 && count == 0)
			{
				strcpy(final_attrib_list,attrib_name_value);
				strcat(final_attrib_list,";");
				count++;
			}
			else if(strcmp(attrib_name,rem_attrib_name) != 0 && count != 0)
			{
				strcat(final_attrib_list,attrib_name_value);
				strcat(final_attrib_list,";");
			}
			else if(strcmp(attrib_name,rem_attrib_name) == 0)
			{
				
				if(rem_attrib_value[0] == '\0')
				{flag = 1;printf("here\n");}
				else
				{
					remove_attrib_entry(attrib_value,rem_attrib_value);	
					flag = 1;
					if(attrib_value[0] == '\0')
						flag = 1;
					else
					{
						
						if(count != 0  )
						{
							strcat(final_attrib_list,attrib_name);
							strcat(final_attrib_list,"=");
							strcat(final_attrib_list,attrib_value);
							strcat(final_attrib_list,";");
						}
						else
						{
							strcpy(final_attrib_list,attrib_name);
							strcat(final_attrib_list,"=");
							strcat(final_attrib_list,attrib_value);
							strcat(final_attrib_list,";");
						}
				
					}

				}
			}
			attrib_name_value = strtok_r(NULL,";",&last1);
		}
		if(flag == 0)
		{
			fprintf(stderr,"projmod : Project does not contain \"%s\"\n",rem_attrib_name);
			exit(2);
		}
		flag = 0;
		count = 0;
		strcpy(temp_a_list,final_attrib_list);
		rem_attrib_name_value = strtok_r(NULL,";",&last2);
	}




	


	final_attrib_list[strlen(final_attrib_list) -1] = '\0';
	attrib_list = final_attrib_list;
	printf("%s\n",attrib_list);


	free(temp_a_list);
	free(temp_ra_list);
	free(final_attrib_list);
	free(attrib_name_value);
	free(rem_attrib_name_value);
}

void remove_entry(char* ug_list,char* remove_ug_list)
{
	char* temp_ug_list;
	char* ug_name;
	char* rem_ug_name;
	char* temp_rem_ug_list;
	char* final_ug_list;

	char* last1;
	char* last2;

	int flag  = 0;
	int count = 0;

	
	temp_ug_list	 = (char*)malloc(sizeof(char)*100);
	ug_name	     	 = (char*)malloc(sizeof(char)*30);
	temp_rem_ug_list = (char*)malloc(sizeof(char)*100);
	rem_ug_name      = (char*)malloc(sizeof(char)*30);
	final_ug_list    = (char*)malloc(sizeof(char)*100);

	strcpy(temp_ug_list,ug_list);
	strcpy(temp_rem_ug_list,remove_ug_list);
	
	rem_ug_name = strtok_r(temp_rem_ug_list,",",&last2);
	while(rem_ug_name != NULL)
	{
		ug_name = strtok_r(temp_ug_list,",",&last1);
		
		while(ug_name != NULL)
		{
			
			if(strcmp(rem_ug_name,ug_name) != 0 && count != 0)
			{
				strcat(final_ug_list,ug_name);
				strcat(final_ug_list,",");
			}
			else if(strcmp(rem_ug_name,ug_name) != 0 && count == 0)
			{
				strcpy(final_ug_list,ug_name);
				strcat(final_ug_list,",");
				count++;
			}
			else if(strcmp(rem_ug_name,ug_name) == 0)
				flag = 1;

			ug_name = strtok_r(NULL,",",&last1);
			
		}
		if(flag == 0)
		{
			fprintf(stderr,"projmod : Project does not contain \"%s\"\n",rem_ug_name);
			exit(2);
		}
		flag = 0;
		count = 0;
		final_ug_list[strlen(final_ug_list) -1] = '\0';
		strcpy(temp_ug_list,final_ug_list);
		rem_ug_name = strtok_r(NULL,",",&last2);
	}

	
	ug_list = final_ug_list;
	
	free(final_ug_list);
	free(rem_ug_name);
	free(temp_rem_ug_list);
	free(ug_name);
	free(temp_ug_list);


}



char* get_proj_attrib_by_name(char* filename,char* proj_name)
{
	FILE* proj_file_ptr;

	char* proj_attribute = NULL;
	char* proj_detail_line;
	char* proj_detail_entry;
		
	int    length;
	size_t len;

	proj_file_ptr = fopen(filename,"r");
	if(proj_file_ptr == NULL)
	{
		fprintf(stderr,"Error opening %s : %s\n",filename,strerror(errno));
		exit(2);
	}

	proj_detail_entry = (char*)malloc(sizeof(char)*500);
	proj_detail_line  = (char*)malloc(sizeof(char)*1024);
	proj_attribute    = (char*)malloc(sizeof(char)*500);
	
	while(!feof(proj_file_ptr))
	{
		len = getline(&proj_detail_line,&length,proj_file_ptr);
		if(len == -1)
			break;
		proj_detail_line[len-1] = '\0';

		proj_detail_entry = strtok(proj_detail_line,":");
		if((proj_detail_entry,proj_name) == 0)
		{
			proj_detail_entry = strtok(NULL,":");
			proj_detail_entry = strtok(NULL,":");
			proj_detail_entry = strtok(NULL,":");
			proj_detail_entry = strtok(NULL,":");
			proj_detail_entry = strtok(NULL,":");
			
			if(proj_detail_entry == NULL)
				proj_attribute = NULL;
			else
				strcpy(proj_attribute,proj_detail_entry);

		}



	}

	fclose(proj_file_ptr);
	free(proj_detail_line);
	free(proj_detail_entry);

	return proj_attribute;
}

char* get_err_attrib(char* attribute_list,int pos)
{
	char* err_attrib;
	char* err_attrib_name;
	int count = 0;
	
	err_attrib 	= (char*)malloc(sizeof(char)*50);
	err_attrib_name = (char*)malloc(sizeof(char)*30);

	err_attrib = strtok(attribute_list,";");
	for(count = 1;count <= pos;count++)
	{
		err_attrib = strtok(NULL,";");
	}
	err_attrib_name = strtok(err_attrib,"=");

	return err_attrib_name;
}

void fn_for_A_option(char* proj_name,char* filename)
{
	char* attribute_list;
	char* err_attribute;	
	
	int ret_value;


	attribute_list    = (char*)malloc(sizeof(char)*1024);
	err_attribute     = (char*)malloc(sizeof(char)*20);



	ret_value = setproject(proj_name,"root",TASK_FINAL|TASK_PROJ_PURGE);
	if( ret_value  != 0)
	{
		if(ret_value == SETPROJ_ERR_TASK)
		{
			if(errno == EAGAIN)
				fprintf(stderr,"Resource control limit has been reached\n");
				
			else if(errno == ESRCH)
				fprintf(stderr,"User \"%s\" not member of project \"%s\"","root",proj_name);
			else if (errno == EACCES)
				fprintf(stderr,"The invoking task is final\n");
			else
				fprintf(stderr,"Could not join the project \"%s\"\n",proj_name);
		}
		else if(ret_value == SETPROJ_ERR_POOL)
		{
			if(errno == EACCES)
				fprintf(stderr,"No resource pool accepting,  default bindings exists for the project \"%s\"\n",proj_name);
			else if(errno == ESRCH)
				fprintf(stderr,"Specified resource pool does not exists for the project \"%s\"\n",proj_name);
			else
				fprintf(stderr,"Could not bind to default resource pool for project \"%s\"\n",proj_name);
		}
		else
		{
			if(ret_value <= 0)
				fprintf(stderr," setproject failed fo project \"%s\"\n",proj_name);
			else
			{
		                attribute_list = get_proj_attrib_by_name(filename,proj_name);
		 	        err_attribute  = get_err_attrib(attribute_list,ret_value-1);
		
				if(err_attribute == NULL)
					fprintf(stderr,"Warning ! , Resource control assignment failed for project \"%s\" ttribute %d \n",proj_name,ret_value);
				else
					fprintf(stderr,"Warning ! , %s resource control assignment failed for project \"%s\" \n",err_attribute,proj_name);
			}
		}
	}

	free(attribute_list);
	free(err_attribute);

}




void projmod_fn(projmod_options* projm)
{
	K_argument* temp_head;

	FILE* proj_file_ptr;
	FILE* temp_proj_file_ptr;
	
	char* temp_filename;
	char* proj_detail_line;
	char* temp_detail_line;
	char* proj_name;
	char* proj_detail_entry;
	char* invalid_user;
	char* invalid_group;
	char* proj_id;
	char* attribute_list;
	
	
	size_t len;
	int length;	
		
	
	proj_id 	  = (char*)malloc(sizeof(char)*10);
	temp_filename     = (char*)malloc(sizeof(char)*40);
	proj_detail_line  = (char*)malloc(sizeof(char)*1024);
	temp_detail_line  = (char*)malloc(sizeof(char)*1024);
	proj_detail_entry = (char*)malloc(sizeof(char)*500);
	proj_name         = (char*)malloc(sizeof(char)*30);
	invalid_user      = (char*)malloc(sizeof(char)*30);
	invalid_group	  = (char*)malloc(sizeof(char)*30);
	attribute_list	  = (char*)malloc(sizeof(char)*500);

	

	strcpy(proj_name,projm->project_name);
	// 
	// Project file 
	if(strcmp(projm->f_argument,"\0") == 0)
		strcpy(projm->f_argument,"/etc/project");

	strcpy(temp_filename,projm->f_argument);
	strcat(temp_filename,".temp");

	project_file_checker(projm->f_argument);


	//
	// Check for -A option and act according to it
	if(projm->A_option_status == 1)
	{
		fn_for_A_option(proj_name,projm->f_argument);
	} 
	
				

	//
	// Applying other options
	proj_file_ptr = fopen(projm->f_argument,"r+");
	if(proj_file_ptr == NULL)
	{
		fprintf(stderr,"Error opening %s : %s\n",projm->f_argument,strerror(errno));
		exit(2);
	}

	temp_proj_file_ptr = fopen(temp_filename,"wb");
	if(temp_proj_file_ptr == NULL)
	{
		fprintf(stderr,"Error opening %s: %s\n",temp_filename,strerror(errno));
		exit(2);	
	}
	



	
	while(!feof(proj_file_ptr))
	{
		len = getline(&proj_detail_line,&length,proj_file_ptr);
		if(len == -1)
			continue;
		strcpy(temp_detail_line,proj_detail_line);
		temp_detail_line[len-1] = '\0';


		proj_detail_entry = strsep(&proj_detail_line,":");

		if(strcmp(proj_detail_entry,projm->project_name) == 0)
		{
			//
			// checking for l argument is present
			if(strcmp(projm->l_argument,"\0") != 0)
			{
				strcpy(temp_detail_line,projm->l_argument);
				strcat(temp_detail_line,":");	
			}		
			else
			{
				strcpy(temp_detail_line,proj_detail_entry);
				strcat(temp_detail_line,":");
			}


			//
			// Checking if p argument is present together with o option
			proj_detail_entry = strsep(&proj_detail_line,":");
			if(projm->p_argument == atoi(proj_detail_entry))
			{
				sprintf(proj_id,"%d",projm->p_argument);
				if(projm->o_option_status == 1)
				{
					strcat(temp_detail_line,proj_id);
					strcat(temp_detail_line,":");
				}
				else
				{
					if(proj_id_check(projm->p_argument,projm->f_argument) == 0)
					{
						strcat(temp_detail_line,proj_id);
						strcat(temp_detail_line,":");
					}
					else
					{
						fprintf(stderr,"projmod: Duplicate projid \"%s\"\n",proj_id);	
						exit(2);
					}

				}
			}
			else
			{
				strcat(temp_detail_line,proj_detail_entry);
				strcat(temp_detail_line,":");
			}


			//
			// Checking for c argument
			proj_detail_entry = strsep(&proj_detail_line,":");
			if(strcmp(projm->c_argument,"\0") == 0)
			{
				if(proj_detail_entry != NULL)
					strcat(temp_detail_line,proj_detail_entry);

				strcat(temp_detail_line,":");
			}
			else
			{
				strcat(temp_detail_line,projm->c_argument);
				strcat(temp_detail_line,":");	
			}
			


			//
			// Checking for U argument
			proj_detail_entry = strsep(&proj_detail_line,":");
			if(strcmp(projm->U_argument,"\0") == 0)
			{
				if(proj_detail_entry != NULL)
					strcat(temp_detail_line,proj_detail_entry);

				strcat(temp_detail_line,":");
			}
			else
			{
				if(user_list_check(projm->U_argument) != NULL)
				{
					fprintf(stderr,"projmod: User \"%s\" does not exists\n",user_list_check(projm->U_argument));
					exit(2);
				}
				else 
				{	
					if(projm->a_option_status == 1)
					{
						strcat(proj_detail_entry,",");
						strcat(proj_detail_entry,projm->U_argument);
						strcat(temp_detail_line,proj_detail_entry);
						strcat(temp_detail_line,":");
					}
					else if(projm->r_option_status == 1)
					{
						remove_entry(proj_detail_entry,projm->U_argument);
						strcat(temp_detail_line,proj_detail_entry);
						strcat(temp_detail_line,":");
				 	}
					else
					{
						strcat(temp_detail_line,projm->U_argument);
						strcat(temp_detail_line,":");
				 	}
				}

			}



			
			//
			// Checking  for G argument
			proj_detail_entry = strsep(&proj_detail_line,":");
			if(strcmp(projm->G_argument,"\0") == 0)
			{
				if(proj_detail_entry != NULL)
					strcat(temp_detail_line,proj_detail_entry);

				strcat(temp_detail_line,":");
			}
			else
			{
				if(group_list_check(projm->G_argument) != NULL)
				{
					fprintf(stderr,"projmod : Group \"%s\" does not exist\n",group_list_check(projm->G_argument));
					exit(2);
				}
				else if(projm->a_option_status == 1)
				{
					strcat(proj_detail_entry,",");
					strcat(proj_detail_entry,projm->G_argument);
					strcat(temp_detail_line,proj_detail_entry);
					strcat(temp_detail_line,":");
				}
				else if(projm->r_option_status == 1)
				{
					remove_entry(proj_detail_entry,projm->G_argument);
					strcat(temp_detail_line,proj_detail_entry);
					strcat(temp_detail_line,":");
				}
				else
				{
					strcat(temp_detail_line,projm->G_argument);
					strcat(temp_detail_line,":");
				}
			}



			//
			// Checking for K arguments
			proj_detail_entry = strsep(&proj_detail_line,":");
			if(projm->head == NULL)
			{
				if(proj_detail_entry != NULL)
					strcat(temp_detail_line,proj_detail_entry);
				
			}
			else
			{
				temp_head = projm->head;
				strcpy(attribute_list,temp_head->data);
				while(temp_head->next != NULL)
				{
					temp_head = temp_head->next;
					strcat(attribute_list,";");
					strcat(attribute_list,temp_head->data);
				
				}



				if(projm->a_option_status == 1)
				{
					strcat(proj_detail_entry,";");
					strcat(proj_detail_entry,attribute_list);
					strcat(temp_detail_line,proj_detail_entry);
				}
				else if(projm->r_option_status == 1)
				{
					remove_attrib(proj_detail_entry,attribute_list);
					strcat(temp_detail_line,proj_detail_entry);	
				}
				else
				{	
					strcat(temp_detail_line,attribute_list);
				}

			}
		}

	
		fprintf(temp_proj_file_ptr,"%s\n",temp_detail_line);
		if(proj_detail_line == NULL)
			proj_detail_line  = (char*)malloc(sizeof(char)*1024);
	}


	fclose(proj_file_ptr);
	fclose(temp_proj_file_ptr);


	
	// We now remove the original project file and will rename temporary file 
	// to the original one
	if(remove(projm->f_argument) == -1)
	{
		fprintf(stderr,"Error: %s :Unable to update the project file\n",projm->f_argument);
		exit(2);
	}
	if(rename(temp_filename,projm->f_argument) == -1)
	{
		fprintf(stderr,"Error: %s :Unable to update the project file \n",projm->f_argument);
		exit(2);
	}
	


	free(proj_id);
	free(temp_detail_line);
	free(temp_filename);
	free(proj_detail_line);
	free(proj_detail_entry);
	free(invalid_user);
	free(invalid_group);
	free(proj_name);
	



}


