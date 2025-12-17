/*********************************************************************
GH/15.08.95

 file contains function:

  struct crivcur *cr_input( char *ctr_file, char *the_file)

 Read all inputs from theoretical and experimental data files

Changes:

 GH/25.09.92
 GH/26.10.92 - change control output
 GH/16.12.92 - allocaton of cur_list by calloc instead of malloc
 GH/11.08.95 - Creation (copy from rfinput.c)
 GH/15.08.95 - Include the_file in parameter list.
 WB/05.10.98 - cur_list[i_cur -1].group_id = I_END_OF_LIST;
 
*********************************************************************/

#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include "cleed_string.h"
#include "cleed_cstring.h"
#include "crfac.h"

/*
#define CONTROL
#define CONTROL_X
*/

#define ERROR

struct crivcur *cr_input( char *ctr_file, char *the_file)

/*

General input routine:

Read all inputs from theoretical and experimental data files according
to commands given in the control file (input, see command line).
The following symbols can be used to specify the files to be read
from:
	e0: energy of beam appearance.
	ef: experimental input file (format: <energy> <intensity>).
       (ei: spots used from current experimental input file.)
        ti: spots used from current theoretical input file.
	id: group ID
	wt: relative weight in R factor calculation.

A sample control file is shown below. Three experimental files
(Ru001cl.iv1-3) are compared with the corresponding beams from the
theoretical input file (rucl3.res) specified by their beam indices.

--- Begin of File ---
#  Clean Ru(001) surface
# (control file for R factor program RFAC)
#
#  ef=<experimental input file>
#  ti=<corresponding indices in theoretical input file>
#  id=<group ID>
#  : = separator
ef=/usr/leed/RU001/Ru001cl.iv1:ti=(-1.,1.)+(1.,-1.):id=11:wt=1.
ef=/usr/leed/RU001/Ru001cl.iv2:ti=(-1.,2.)         :id=12:wt=1.
ef=/usr/leed/RU001/Ru001cl.iv3:ti=(-2.,2.)+(2.,-2.):id=11:wt=1.
--- End of File ---

return value: pointer to a list of IV curves (struct crivcur *cur_list).
              This list is terminated by the value I_END_OF_LIST in 
	      cur_list[i+1].group_id.
*/

{

int i, j, len;
int n_cur, i_cur;
long offs;

struct crivcur *cur_list;   /* list of IV curves */
char *ctr_buffer;           /* buffer for control file */
char *the_buffer;           /* buffer for theoretical input file */
char line_buffer[STRSZ],    /* buffer for a single command line */
     exp_file[STRSZ],       /* name of experimental input file */ 
     index_list[STRSZ];     /* command line for averaging theoretical
			       indices */

/*********************************************************************
 Copy theoretical input file to the_buffer
 first free previously allocated memory.
*********************************************************************/

#ifdef CONTROL
 fprintf(STDCTR,"(cr_input): read theoretical data from \"%s\"\n", the_file);
#endif
 the_buffer = file2buffer(the_file);
 

/*********************************************************************
 Copy control file to ctr_buffer
 count lines without comments (i.e. number of IV curves to compare)
 allocate memory for IV curves
*********************************************************************/

#ifdef CONTROL
 fprintf(STDCTR,"(cr_input): read control data from \"%s\"\n", ctr_file);
#endif
 ctr_buffer = file2buffer(ctr_file);

 n_cur = rf_nclines(ctr_buffer) ;
#ifdef CONTROL_X
 fprintf(STDCTR,"(cr_input): n_cur = %d\n", n_cur);
#endif
 cur_list = (struct crivcur *) calloc(n_cur, sizeof(struct crivcur));

/*********************************************************************
 Scan through control file.
 read IV curves.
*********************************************************************/

 i_cur = 0;
 offs = 0;
 cur_list[i_cur].exp_list = NULL;
 cur_list[i_cur].the_list = NULL;

 while ((len = bgets(ctr_buffer, offs, STRSZ, line_buffer)) > -1)
 {
   /* initialise variables */
   exp_file[0] = '\0';
   cur_list[i_cur].eng_0 = 0.;
   cur_list[i_cur].group_id = DEFAULT_GROUP_ID;
   cur_list[i_cur].weight = 1.;

   if (line_buffer[0] != '#')         /* exclude comments */
   {
     for (i=0; i < (long)cleed_strnlen(line_buffer, STRSZ); i++)
     {

/* ef: experimental input file */
       if ( !strncmp("ef=", line_buffer+i, 3) )
       {
         i+=3;
         while(line_buffer[i] == ' ') i++;
         for(j=0; (line_buffer[i] != '\0') && /* note the ' ' removed */
                   (line_buffer[i] != ':'); j++, i++) 
         { exp_file[j] = line_buffer[i]; }
         exp_file[j] = '\0';
         
  /* 
     Read experimental IV curve from file and store to exp_list.  
  */
         cur_list[i_cur].exp_list = cr_rdexpt(cur_list+i_cur, exp_file); 
         
       }/* if "ef=" */

/* ti: index list (read theoretical data from file) */
       if ( !strncmp("ti=", line_buffer+i, 3) )
       {
         i+=3;
         for(j=0; (line_buffer[i] != '\0') && (line_buffer[i] != ':')
	      && (line_buffer[i] != ' '); j++, i++)
         { index_list[j] = line_buffer[i]; }
         index_list[j] = '\0';
  
         if (the_buffer == NULL)
         {
           #ifdef ERROR
           fprintf(STDERR,
                  "(cr_input): *** No theoretical input file present \"%s\"\n",
	          index_list);
           #endif
           exit(1);
         }
         else
         {
           cur_list[i_cur].the_list = 
                cr_rdcleed(
                        cur_list+i_cur, /* IV curve structure */
                        the_buffer,      /* theoretical input */
                        index_list);     /* control list for beam average */
    /* 
       If the energy of appearance is not specified, use first energy
       in the theoretical file.
    */
           if ( IS_EQUAL_REAL(cur_list[i_cur].eng_0, 0.)) 
           {  cur_list[i_cur].eng_0 = cur_list[i_cur].the_first_eng; }
         }
       }/* if "ti=" */

/* id: group ID */
       if ( !strncmp("id=", line_buffer+i, 3) )
       {
         i+=3;
         sscanf(line_buffer+i,"%d",&(cur_list[i_cur].group_id));
       }/* if "id=" */

/* wt: relative weight in R-factor average */
       if ( !strncmp("wt=", line_buffer+i, 3) )
       {
         i+=3;
#ifdef REAL_IS_DOUBLE
         sscanf(line_buffer+i,"%lf",&(cur_list[i_cur].weight));
#endif
#ifdef REAL_IS_FLOAT
         sscanf(line_buffer+i,"%f",&(cur_list[i_cur].weight));
#endif
       }/* if "id=" */

/* e0: energy of beam appearence */
       if ( !strncmp("e0=", line_buffer+i, 3) )
       {
         i+=3;
#ifdef REAL_IS_DOUBLE
         sscanf(line_buffer+i,"%lf",&(cur_list[i_cur].eng_0));
#endif
#ifdef REAL_IS_FLOAT
         sscanf(line_buffer+i,"%f",&(cur_list[i_cur].eng_0));
#endif
       }/* if "e0=" */


     } /* for i: scan through line_buffer */

/*
  END OF LINE:
  increment i_cur and reset lists;
*/
#ifdef CONTROL
     fprintf(STDCTR,
     "(cr_input) %d: ti: %s, ef: %s, e0: %.1f id: %d, wt: %.1f, \n", 
      i_cur, index_list, exp_file,
      cur_list[i_cur].eng_0, cur_list[i_cur].group_id,
      cur_list[i_cur].weight);
#endif
     i_cur ++;
     cur_list[i_cur].exp_list = NULL;
     cur_list[i_cur].the_list = NULL;

   } /* if != '#' (no comment) */

/* increment offset in ctr_buffer */
   offs += (long)len;
 }  /* while: scan through control file */


 cur_list[i_cur -1].group_id = I_END_OF_LIST;

/*
 free previously allocated memory.
 return.
*/
 free(the_buffer);
 free(ctr_buffer);
 return(cur_list);
}   /* end of function */
