#ifndef __lgrp_structure_H__
#define __lgrp_structure_H__

typedef struct lgrpinfo_options{
	int a_option_status;
	int c_option_status;
	int C_option_status;
	int e_option_status;
	int G_option_status;
	int I_option_status;
	int l_option_status;
	int L_option_status;
	int m_option_status;
	int P_option_status;
	int r_option_status;
	int t_option_status;
	int T_option_status;
	char u_option_argument;
	int lgrp_argument[1024];		    // -1 -- all , -2 -- intermidiate , -3 -- leaves , -4 -- root , x -- lgroup Id given by user
}lgrp_options;

//
// Function to initialize the structure
void init_structure(lgrp_options*);




#endif
