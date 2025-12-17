/***********************************************************************
 GH/23.10.95

 file contains function:

  int sr_rdver(char * ver_file, real *y, real **p, int ndim)

 Read vertex for simplex algorithm from backup file.

 Changes:

GH/11.09.95 - Creation
GH/28.09.95 - Do not check dimensions if ndim is -1.
GH/23.10.95 - fix bug in the fgets calls (n_str instead of STRSZ)

***********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>
// cppcheck-suppress missingIncludeSystem
#include <ctype.h>

#include "search.h"

#ifdef ERROR
#ifdef EXIT_ON_ERROR
#define ALLERR(x) \
        fprintf(STDERR," *** error (sr_rdver): allocation error (%s)\n",x); \
        exit(1)
#else
#define ALLERR(x) \
        fprintf(STDERR," *** error (sr_rdver): allocation error (%s)\n",x); \
        return(-1)
#endif
#else      /* ifdef ERROR */
#ifdef EXIT_ON_ERROR
#define ALLERR(x) exit(1)
#else
#define ALLERR(x) return(-1)
#endif
#endif

/**********************************************************************/

static FILE *sr_rdver_open(const char *ver_file)
{
  FILE *fp = fopen(ver_file, "r");
  if (fp == NULL)
  {
#ifdef ERROR
    fprintf(STDERR,
            " *** error (sr_rdver): could not open file \"%s\"\n", ver_file);
#endif
#ifdef EXIT_ON_ERROR
    exit(1);
#else
    return NULL;
#endif
  }
  return fp;
}

static int sr_rdver_read_header(FILE *ver_stream, const char *ver_file,
                                int ndim_in, int *out_ndim, int *out_mpar)
{
  char linebuffer[STRSZ];
  while (fgets(linebuffer, (int)sizeof(linebuffer), ver_stream) != NULL) {
    if (linebuffer[0] != '#') break;
  }

  int file_ndim = 0;
  int file_mpar = 0;
  if (sscanf(linebuffer, "%d %d", &file_ndim, &file_mpar) != 2) {
#ifdef ERROR
    fprintf(STDERR,
            " *** error (sr_rdver): invalid header in \"%s\"\n", ver_file);
#endif
#ifdef EXIT_ON_ERROR
    exit(1);
#else
    return -1;
#endif
  }

  if (ndim_in < 0)
  {
#ifdef WARNING
    fprintf(STDWAR,
            " * warning (sr_rdver): ndim < 0: dimensions are not checked\n");
#endif
    *out_ndim = file_ndim;
    *out_mpar = file_mpar;
    return 0;
  }

  if (file_ndim != ndim_in) {
#ifdef ERROR
    fprintf(STDERR,
            " *** error (sr_rdver): dimensions do not match: %d/%d, %d/%d\n",
            file_ndim, ndim_in, file_mpar, ndim_in + 1);
#endif
#ifdef EXIT_ON_ERROR
    exit(1);
#else
    return -1;
#endif
  }
  if (file_mpar != ndim_in + 1) {
#ifdef ERROR
    fprintf(STDERR,
            " *** error (sr_rdver): dimensions do not match: %d/%d, %d/%d\n",
            file_ndim, ndim_in, file_mpar, ndim_in + 1);
#endif
#ifdef EXIT_ON_ERROR
    exit(1);
#else
    return -1;
#endif
  }

  *out_ndim = ndim_in;
  *out_mpar = file_mpar;
  return 0;
}

static const char *sr_rdver_skip_ws(const char *p)
{
  while (*p != '\0') {
    if (!isspace((unsigned char)*p)) break;
    p++;
  }
  return p;
}

static int sr_rdver_parse_vertex_line(const char *line, int ndim, real *out_y, real *out_p)
{
  const char *ptr = sr_rdver_skip_ws(line);
  if (*ptr == '\0') return 1;
  if (*ptr == '#') return 1;

  char *endptr = NULL;
  real yval = (real)strtod(ptr, &endptr);
  if (endptr == (char *)ptr) return -1;
  ptr = endptr;

  for (int j_par = 1; j_par <= ndim; j_par++)
  {
    out_p[j_par] = (real)strtod(ptr, &endptr);
    if (endptr == (char *)ptr) return -1;
    ptr = endptr;
  }

  *out_y = yval;
  return 0;
}

static int sr_rdver_read_vertices(FILE *ver_stream, const char *ver_file,
                                  real *y, real **p, int ndim, int m_par)
{
  char linebuffer[STRSZ];
  int i_par = 1;

  while (fgets(linebuffer, (int)sizeof(linebuffer), ver_stream) != NULL)
  {
    if (i_par > m_par) break;
#ifdef CONTROL_X
    fprintf(STDCTR, "(%s): %s", ver_file, linebuffer);
#endif

    int rc = sr_rdver_parse_vertex_line(linebuffer, ndim, &y[i_par], p[i_par]);
    if (rc < 0) return -1;
    if (rc == 0) i_par++;
  }

  return (i_par == m_par + 1) ? 0 : -1;
}

int sr_rdver(const char * ver_file, real *y, real **p, int ndim)

/***********************************************************************

 Read vertex for simplex algorithm from backup file.
 
INPUT:
  char *ver_file - (input) input file containing start geometry for 
                 SEARCH.

  real *y - (output) function values at the vertex positions.
  real **p - (output) coordinates of the vertex.

  int ndim - (input) dimension of y and p. If ndim < 0, do not check
             dimensions.

NOTE: memory for y and p must be allocated before entering the function.
             y = vector(1, ndim)
             p = matrix(1, ndim+1,1, ndim)

DESIGN:

FUNCTION CALLS:

RETURN VALUES:

    1 if ok.
   -1 if failed (and EXIT_ON_ERROR is not defined)

***********************************************************************/
{

FILE *ver_stream = NULL;
int m_par = 0;

/********************************************************************
  START INPUT
  Open and Read vertex file
********************************************************************/

 ver_stream = sr_rdver_open(ver_file);
#ifndef EXIT_ON_ERROR
 if (ver_stream == NULL) return -1;
#endif

#ifdef CONTROL_X
 fprintf(STDCTR,"(sr_rdver): Reading file \"%s\"\n",ver_file);
#endif

#ifdef EXIT_ON_ERROR
 sr_rdver_read_header(ver_stream, ver_file, ndim, &ndim, &m_par);
#else
 if (sr_rdver_read_header(ver_stream, ver_file, ndim, &ndim, &m_par) != 0)
 {
   fclose(ver_stream);
   return -1;
 }
#endif

 if (sr_rdver_read_vertices(ver_stream, ver_file, y, p, ndim, m_par) != 0)
 {
   fclose(ver_stream);
   return -1;
 }

#ifdef CONTROL_X
   fprintf(STDCTR,"\n");
#endif
/************************************************************************
  END OF INPUT
  Close input file.
*************************************************************************/
 fclose(ver_stream);

#ifdef CONTROL
 fprintf(STDCTR,"(sr_rdver): vertex read from \"%s\":\n", ver_file);
 for (int i_par = 1; i_par<= m_par; i_par++)
 {
   fprintf(STDCTR,"(%2d) %7.4f :", i_par, y[i_par]);
   for(int j_par=1; j_par<= ndim; j_par++)
     fprintf(STDCTR," %6.3f", p[i_par][j_par]);
   fprintf(STDCTR,"\n");
 }
 fflush(STDCTR);
#endif

 return(1);
}  /* end of function sr_rdver */
/************************************************************************/
