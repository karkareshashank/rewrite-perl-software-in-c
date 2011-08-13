#include "psrinfo_struct_init.h"

//
// Function to initialize the psrinfo_option_status structure
void init_structure(psrinfo_options* psropt)
{
	int i;

	psropt->v_option_status = 0;
	psropt->s_option_status = 0;
	psropt->p_option_status = 0;
	
	for(i = 0;i < 100;i++)
		psropt->arguments[i] = -1;
}
