#ifndef __psrinfo_functions_H__
#define __psrinfo_functions_H__


// Including all the standard and necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <kstat.h>
#include <inttypes.h>
#include <sys/processor.h>
#include <sys/types.h>
#include <time.h>

// Including user defined header 
#include "psrinfo_struct_init.h"
#include "psrinfo_error_msg.h"

// 
// Defining the structure to construct component tree like linklist
typedef struct node{
			int chip_id;
			int core_id;
			int vp_id;
			struct node* next;
}node;


//
// These functions are for checking the error in calling any of the kstat_data_lookup(),
// kstat_read() , kstat_open() and kstat_lookup() functions
void error_check_kstat_ctl_t(kstat_ctl_t *kc);
void error_check_kstat_t(kstat_t *ksp);
void error_check_kstat_named_t(kstat_named_t *knp);

// This is the compare function used in qsort function to sort the array
static int intcompare(const void *p1, const void *p2);

//
// Functions for various combinations of the options provided
void psrinfo_no_opt_fn(psrinfo_options *psropt);
void psrinfo_v_opt_fn(psrinfo_options *psropt);
void psrinfo_vp_opt_fn(psrinfo_options *psropt);
void psrinfo_s_sp_opt_fn(psrinfo_options *psropt);
void psrinfo_p_opt_fn(psrinfo_options *psrinfo);









#endif
