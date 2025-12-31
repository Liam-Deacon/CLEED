/*********************************************************************
LD/24.04.14
  file contains functions:

  search_usage (24.04.14)
     Provides syntax help and other information before exiting

  search_info (24.04.14)
     Provides version information then exits
  
Changes:

*********************************************************************/

#include "search_optimizer.h"
#include "search_ver.h"

void search_usage(FILE *output) {
	fprintf(output,"usage: \t%s -i <inp_file> \n", SEARCH);
    fprintf(output, "      \t   [-d <delta> -v <vertex_file> -s <search_type> ...]\n");
    fprintf(output, "\n");
    fprintf(output, "Options:\n");
    fprintf(output, "  -b <bul_file>         : bulk parameter input file\n"
                    "                         (assumed same prefix as <inp_file>)\n");
	fprintf(output, "  -c <ctr_file>         : control file for IV curves\n"
                    "                         (assumed same prefix as <inp_file>)\n");
    fprintf(output, "  -d <delta>            : initial displacement\n");
    fprintf(output, "  -h --help             : print help and exit\n");
	fprintf(output, "  -i <inp_file>         : surface parameter input file\n");
    fprintf(output, "  -s <search_type>      : can be \n");
    sr_optimizer_print_help(output);
    fprintf(output, "  --max-evals <n>       : limit objective evaluations (simplex/PSO/DE).\n");
    fprintf(output, "  --max-iters <n>       : limit iterations (Powell/annealing/PSO/DE).\n");
    fprintf(output, "  --seed <n>            : seed stochastic optimizers (annealing/PSO/DE).\n");
    fprintf(output, "  --pso-swarm <n>       : set PSO swarm size.\n");
    fprintf(output, "  --pso-inertia <n>     : set PSO inertia weight.\n");
    fprintf(output, "  --pso-c1 <n>          : set PSO cognitive coefficient.\n");
    fprintf(output, "  --pso-c2 <n>          : set PSO social coefficient.\n");
    fprintf(output, "  --pso-vmax <n>        : set PSO velocity clamp.\n");
    fprintf(output, "  -v <vertex_file>      : file to read vertex information if resuming search\n");                
    fprintf(output, "  -V --version          : print version and information about this program\n");
    fprintf(output, "\n");
}

void search_info()
{
    printf("%s - version %s (%s)\n", SEARCH, SEARCH_VERSION, PROG_PLAT);
    printf("\n%s\n", SEARCH_SHORTDESC);
    printf("\n%s\n%s\n", SEARCH_COPYRIGHT, SEARCH_LICENSE);
    printf("\nContact either Georg Held (g.held@reading.ac.uk) " 
        "or %s for reporting bugs or submitting fixes.\n", SEARCH_MAINTAINER);
}
