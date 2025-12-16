/*********************************************************************
ZZ/03.03.08
  file contains functions:

  bsrinp (03.03.08)
      Makes sa ia_i.bsr files for each angle of incidence

Changes:

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include "caoi_leed.h"


void bsrinp (char *filebsr, int number)
{
  int i_ang;
  int length;
  int num;

 float ip, it;

 char linebuffer[STRSIZE];
 char helpstring[STRSIZE];

 FILE *readstream;
 FILE *writestream;

 printf("*** sa: %d ***\n", number);
 fprintf(STDERR,
    "***%s***\n", filebsr);
/***********************************************************************
 Open .bsr file for reading
***********************************************************************/


 if ((readstream = fopen (filebsr, "r")) == NULL)
   {
    #ifdef ERROR
    fprintf(STDERR,
    "*** error (caoi_leed_bsr): can not open file %s\n", filebsr);
    #endif
    exit(1);
   }


/***********************************************************************
 Makes sa ia_i.bsr files for each angle of incidence
***********************************************************************/

 length = strlen(filebsr) - 4;
 fprintf(STDERR,
    "***length = %d***\n", length);


  for (i_ang = 0; i_ang < number; i_ang ++)
  {
    fprintf(STDERR,
        "*** i_ang: %d ***\n", i_ang);
    rewind(readstream);
    strncpy(linebuffer, filebsr, length);
    fprintf(STDERR, "***%s***\n", linebuffer);
    sprintf(linebuffer + length, "ia_%d.bsr", i_ang + 1);
    fprintf(STDERR, "***%s***\n", linebuffer);

    if ((writestream = fopen(linebuffer, "w")) == NULL)
    {
        #ifdef ERROR
        fprintf(STDERR,
            "*** error (caoi_leed_bsr): can not open file %s\n", linebuffer);
        #endif
        exit(1);
    }

    while (fgets(helpstring, STRSIZE, readstream))
    {
      if (!strncasecmp(helpstring, "ipt:", 4))
      {
        sscanf(helpstring + 4, " %d", &num);
 
        if (num == (i_ang +1))
        {
            sscanf(helpstring +4, " %d %f %f", &num, &ip, &it);
            fprintf(writestream, "ip: %.2f\nit: %.2f\n", ip, it);
        }
      }

      else fprintf(writestream, "%s", helpstring);

    } /*while helpstring*/

    fclose(writestream);

  } /*for i_ang*/

 fclose(readstream);

} /*bsrinp*/
