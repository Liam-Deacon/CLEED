/***********************************************************************
GH/28.09.00

file contains function:

  void sr_amoeba_gsl(gsl_matrix *p, gsl_vector *y, 
                 real epsilon, real (*funk)(), size_t *iter)

 Find minimum by simplex algorithm. (using GNU Scientific Library)

 Changes:

BN/   07.93 - Copy vertex to *.ver after each iteration.
GH/12.09.95 - Criterion of termination is absolute deviation in vertex
              rather than relative deviation.
LD/30.04.14 - Removed dependence on 'cp' and 'date' system calls
LD/01.07.2014 - Modified to use the GNU Scientific Library

***********************************************************************/
#include <math.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>

#include<gsl/gsl_matrix.h>
#include<gsl/gsl_vector.h>
#include<gsl/gsl_multimin.h>

#include "search.h"
#include "search_func.h"
#include "copy_file.h"

extern char *sr_project;

/* GSL -> CLEED wrapper function */
double sr_amoeba_eval(const gsl_vector *v, void *params)
{
    /*!TODO: fix this casting of object pointer to function pointer
             as it may cause undefined behaviour */
    //real (*funk)() = (real*) params; 
    int *dummy = (int *) params;
    dummy = dummy + 0;
    
    return sr_evalrf_gsl(v);
}


int sr_amoeba_gsl(
    gsl_matrix *p,   /* NxN matrix input */
    gsl_vector *y,   /* vector whose components must be pre-initialized to the 
                        values of func evaluated at the N+1 vertices of p */
    real epsilon,    /* fractional convergence tolerance */
    size_t *iter     /* number of function evaluations taken */
    )
{
    /* vertex output */
    FILE *vertex_stream;
    char vertex_file[STRSZ];
    char *old_file, *new_file;
    time_t timestamp;
    size_t np = y->size;       /* number of parameters */
    
    /* The parameter vector for the function defn: UNUSED */
    gsl_multimin_fminimizer *s ;
    gsl_vector *ss, *x;
    gsl_multimin_function func;
    size_t i, j;
    int status;
    double size;
    
    /* Initial vertex size vector */
    ss = gsl_vector_alloc(np);
    
    /* Set all step sizes to 1 */
    gsl_vector_set_all(ss, 1.0);
    
    /* Starting point */
    x = gsl_vector_alloc(np);
    
    /* Initialize method and iterate */
    func.f = &sr_amoeba_eval;
    func.n = np;
    func.params = NULL;
    //!func.params = (void *)funk;
    
    s = gsl_multimin_fminimizer_alloc(gsl_multimin_fminimizer_nmsimplex, np);
    gsl_multimin_fminimizer_set(s, &func, x, ss);
    
    /* iterate through minimiser */
    do {
        *iter += 1;
        status = gsl_multimin_fminimizer_iterate(s);
        
        if (status != GSL_CONTINUE && status != GSL_SUCCESS)
        {   
            fprintf(STDERR, "***error (sr_amoeba_gsl): %s\n", 
                    gsl_strerror(status));
            exit(status);
            break;
        }
        size = gsl_multimin_fminimizer_size(s);
        status = gsl_multimin_test_size(size, epsilon);
        
        for(j = 0; j < np; j++) 
            gsl_matrix_set(p, *iter, j, gsl_vector_get(s->x, j));
        
    } while (status == GSL_CONTINUE && *iter < MAX_ITER_AMOEBA);
        
/***************************************************************************
Write y/p to backup file
***************************************************************************/
    /* removed dependence on 'cp' & 'date' system calls */
    old_file = (char *) malloc(sizeof(char)*(strlen(sr_project)+5));
    new_file = (char *) malloc(sizeof(char)*(strlen(sr_project)+5));

    /* remove 'cp' system call dependence */
    strcpy(old_file, sr_project);
    strcpy(new_file, sr_project);
    strcat(old_file, ".ver");
    strcat(new_file, ".vbk");
    if (copy_file(old_file, new_file)) 
    {
        fprintf(STDERR, "*** error (sr_amoeba_): "
            "failed to copy file \"%s\" -> \"%s\"", old_file, new_file);
        exit(1);
    }

    strcpy(vertex_file, new_file);
    vertex_stream = fopen(vertex_file, "w");
    fprintf(vertex_stream,"%d %d %s\n", np, np+1, sr_project);
    for (i = 0; i < np+1; i++) 
    {
        fprintf(vertex_stream,"%e ", gsl_vector_get(y, i));
        for(j=0; j < np; j++) 
            fprintf(vertex_stream,"%e ", gsl_matrix_get(p, i, j));
        fprintf(vertex_stream,"\n");
    }
        
    /* add date */
    timestamp = time(NULL);
    fprintf(vertex_stream, "%s\n", asctime(localtime(&timestamp)));
    
    fclose(vertex_stream);
	
    
    /* free GSL memory */
    gsl_vector_free(x);
    gsl_vector_free(ss);
    gsl_multimin_fminimizer_free(s);

    return status;
}