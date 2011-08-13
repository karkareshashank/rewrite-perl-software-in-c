#ifndef __project_file_check_H__
#define __project_file_check_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#define _GNU_SOURCE


void project_file_checker(char* );				// Checks the project file for any error


int 	projname_check(char*);					// Checks the project name for valid entry
char* 	user_list_check(char*);					// Checks the users from the passwd file
char* 	group_list_check(char*);				// Checks the group from the group file
int	colon_count(char*);					// Checks the given detail line for colons
int 	projname_dup_check(char*,char*);			// Checks for the duplicate of the name
int proj_id_check(int,char*);					// Checks the project file for given project id
int max_proj_id(char*);						// Finds the maximum project id in the file
 			

#endif
