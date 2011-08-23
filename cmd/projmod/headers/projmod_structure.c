#include <stdio.h>

#include "projmod_structure.h"

// This function initializes all the structures
void init_structures(projmod_options *projm)
{
	
	strcpy(projm->f_argument,"\0");
	strcpy(projm->c_argument,"\0");
	strcpy(projm->G_argument,"\0");
	strcpy(projm->U_argument,"\0");
	projm->head            = NULL;
	
	projm->p_argument      = -1;
	projm->n_option_status = 0;
	projm->o_option_status = 0;
	strcpy(projm->project_name,"\0");

	projm->a_option_status   = 0;
	projm->r_option_status   = 0;
	projm->s_option_status   = 0;
	projm->A_option_status   = 0;
	strcpy(projm->l_argument,"\0");
	
	
}
