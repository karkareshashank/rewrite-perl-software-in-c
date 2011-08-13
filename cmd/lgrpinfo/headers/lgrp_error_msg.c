#include "lgrp_error_msg.h"
#include <stdio.h>

void lgrp_usage_error_print()
{
	printf("Usage: lgrpinfo [-aceGlLmrt] [-u unit] [-C|-P] [lgrp] ...\n");
      	printf("       lgrpinfo -I [-c] [-G] [-C|-P] [lgrp] ...\n");
      	printf("       lgrpinfo -T [-aceGlLmr] [-u unit]\n");
      	printf("       lgrpinfo -h\n");

}


void mutually_exclusive_error_print()
{
	fprintf(stderr,"lgrpinfo : Option -C , -P , -T cannot be used togather\n");
	lgrp_usage_error_print();
}


void cannot_be_used_with_error_print(char opt,char* with_opt)
{
	fprintf(stderr,"lgrpinfo : Option %c cannot be used with %s\n",opt,with_opt);
	lgrp_usage_error_print();
}

void lgrp_help_message()
{
	printf("    Display information about locality groups\n");
	printf("\n");
	printf("    \t-a: Equivalent to \"-celLmrt\" without -T and to \"-celLmr\" with -T\n");
	printf("    \t-c: Print CPU information\n");
	printf("    \t-C: Children of the specified lgroups\n");
	printf("    \t-e: Print lgroup load average\n");
	printf("    \t-h: Print this message and exit\n");
	printf("    \t-I: Print lgroup or CPU IDs only\n");
	printf("    \t-l: Print information about lgroup latencies\n");
	printf("    \t-G: Print OS view of lgroup hierarchy\n");
	printf("    \t-L: Print lgroup latency table\n");
	printf("    \t-m: Print memory information\n");
	printf("    \t-P: Parent(s) of the specified lgroups\n");
	printf("    \t-r: Print lgroup resources\n");
	printf("    \t-t: Print information about lgroup topology\n");
	printf("    \t-T: Print the hierarchy tree\n");
	printf("    \t-u unit: Specify memory unit (b,k,m,g,t,p,e)\n");
	printf("\n");
	printf("\n");
        printf("    The lgrp may be specified as an lgroup ID, \"root\", \"all\",\n");
	printf("  \"intermediate\" or \"leaves\".\n");
	printf("\n");
        printf("    The default set of options is \"-celmrt all\"\n");
	printf("\n");
	printf("    Without any options print topology, CPU and memory information about each\n");
	printf("    lgroup. If any lgroup IDs are specified on the command line only print\n");
	printf("    information about the specified lgroup.\n");

}

