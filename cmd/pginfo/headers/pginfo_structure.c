#include <stdio.h>
#include <string.h>

#include "pginfo_structure.h"


void pginfo_struct_init(pginfo_options* pgopt)
{
		int i;

		pgopt->c_option_status 	= 0;
		pgopt->C_option_status 	= 0;
		pgopt->h_option_status 	= 0;
		pgopt->I_option_status 	= 0;
		pgopt->p_option_status 	= 0;
		pgopt->s_option_status 	= 0;
		pgopt->T_option_status 	= 0;
		pgopt->v_option_status 	= 0;

		pg_ks*	pg_ks_head	= NULL;

		strcpy(pgopt->r_argument,"\0");
		strcpy(pgopt->R_argument,"\0");
		
		for(i = 0;i < 1024;i++)
			pgopt->argument[i] = -1;

		
		
}
