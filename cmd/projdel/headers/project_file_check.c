#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <umem.h>


#include "project_file_check.h"


// This function checks for the correctness of the project name
int projname_check(char proj_name[])
{
	int str_len;
	int count;
	
	str_len = strlen(proj_name);
	for(count = 0;count < str_len;count++)
	{
		if((proj_name[count] >= '0'    &&   proj_name[count] <= '9') || (proj_name[count] >= 'A'   &&   proj_name[count] <= 'Z') || (proj_name[count] >= 'a'   &&   proj_name[count] <= 'z' )|| proj_name[count] == '_' || proj_name[count] == '-' || proj_name[count] == '.')
			continue;
		else
			return 1;
	}

	return 0;


}


// This function will check for the valid user of the project
char* user_list_check(char temp_user_list[])
{
	FILE* passwd_file_ptr;			// Store file pointer of /etc/passwd file
	char* single_user = NULL;		// Stores username from the input
	char* user_detail_line = NULL;		// Contains one line of the /etc/passwd file
	char* file_user = NULL;			// Contains username from the above extracted line
	char* user_list = NULL;
	
	
	int flag = 0;				// Sets whether the user is found or not
	size_t pos_delim;
	size_t len;				// Contains position of hte first : in the user_detail_line
	int i;
	

	// Allocating space for the vairables
	single_user	 = (char*)malloc(sizeof(char)*20);
	file_user  	 = (char*)malloc(sizeof(char)*30);
	user_list   	 = (char*)malloc(sizeof(char)*100);

	
	strcpy(user_list,temp_user_list);
	
	// Opening the file /etc/passwd in READ mode only	
	passwd_file_ptr = fopen("/etc/passwd","r");
	if(passwd_file_ptr == NULL)
	{
		fprintf(stderr,"Error opening /etc/passwd : %s \n",strerror(errno));
		exit(6);
	}
	
	
	single_user = strtok(user_list,",");			// Extracts the users fro the user list
	while(single_user != NULL)
	{
		while(!feof(passwd_file_ptr))
		{
			getline(&user_detail_line,&len,passwd_file_ptr);	// Extract the line from the file
			
			file_user = strsep(&user_detail_line,":");
			if(strcmp(single_user,file_user) == 0)		// Compares it with the input username
			{
				flag = 1;
				break;
			}

				
			memset(file_user,0,30);			// Clears the array for hte further use			
		}
		
		if(flag == 0)
			return single_user;			// If match no found return that username
		flag = 0;	
		single_user = strtok(NULL,",");
		rewind(passwd_file_ptr);			// Puts the FILE pointer to the top of the file
		for(i = 0;i < 30;i++)
			file_user[i] = '\0';
	}	


	fclose(passwd_file_ptr);

	free(single_user);
	free(file_user);
	free(user_list);

	return NULL;





}





//
// This function checks valid group by matching them from the group file
//
char* group_list_check(char* temp_group_list)
{
	FILE* group_file_ptr;			// Store file pointer of /etc/group file
	char* single_group;			// Stores groupname from the input
	char* group_detail_line = NULL;		// Contains one line of the /etc/group file
	char* file_group;			// Contains groupname from the above extracted line
	char* group_list;
	
	
	int flag = 0;				// Sets whether the group is found or not
	size_t pos_delim;			// Contains position of hte first : in the group_detail_line
	size_t len;

	// Allocating space for the vairables
	single_group = (char*)malloc(sizeof(char*)*20);
	file_group   = (char*)malloc(sizeof(char*)*30);
	group_list   = (char*)malloc(sizeof(char*)*100);

	strcpy(group_list,temp_group_list);

	// Opening the file /etc/group in READ mode only	
	group_file_ptr = fopen("/etc/group","r");
	if(group_file_ptr == NULL)
	{
		fprintf(stderr,"Error opening /etc/group : %s \n",strerror(errno));
		exit(6);
	}
	
	
	single_group = strtok(group_list,",");			// Extracts the groups fro the group list
	while(single_group != NULL)
	{
		while(!feof(group_file_ptr))
		{
			getline(&group_detail_line,&len,group_file_ptr);	// Extract the line from the file
			
			pos_delim = strcspn(group_detail_line,":");
			strncpy(file_group,group_detail_line,pos_delim);	// Extract the group from the line
			
			if(strcmp(single_group,file_group) == 0)		// Compares it with the input groupname
			{
				flag = 1;
				break;
			}

				
			memset(file_group,0,30);		// Clears the array for hte further use			
		}
		
		if(flag == 0)
			return single_group;			// If match no found return that groupname
		flag = 0;	
		single_group = strtok(NULL,",");
		rewind(group_file_ptr);			// Puts the FILE pointer to the top of the file
		file_group[0] = '\0';
	}	


	fclose(group_file_ptr);

	free(single_group);
	free(group_detail_line);
	free(file_group);
	

	return NULL;


}


//
// This function counts the colons in the line 
int colon_count(char proj_detail_line[])
{
	int 	len;
	int 	i;
	int 	colons	= 0;

	len = strlen(proj_detail_line);
	
	for(i = 0 ; i < len ; i++)	
	{
		if(proj_detail_line[i] == ':')
			colons++;
	}

	if(colons != 5)	
		return 1;
	else
		return 0;
}





//
// This function checks the given file for errors like , valid project name , duplicate project name,
// project id is present or not ,Users and  Group exists or not
void project_file_checker(char* filename)
{
	FILE* file_ptr;					// Store file pointer to the project file

	
	char* proj_detail_line;				// Contains one line form the project file
	char* proj_detail_entry;			// Contains each entry from the above line
	char* error_user;				// Contains the unidentified user
	char* error_group;			 	// Contains the unidentified group



	//
	// Allocating space for the pointer variables
	proj_detail_line  = (char*)malloc(sizeof(char)*1024);
	proj_detail_entry = (char*)malloc(sizeof(char)*500);
	error_user	  = (char*)malloc(sizeof(char)*30);
	error_group	  = (char*)malloc(sizeof(char)*30);


	int length = 1024;					// Contains the length of the line from the project file
	int line_count = 1;				// Counts the line number of the project file
	int exit_flag = 0;				// 1 - If exit form this function , 0 - If no error found
	size_t len;





	// Opening the project file for checking it
	file_ptr = fopen(filename,"r");
	if(file_ptr == NULL)
	{
		fprintf(stderr,"Error reading %s : %s\n",filename,strerror(errno));
		exit(2);
	}



	//
	// For each line we check project name , Users  and the Group
	//
	while(!feof(file_ptr))
	{
		// Extracts one detail line from the project file
		len = 	getline(&proj_detail_line,&length,file_ptr);
		if(len == -1) 
			break;
		proj_detail_line[len-1] = '\0';


		// Check for the colons in the detail line
		if(colon_count(proj_detail_line) == 1)
		{
			printf("projdel : Parse error on line %d , Incorrect number of fields , Should have 5 \":\"'s.\n",line_count);
			exit_flag = 1;
		}


		//
		//Checking for the correctness of the project name
		proj_detail_entry = strsep(&proj_detail_line,":");
		if(projname_check(proj_detail_entry) == 1)
		{
			printf("projdel: Parse error on line %d, Invalid project name \"%s\", contains invalid characters\n",line_count,proj_detail_entry);
			exit_flag == 1;
		}




		//
		// Checking if the project ID is present or not
		proj_detail_entry = strsep(&proj_detail_line,":");
		if(strlen(proj_detail_entry) == 0)
		{
			printf("projdel: Parse error on line %d , Invalid project ID \"\"\n",line_count);
			exit_flag = 1;
		}


		// Comment
		proj_detail_entry = strsep(&proj_detail_line,":");



		//
		// Checking for the valid users
		proj_detail_entry = strsep(&proj_detail_line,":");
		error_user = user_list_check(proj_detail_entry);
		if(error_user != NULL)
		{
			printf("projdel : Parse error on line %d , Invalid user \"%s\"\n",line_count,error_user);
			exit_flag = 1;
		}



		//
		// Checking for the valid group
		proj_detail_entry = strsep(&proj_detail_line,":");
		error_group  = group_list_check(proj_detail_entry);
		if(error_group != NULL)
		{
			printf("projdel : Parse error on line %d , Invalid group \"%s\"\n",line_count,error_group);
			exit_flag = 1;
		}		


		// NO need to check Attribute section 


		line_count++;
	}



end:

	fclose(file_ptr);

//	free(cmd);
//	free(proj_detail_line);
//	free(proj_detail_entry);
//	free(error_user);
//	free(error_group);

	if(exit_flag == 1)
		exit(1);
	else
		return;

}
