#include <stdio.h>
#include "lgrp_structure.h"

//
// Function to initialize the structure
void init_structure(lgrp_options* lgrpopt)
{
	int count;
	lgrpopt->a_option_status = 0;
	lgrpopt->c_option_status = 0;
	lgrpopt->C_option_status = 0;
	lgrpopt->e_option_status = 0;
	lgrpopt->G_option_status = 0;
	lgrpopt->I_option_status = 0;
	lgrpopt->l_option_status = 0;
	lgrpopt->L_option_status = 0;
	lgrpopt->m_option_status = 0;
	lgrpopt->P_option_status = 0;
	lgrpopt->r_option_status = 0;
	lgrpopt->t_option_status = 0;
	lgrpopt->T_option_status = 0;
	lgrpopt->u_option_argument = 'z';
	
	for(count = 0;count < 1024;count++)
		lgrpopt->lgrp_argument[count] = -5;

}

 
