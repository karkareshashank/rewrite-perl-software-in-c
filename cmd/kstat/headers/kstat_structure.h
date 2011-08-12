#ifndef __kstat_structure_H__
#define __kstat_structure_H__

typedef struct mins_arg 
{

	char	m_argument[200];
	char	i_argument[200];
	char	n_argument[200];
	char	s_argument[200];
	struct mins_arg  *next;

}mins_arg;





typedef struct kstat_options
{

		int l_option_status;
		int p_option_status;
		int q_option_status;

		char T_argument;

		char c_arg[200];
		
		int interval;
		int count;
		mins_arg* head;
}kstat_options;


void init_structure(kstat_options*);
void mins_struct_init(mins_arg*);


#endif
