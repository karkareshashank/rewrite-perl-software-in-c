#include <stdio.h>
#include <string.h>

#include "kstat_structure.h"


// Initializes the kstat_options structure
void init_structure(kstat_options* kopt)
{

	kopt->l_option_status 	= 0;
	kopt->p_option_status 	= 0;
	kopt->q_option_status 	= 0;

	kopt->T_argument	= 'z';

	strcpy(kopt->c_arg,"*");
	
	kopt->interval		= 0;
	kopt->count		= -1;

	kopt->head		= NULL;

}


// Initializes the mins structure
void mins_struct_init(mins_arg* mins)
{
	strcpy(mins->m_argument,"*");
	strcpy(mins->i_argument,"*");
	strcpy(mins->n_argument,"*");
	strcpy(mins->s_argument,"*");


}
