
/**************************************************************************<
    >
    >         File Name: refinp.c
    >
>**************************************************************************/

#include "mkiv.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char line_buffer[STRSZ];
 
int ref_inp(int *naux, struct spot *aux, int verb)

/***************************************************************************
  SU/ 17.04.91
  GH/ 05.08.92
  CS/11.8.93    

 Purpose:
  ref_inp reads the indices and positions of (at least 3) reflexes from
  the standard input

 Input:
  int verb - defines amount of output.

 Output:
  int *naux - number of spots
  struct spot *aux - indices and positions of spots

***************************************************************************/

{
    int i;
    FILE *in_out;

/**************************************************************************/

    fprintf(stdout,"Enter number of spots for determination of basis (max. 5):\n");
    fprintf(stdout,"\t<num>:\tinput through terminal\n");
    fprintf(stdout,"\tr:\tformer input will be read \n\t\tfrom file 'mkiv.pos'\n");
    fgets(line_buffer, STRSZ, stdin);

    if ( verb&QUICK || ( !(verb&QUICK) && *line_buffer=='r' ) ) {
        if ((in_out = fopen("mkiv.pos","r")) == NULL) {
            ERR_EXIT(mkiv.pos: fopen for reading failed!)
        }
        else
            fprintf(stdout, "Reading reference spot positions from 'mkiv.pos' \n");

        fscanf(in_out, "%d\n", naux);
        QQ printf("\t%3d reflexes:\n",*naux);
        for (i=0; i<*naux; i++) {
            fscanf(in_out, "%f %f\n",&(aux[i].lind1),&(aux[i].lind2) );
            fscanf(in_out, "%f %f\n",&(aux[i].xx),&(aux[i].yy) );
            QQ printf("\t(%4.2f,%4.2f) at\t(%3.0f,%3.0f)\n",
                     aux[i].lind1,aux[i].lind2, aux[i].xx,aux[i].yy );
        }
    }
    else {
        sscanf(line_buffer,"%d", naux);
        if ((in_out = fopen("mkiv.pos","w")) == NULL)
            ERR_EXIT("mkiv.pos: fopen for writing failed!");
        fprintf(in_out, "%d\n", *naux);
        for (i=0; i<*naux; i++) {
            fprintf(stdout," Enter indices for one reflex:\n");
            fscanf(stdin,"%f%f",&(aux[i].lind1),&(aux[i].lind2) );
            fprintf(in_out, "%f %f\n",aux[i].lind1,aux[i].lind2 );
            fprintf(stdout," Enter horizontal and vertical comp. of reflex position;\n");
            scanf("%f%f",&(aux[i].xx),&(aux[i].yy) );
            fprintf(in_out, "%f %f\n",aux[i].xx,aux[i].yy );
        }
    }
    fclose(in_out);
    
    return 0;
}
/***************************************************************************/
