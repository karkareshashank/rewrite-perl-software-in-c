#ifndef __projadd_structure_H__
#define __projadd_Structure_H__



/* structure K_argument will store all the
 * argument provided with option -K */
typedef struct K_argument
{
	char data[100];
	struct K_argument *next;
}K_argument;




/* Structure projadd_options will store 
 * all the aguments of the options common
 * in all the projadd/mod/del command      */
typedef struct projadd_options
{
	char f_argument[100];		//store filename provided with -f option
	char c_argument[100];		//store comment provided with -c option
	char G_argument[100];		//store group list provided with -G option
	char U_argument[100];		//store users list provided with -U option
	K_argument *head;		// store all the attributes provided with -K options
	int  p_argument;		//store project id given by the user with -p option
	int  n_option_status;		// 1 if n option is given
	int  o_option_status;		// 1 if o option is given
	char project_name[100];		// stores the project name given with the command
}projadd_options;


void init_structures(projadd_options*);


#endif
