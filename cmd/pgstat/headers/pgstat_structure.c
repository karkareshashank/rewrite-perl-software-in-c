#include <stdio.h>
#include <string.h>

#include "pgstat_structure.h"

void init_pg(pg* new)
{
	new->pgid		= 0;
	new->parent_pg_id 	= -1;

	new->hw_load		= 0.0;
	new->hw_util		= 0.0;
	new->capacity		= 0.0;
	new->sw_load		= 0.0;
	new->usr		= 0.0;
	new->sys		= 0.0;
	new->idle		= 0.0;

	strcpy(new->relationship,"\0");
	strcpy(new->cpus,"\0");

	new->next		= NULL;

}

void pgstat_struct_init(pgstat_options* pgopt)
{
		int i;

		pgopt->A_option_status = 0;
		pgopt->C_option_status = 0;
		pgopt->p_option_status = 0;
		pgopt->v_option_status = 0;

		pgopt->s_argument = 0;
		pgopt->S_argument = 0;
		strcpy(pgopt->r_argument,"\0");
		strcpy(pgopt->R_argument,"\0");
		
		pgopt->t_argument	= -1;
		pgopt->T_argument	= 'z';
		pgopt->interval		= 0;
		pgopt->count		= -2;

		for(i = 0;i < 1024;i++)
		{
			pgopt->P_argument[i] = -1;
			pgopt->c_argument[i] = -1;
		}

		pg_ks*		pg_ks_head1 	= NULL;
		pg_ks*		pg_ks_head2 	= NULL;
		pg*		pg_head 	= NULL;
		pg_total*	pg_total_head 	= NULL;
		cpu_data*	cpu_head1 	= NULL;
		cpu_data*	cpu_head2	= NULL;

}
