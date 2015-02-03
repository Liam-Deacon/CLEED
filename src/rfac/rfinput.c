/*********************************************************************
 *                        RFINPUT.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.10.26 - change control output
 *   GH/1992.12.16 - allocaton of cur_list by calloc instead of malloc
 *   GH/1995.08.11 - Creation (copy from rfinput.c)
 *   GH/1995.08.15 - Include the_file in parameter list.
 *   WB/1998.10.05 - cur_list[i_cur -1].group_id = I_END_OF_LIST;
 *********************************************************************/

/*! \file
 *  \brief Implementation file for rfac_ivcur_read() function.
 */

#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include "rfac.h"

/*!
 * General input routine:
 *
 * Read all inputs from theoretical and experimental data files according
 * to commands given in the control file (input, see command line).
 * The following symbols can be used to specify the files to be read
 * from:
 * e0: energy of beam appearance.
 * ef: experimental input file (format: <energy> <intensity>).
 *      (ei: spots used from current experimental input file.)
 *       ti: spots used from current theoretical input file.
 * id: group ID
 * wt: relative weight in R factor calculation.
 *
 * A sample control file is shown below. Three experimental files
 * (Ru001cl.iv1-3) are compared with the corresponding beams from the
 * theoretical input file (rucl3.res) specified by their beam indices.
 *
 * --- Begin of File ---
 * #  Clean Ru(001) surface
 * # (control file for R factor program RFAC)
 * #
 * #  ef=<experimental input file>
 * #  ti=<corresponding indices in theoretical input file>
 * #  id=<group ID>
 * #  : = separator
 * ef=/usr/leed/RU001/Ru001cl.iv1:ti=(-1.,1.)+(1.,-1.):id=11:wt=1.
 * ef=/usr/leed/RU001/Ru001cl.iv2:ti=(-1.,2.)         :id=12:wt=1.
 * ef=/usr/leed/RU001/Ru001cl.iv3:ti=(-2.,2.)+(2.,-2.):id=11:wt=1.
 * --- End of File ---
 *
 * \return pointer to a list of IV curves #rfac_ivcur \p *cur_list .
 * This list is terminated by the value #I_END_OF_LIST in
 * cur_list[i+1].group_id.
*/
rfac_ivcur *rfac_ivcur_read(const char *control_file, const char *theory_file)
{

  int i, j, len;
  size_t n_cur, i_cur = 0;
  long offs = 0L;

  rfac_ivcur *cur_list;       /* list of IV curves */
  char *ctr_buffer;           /* buffer for control file */
  char *the_buffer;           /* buffer for theoretical input file */
  char line_buffer[STRSZ];    /* buffer for a single command line */
  char exp_file[FILENAME_MAX];/* name of experimental input file */
  char index_list[STRSZ];     /* command line for averaging theoretical
			                         * indices */
  char fmt_buffer[STRSZ];     /* buffer for format specifier */

/*********************************************************************
 * Copy theoretical input file to the_buffer
 * first free previously allocated memory.
 *********************************************************************/

  CONTROL_MSG(CONTROL, "read theoretical data from \"%s\"\n", theory_file);

  the_buffer = file2buffer(theory_file);

/*********************************************************************
 * Copy control file to ctr_buffer
 * count lines without comments (i.e. number of IV curves to compare)
 * allocate memory for IV curves
 *********************************************************************/

  CONTROL_MSG(CONTROL, "read control data from \"%s\"\n", control_file);

  ctr_buffer = file2buffer(control_file);

  n_cur = rfac_nclines(ctr_buffer) ;

  CONTROL_MSG(CONTROL_X, "n_cur = %d\n", n_cur);

  cur_list = (rfac_ivcur*) calloc(n_cur, sizeof(rfac_ivcur));

  /* set real format specifier */
  sprintf(fmt_buffer, "%%%sf", CLEED_REAL_FMT);

/*********************************************************************
 * Scan through control file.
 * read IV curves.
 *********************************************************************/

  cur_list[i_cur].experimental->data = NULL;
  cur_list[i_cur].theory->data = NULL;

  while ((len = bgets(ctr_buffer, offs, STRSZ, line_buffer)) > -1)
  {
    /* initialise variables */
    exp_file[0] = '\0';
    cur_list[i_cur].eng_0 = 0.;
    cur_list[i_cur].group_id = DEFAULT_GROUP_ID;
    cur_list[i_cur].weight = 1.;

    if (line_buffer[0] != '#')         /* exclude comments */
    {
      for (i=0; i < (long)strlen(line_buffer); i++)
      {

        /* ef: experimental input file */
        if ( !strncmp("ef=", line_buffer+i, 3) )
        {
          i+=3;
          while(line_buffer[i] == ' ') i++;
          for(j=0; (line_buffer[i] != '\0') && /* note the ' ' removed */
                   (line_buffer[i] != ':'); j++, i++) 
          {
            exp_file[j] = line_buffer[i];
          }
          exp_file[j] = '\0';
         
          /* Read experimental IV curve from file and store to exp_list. */
          cur_list[i_cur].experimental->data = rfac_iv_read(exp_file);
         
        } /* if "ef=" */

        /* ti: index list (read theoretical data from file) */
        if ( !strncmp("ti=", line_buffer+i, 3) )
        {
          i+=3;
          for(j=0; (line_buffer[i] != '\0') && (line_buffer[i] != ':') &&
                   (line_buffer[i] != ' '); j++, i++)
         {
           index_list[j] = line_buffer[i];
         }
         index_list[j] = '\0';
  
         if (the_buffer == NULL)
         {
           ERROR_MSG("No theoretical input file present \"%s\"\n", index_list);
           exit(1);
         }
         else
         {
           cur_list[i_cur].theory->data = rfac_iv_data_read_cleed(
                        cur_list+i_cur, /* IV curve structure */
                        the_buffer,      /* theoretical input */
                        index_list);     /* control list for beam average */
           /*
            * If the energy of appearance is not specified, use first energy
            * in the theoretical file.
            */
           if ( IS_EQUAL_REAL(cur_list[i_cur].eng_0, 0.)) 
           {
             cur_list[i_cur].eng_0 = cur_list[i_cur].theory->first_eng;
           }

         } /* else */

       }/* if "ti=" */

       /* id: group ID */
       if ( !strncmp("id=", line_buffer+i, 3) )
       {
         i+=3;
         sscanf(line_buffer+i, "%d", &(cur_list[i_cur].group_id));
       } /* if "id=" */

       /* wt: relative weight in R-factor average */
       if ( !strncmp("wt=", line_buffer+i, 3) )
       {
         i+=3;
         sscanf(line_buffer+i, fmt_buffer, &(cur_list[i_cur].weight));
       } /* if "id=" */


       /* e0: energy of beam appearence */
       if ( !strncmp("e0=", line_buffer+i, 3) )
       {
         i+=3;
         sscanf(line_buffer+i, fmt_buffer, &(cur_list[i_cur].eng_0));
       } /* if "e0=" */


     } /* for i: scan through line_buffer */


     /*
      * END OF LINE:
      *   increment i_cur and reset lists;
      */

     CONTROL_MSG(CONTROL,
            "%d: ti: %s, ef: %s, e0: %.1f id: %d, wt: %.1f, \n",
            i_cur, index_list, exp_file,
            cur_list[i_cur].eng_0, cur_list[i_cur].group_id,
            cur_list[i_cur].weight);
     i_cur++;
     free(cur_list[i_cur].experimental);
     free(cur_list[i_cur].theory);

   } /* if != '#' (no comment) */

   /* increment offset in ctr_buffer */
   offs += (long)len;

  } /* while: scan through control file */

  cur_list[i_cur-1].group_id = END_OF_GROUP_ID;

  /* free previously allocated memory and return.*/
  free(the_buffer);
  free(ctr_buffer);

  return(cur_list);

}   /* end of function */
