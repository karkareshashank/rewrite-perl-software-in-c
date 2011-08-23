#ifndef __pgstat_structure_H__
#define __pgstat_structure_H__


typedef struct cpu_data
{
		int	cpu_id;
		int	cpu_ticks_idle;
		int	cpu_ticks_user;
		int	cpu_ticks_kernel;
	
		struct cpu_data* next;
}cpu_data;


typedef struct pg_kstat_data
{
		int ncpus;
		int cpus[1024];
		int pg_id;

		int hw_util;
		int hw_util_rate;
		int hw_util_rate_max;
		int hw_util_time_running;
		int hw_util_time_stopped;
		int snaptime;

		int generation;
		int parent_pg_id;
		
		char relationship[1024];

		struct pg_kstat_data* next;
}pg_ks;


typedef struct pg_data
{
		int 		pgid;
		int		parent_pg_id;
				
		float		hw_load;
		float  		hw_util;
		float		capacity;
		float 		sw_load;
		float		usr;
		float		sys;
		float 		idle;

		char		relationship[1024];
		char		cpus[1024];	
		
		struct pg_data* 	next;
}pg;



typedef struct pg_data_total
{
		int pgid;
	
		float hw_load_min;
		float hw_load_max;
		float hw_load_avg;

		float sw_load_min;
		float sw_load_max;
		float sw_load_avg;

		struct pg_data_total* next;

}pg_total;


typedef struct pgstat_options
{
	int	A_option_status;
	int	C_option_status;
	int	p_option_status;
	int	v_option_status;
	int	t_argument;
	int	P_argument[1024];
	int	c_argument[1024];
	int	interval;
	int	count;

	int	s_argument;
	int	S_argument;
	char	T_argument;
	char	r_argument[1024];
	char	R_argument[1024];
	
	pg_ks*		pg_ks_head1;
	pg_ks*		pg_ks_head2;
	pg*		pg_head;
	cpu_data*	cpu_head1;
	cpu_data*	cpu_head2;

	pg_total*	pg_total_head;


}pgstat_options;


void pgstat_struct_init(pgstat_options*);


#endif
