#ifndef __lgrpinfo_functions_H__
#define __lgrpinfo_functions_H__

#include <sys/lgrp_user.h>

void lgrpinfo_fn(lgrp_options* lgrpopt);
// 

// Compare function used in qsort function
static int intcompare(const void *, const void *);


// Walk the lgroup hierarchy
int lgrp_walk(lgrp_cookie_t, lgrp_id_t, lgrp_content_t);

// Function to print lgroup id and if it is root , leaves or intermediate
void lgrpid_category(lgrp_cookie_t,lgrp_id_t);

// Functions for specific options 
void t_option_fn(lgrp_cookie_t,lgrp_id_t);
void r_option_fn(lgrp_cookie_t,lgrp_id_t,int);
char gives_unit_character(int);
void m_option_fn(lgrp_cookie_t,lgrp_id_t,int,int);
void l_option_fn(lgrp_cookie_t,lgrp_id_t,int);
void e_option_fn(lgrp_cookie_t,lgrp_id_t,int);
void c_option_fn(lgrp_cookie_t,lgrp_id_t,int);
void L_option_fn(lgrp_cookie_t,lgrp_id_t*);

void Ic_option_fn(lgrp_cookie_t,lgrp_id_t*);

void T_option_fn(lgrp_cookie_t, lgrp_id_t,int,lgrp_options*);


#endif
