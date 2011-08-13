#include <stdio.h>

#include "projadd_structure.h"

// This function initializes all the structures
void init_structures(projadd_options *projadd)
{
	
	strcpy(projadd->f_argument,"\0");
	strcpy(projadd->c_argument,"\0");
	strcpy(projadd->G_argument,"\0");
	strcpy(projadd->U_argument,"\0");
	projadd->head            = NULL;
	
	projadd->p_argument      = -1;
	projadd->n_option_status = 0;
	projadd->o_option_status = 0;
	strcpy(projadd->project_name,"\0");
	
	
}
