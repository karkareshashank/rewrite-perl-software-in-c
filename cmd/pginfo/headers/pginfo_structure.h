#ifndef __pginfo_structure_H__
#define __pginfo_structure_H__


typedef struct pg_kstat_data
{
		int pg_id;
		int parent_pg_id;
		int ncpus;
		int cpus[1024];
				
		char char_cpus[1024];		
		char relationship[1024];
		char p_value[1024];

		struct pg_kstat_data* next;
}pg_ks;


typedef struct pginfo_options
{
	int	c_option_status;
	int	C_option_status;
	int	p_option_status;
	int	v_option_status;
	int	h_option_status;
	int	I_option_status;
	int	s_option_status;
	int	T_option_status;

	char	r_argument[1024];
	char	R_argument[1024];
	
	int	argument[1024];

	pg_ks*	pg_ks_head;
	

}pginfo_options;


void pginfo_struct_init(pginfo_options*);


#endif
