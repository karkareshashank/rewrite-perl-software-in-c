#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <umem.h>

#include "./headers/project_file_check.h"

void projdel_usage_fn()
{
	printf("Usage: projdel [-f filename] project\n");
}




int main(int argc,char **argv)
{
	FILE* proj_file_ptr;				// File pointer for project file
	FILE* temp_file_ptr;				// File pointre for temporary project file

	char* proj_filename;				// Stores project filename
	char* temp_filename;				// Stores temporay project filename
	char* proj_name_user;				// Stores user given project name
	char* proj_name_file;				// Stores project name extracted from file
	char* proj_detail_line;				// Stores the detail line to be written in temp file
	char* temp_detail_line;				// Stores the detail line used for extracting project name
	char  optstring[] 		= ":f:";	// Stores all the possible options used in projdel

	char  optch;					// Stores current option processed
		
	extern int 	optind;				// Stores the position of the option processed 
	int 	   	exit_status 	= 0;		// Stores the exit value in it
	int	   	f_flag     	= 0;		// 1 -- if -f option used else 0
	int	   	temp_argc;			// Temporary variable for storing argc value
	int 		match_flag	= 0;		// 1 -- if project name matched else 0
	size_t		len		= 1024;		// Used in getline()
	ssize_t		readlen		= 0;		// Stores the returned value from getline()
	

	optind = 1;					// Initializing the variable to 1 so that 1 option process start
	temp_argc = argc;			


	//
	// Allocating space for variables
	proj_filename 		= (char*)malloc(sizeof(char)*100);
	temp_filename 		= (char*)malloc(sizeof(char)*110);
	proj_name_user 		= (char*)malloc(sizeof(char)*100);
	proj_name_file 		= (char*)malloc(sizeof(char)*100);
	proj_detail_line	= (char*)malloc(sizeof(char)*1024);	
	temp_detail_line	= (char*)malloc(sizeof(char)*1024);	

	
	//
	// Coomand line argument processing is done below
	while((optch = getopt(argc,argv,optstring)) != -1)
	{
		switch(optch)
		{	
			case 'f':
				strcpy(proj_filename,optarg);
				f_flag = 1;
				break;

			case ':':
				printf("projdel : Option f require an argument \n");
				projdel_usage_fn();
				exit_status = 2;
				goto end;

			case '?':
				printf("projdel : Unknown option %c\n",optopt);
				projdel_usage_fn();
				exit_status = 2;
				goto end;
		}


	}



	// Displaying error accorind to the input given on the command line
	if(temp_argc == 1)
	{
		printf("projdel: Invalid command-line arguments\n");
		projdel_usage_fn();
		exit_status  =  2;
		goto end;	
	}
	else
	{
		if(optind == (temp_argc-1))
			strcpy(proj_name_user,argv[optind]);
		else
		{
			printf("projdel: Invalid command-line arguments\n");
			projdel_usage_fn();
			exit_status  =  2;
			goto end;
		}	
	}


	
	// If user have not used -f option the using default file /etc/project
	if(f_flag == 0)
		strcpy(proj_filename,"/etc/project");
	
	

	// generating temporary filename
	strcpy(temp_filename,proj_filename);
	strcpy(temp_filename,".temp");


	// Checking the file before using it
	project_file_checker(proj_filename);


	// Opening the project file and temporary file for future use
	proj_file_ptr = fopen(proj_filename,"r");
	if(proj_file_ptr == NULL)
	{
		fprintf(stderr,"projdel : Error in %s : %s \n",proj_filename,strerror(errno));
		exit_status = 5 ;
		goto end;
	}
	temp_file_ptr = fopen(temp_filename,"w");
	if(temp_file_ptr == NULL)
	{
		fprintf(stderr,"projdel : Error in %s : %s \n",temp_filename,strerror(errno));
		exit_status = 5 ;
		goto end;	
	}


	// Matching and performing the task according to the output of the match
	readlen = getline(&temp_detail_line,&len,proj_file_ptr);
	while(readlen != -1)
	{
		strcpy(proj_detail_line,temp_detail_line);

		proj_name_file = strsep(&temp_detail_line,":");
		if(strcmp(proj_name_file,proj_name_user) == 0)
			match_flag = 1;
		else
			fprintf(temp_file_ptr,"%s",proj_detail_line);
		
		readlen = getline(&temp_detail_line,&len,proj_file_ptr);

	}
	
	
	if(match_flag == 0)
	{
		printf("projdel : Project name \"%s\" not found in the file \"%s\"\n",proj_name_user,proj_filename);
		exit_status = 6;
		goto exit;
	}



exit:
	fclose(temp_file_ptr);
	fclose(proj_file_ptr);

	if(exit_status == 0)
	{
		if(remove(proj_filename) == -1)
		{
			printf("projdel : Error  in %s: %s \n",proj_filename,strerror(errno));
			if(remove(temp_filename) == -1)
			{
				printf("projdel : Error  in %s: %s \n",temp_filename,strerror(errno));
				exit_status  = 5;
				goto end;
			}
			exit_status = 5;
			goto end;
		}

		if(rename(temp_filename,proj_filename) == -1)
		{
			printf("projdel : Error  in %s: %s \n",proj_filename,strerror(errno));
			exit_status = 5;
			goto end;
		}
		
	}
	else
	{
		if(remove(temp_filename) == -1)
		{
			printf("projdel : Error  in %s: %s \n",proj_filename,strerror(errno));
			exit_status = 5;
			goto end;
		}
	}

	
end:
//	free(proj_filename);
//	free(temp_filename);
//	free(proj_name_user);
//	free(proj_name_file);
//	free(temp_detail_line);	
//	free(proj_detail_line);

	return exit_status;

}
