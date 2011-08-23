#ifndef __pgstat_fn_H__
#define __pgstat_fn_H__

void fill_space(char*);

int search_num(int,int*);

void cpu_list_fill(char*,int*);

void version_print();

int match_key(char*);

void c_P_option_function(pgstat_options*);

#endif
