#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "projadd_functions.h"
#include "projadd_error_msg.h"
#include "project_file_check.h"
#include "projadd_structure.h"


void projadd_fn(projadd_options *projadd)
{
	FILE* proj_file_ptr;			// Variable to store pointer to the project file

	char* project_detail_line;		// Variable to store the project details in order 
	char* project_id_char;			// Variable to store project id as string
	char* invalid_user;			// Variable to store invalid user returned from the user_list_check fn
	char* invalid_group;			// Variable to store invalid group returned from the group_list_check fn
	char* project_attribute;		// Variable to store all project attribute seperated by ';'

	K_argument* temp_head = NULL;		// pointer to point to head of the link-list containing all K arguments 
	

	//
	// Allocating space for all pointer variable
	project_detail_line = (char*)malloc(sizeof(char*)*1024);
	project_id_char     = (char*)malloc(sizeof(char*)*20);
	invalid_user        = (char*)malloc(sizeof(char*)*20);
	invalid_group       = (char*)malloc(sizeof(char*)*20);
	project_attribute   = (char*)malloc(sizeof(char*)*300);


	//
	// Check for project name and duplicate of it
	if(projname_check(projadd->project_name) == 1)
	{
		fprintf(stderr,"projadd: Invalid project name \"%s\", contains invalid characters\n",projadd->project_name);
		exit(2);
	}
	else
		strcpy(project_detail_line,projadd->project_name);
	
	if(projname_dup_check(projadd->project_name,projadd->f_argument) == 1)
	{
		fprintf(stderr,"projadd: Duplicate project name \"%s\"\n",projadd->project_name);
		exit(2);
	}
	strcat(project_detail_line,":");



	//
	// Check for projid 
	// If no id given by user , we find max project id in the project file 
	// and increment its value by 1 and set it as the new project id
	if(projadd->p_argument == -1)
	{
		projadd->p_argument = max_proj_id(projadd->f_argument);
		(projadd->p_argument)++;
	}
	else
	{
		// If option -o is not given then we have to check for duplicate project id in the file
		if(projadd->o_option_status == 0)
		{
			if(proj_id_check(projadd->p_argument,projadd->f_argument) == 1)
			{
				fprintf(stderr,"projadd: Duplicate projid \"%d\"\n",projadd->p_argument);
				exit(2);
			}
		}
	}	
	sprintf(project_id_char,"%d",projadd->p_argument);
	strcat(project_detail_line,project_id_char);
	strcat(project_detail_line,":");

	

	// 
	// Check for the comment
	if(strcmp(projadd->c_argument,"\0") == 0)
		strcat(project_detail_line,":");
	else
	{
		strcat(project_detail_line,projadd->c_argument);
		strcat(project_detail_line,":");
	}
	


	

	//
	// Check for Users
	if(strcmp(projadd->U_argument,"\0") == 0)
		strcat(project_detail_line,":");
	else
	{
		invalid_user = user_list_check(projadd->U_argument);
		if(invalid_user == NULL)
		{
			strcat(project_detail_line,projadd->U_argument);
			strcat(project_detail_line,":");
		}
		else
		{
			printf("projadd: User \"%s\" does not exist\n",invalid_user);
			exit(2);
		}

	}
	


	
	//
	// Check for group
	if(strcmp(projadd->G_argument,"\0") == 0)
		strcat(project_detail_line,":");
	else
	{
		
		invalid_group = group_list_check(projadd->G_argument);
		if(invalid_group == NULL)
		{
			strcat(project_detail_line,projadd->G_argument);
			strcat(project_detail_line,":");
		}
		else
		{
			printf("projadd: Group \"%s\" does not exist\n",invalid_group);
			exit(2);
		}

	}
	




	//
	// Adding attributes
	if(projadd->head != NULL)
	{
		temp_head = projadd->head;
		strcpy(project_attribute,temp_head->data);
		while(temp_head->next != NULL)
		{
			temp_head = temp_head->next;
			strcat(project_attribute,";");
			strcat(project_attribute,temp_head->data);
		}
	}
	strcat(project_detail_line,project_attribute);
	


	//
	// Adding the details to the file	
	//
	proj_file_ptr = fopen(projadd->f_argument,"a");
	if(proj_file_ptr == NULL)
	{
		fprintf(stderr,"Error opening %s : %s\n",projadd->f_argument,strerror(errno));
		exit(2);
	} 
	fprintf(proj_file_ptr,"%s\n",project_detail_line);
	

	fclose(proj_file_ptr);


	//
	// Freeing all the allocated variables
	free(project_detail_line);
	free(project_id_char);
	free(invalid_user);
	free(invalid_group);
	free(project_attribute);
	






}
