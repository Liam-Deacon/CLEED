
/***************************************************************************
    
             File Name: readinp.c
    
***************************************************************************/

/**************************************************************************/
#include <ctype.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "mkiv.h"
#include "inputs.h"
/**************************************************************************/

int readinp(int verb, char *inp_path, char *param_path, char *ref_name, 
            char *mask_name, int *nstart, int *nstop, float *e_step, 
            struct coord *cen, float *router, float *rinner, int *bg, 
            int *smooth, float *kpl_10, int *ndom, struct domain superlat[], 
            int *ndesi, struct lindex desi[], int *nref, struct lindex ref[], 
            int *nexcl, struct lindex excl[] )

/****************************************************************************
  CS/7.9.93

 Purpose:
  readinp reads some necessary data statements from the file mkiv.inp
  if variables don't occur in the input file, defaults are taken instead

****************************************************************************/

{
    int i, nndom = 0;
    float fac, interim;
    char buf[STRSZ], var[STRSZ];
    FILE *fopen(), *fp;

/***************************************************************************/

    /* Preset variables */

    *nstart	= 40;
    *nstop = 400;
    *e_step	= 1;
    cen->xx	= 250;
    cen->yy	= 250;
    *router	= 230;
    *rinner	= 0;
    *bg = 2;
    *smooth = -1;
    *kpl_10 = 2.6771;
    *ndom = 0;

    /* Open statement */
    fp = fopen(inp_path,"r");
    if (fp == NULL) 
    {
        fprintf(stderr, "***error (readinp): failed to open '%s'\n", 
                inp_path);
        exit(1);
    }
    else  
        printf("\nreading input from \"%s\":\n", inp_path);
     
    /* Read data */

    while ( fgets(buf,STRSZ,fp)!=NULL && *buf!='.' ) {
        if ( ispunct(*buf) ) continue;
        sscanf(buf,"%s", var);

        for ( i=0; i<INP_MAX; i++)
            if ( ! strncasecmp( var, INP[i], INP_LEN ) ) break;

        switch (i+1) 
        {
            case  1 :  sscanf( buf, "%s %s", var, ref_name); break;
            case  2 :  sscanf( buf, "%s %s", var, mask_name); break;
            case  3 :  sscanf( buf, "%s %d", var, nstart); break;
            case  4 :  sscanf( buf, "%s %d", var, nstop); break;
            case  5 :  sscanf( buf, "%s %f", var, e_step); break;
            case  6 :  sscanf( buf, "%s %d", var, &(cen->xx)); break;
            case  7 :  sscanf( buf, "%s %d", var, &(cen->yy)); break;
            case  8 :  sscanf( buf, "%s %f", var, router); break;
            case  9 :  sscanf( buf, "%s %f", var, rinner); break;
            case 10 :  sscanf( buf, "%s %d", var, bg); break;
            case 11 :  sscanf( buf, "%s %d", var, smooth); break;
            case 12 :  sscanf( buf, "%s %f", var, kpl_10); break;
            case 13 :  sscanf( buf, "%s %d", var, ndom); break;
            case 14 :  getdomain( buf, &superlat[nndom++]);	break;
            case 15 :  getindices( fp, buf, ndesi, desi, nref, 
                                   ref, nexcl, excl); break;
            default :  buf[strlen(buf)-1] = '\0';
                       fprintf(stderr, "***error (readinp):"
                            " wrong arguments in line '%s' \n", buf); 
        }
    }
    fclose(fp);

    fp = fopen(param_path, "w");
    if (fp == NULL) 
        fprintf(stderr, "***error (readvar): "
                "unable to write to '%s'\n", param_path);
    else {
        VPRINT("writing parameter input-values to '%s'\n\n", param_path);
        fprintf(fp,"ref_name = %s \n", ref_name);
        fprintf(fp,"mask_name = %s \n", mask_name);
        fprintf(fp,"nstart = %d \n", *nstart);
        fprintf(fp,"nstop = %d \n", *nstop);
        fprintf(fp,"n_step = %f \n", *e_step);
        fprintf(fp,"center = (%d,%d) \n", cen->xx, cen->yy);
        fprintf(fp,"router = %.0f \n",*router);
        fprintf(fp,"rinner = %.0f \n", *rinner);
        fprintf(fp,"bg = %s \n", ((*var&&(*bg==2))?"YES":"NO"));
        fprintf(fp,"smoothcurrent = %s \n", 
                    ((*var&&(*smooth==-1))?"YES":"NO"));
        fprintf(fp,"kp_len_10 = %f \n", *kpl_10);
        fprintf(fp,"ndom = %d \n", *ndom);
        for ( i=0; i<*ndom; i++)  
            fprintf(fp,"%f %f %f %f \n", 
                    superlat[i].lind11, superlat[i].lind12,
                    superlat[i].lind21, superlat[i].lind22);
        fprintf(fp,"ndesi = %d \n", *ndesi);
        for ( i=0; i<*ndesi && i<9; i++)
            fprintf(fp,"%f %f \n", desi[i].lind1, desi[i].lind2);
        if ( i<*ndesi ) fprintf(fp,"\t.\n\t.\n\t.\n\n");
        fclose(fp);
    }

    printf("Total of %d spots.\n",*ndesi);
    if (*nref > 2)  printf("%d spots used for recalibration;\n", *nref);
    else            printf("Recalibration with most intense spots;\n");
    printf(" No maximum search for %d spots.\n", *nexcl);

    /* inversion of superlattice matrices */

    for (i=0; i<*ndom; i++) {
        fac = superlat[i].lind11 * superlat[i].lind22 -
              superlat[i].lind12 * superlat[i].lind21;
        interim = superlat[i].lind11;
        superlat[i].lind11 =  superlat[i].lind22 / fac;
        superlat[i].lind22 =  interim / fac;
        interim = superlat[i].lind12;
        superlat[i].lind12 = -superlat[i].lind21 / fac;
        superlat[i].lind21 = -interim / fac;
    }
    
    return 0;
}


/***************************************************************************/
int getdomain (char *buffer, struct domain *superlat)
/***************************************************************************/
{
    char var[STRSZ];

    sscanf( buffer, "%s %f %f %f %f", var,
          &superlat->lind11,
          &superlat->lind12,
          &superlat->lind21,
          &superlat->lind22 );
          
   return 0;
}

/***************************************************************************/
int getindices(FILE *fp, char *buffer, int *ndesi, struct lindex desi[], 
               int *nref, struct lindex ref[], int *nexcl, 
               struct lindex excl[])
/***************************************************************************/
{
    char var[STRSZ];
    int j;

    sscanf( buffer, "%s %d", var, ndesi);
    for ( j=0; fgets(buffer,STRSZ,fp)!=NULL && j<*ndesi; j++) {
        if ( buffer[0] == 'r' ) {
            sscanf( ++buffer, "%f %f", &ref[*nref].lind1, &ref[*nref].lind2);
            ++*nref;
        }
        if ( buffer[0] == 'i' ) {
            sscanf( ++buffer, "%f %f", &excl[*nexcl].lind1, &excl[*nexcl].lind2);
            ++*nexcl;
        }
        sscanf( buffer, "%f %f", &desi[j].lind1, &desi[j].lind2);
    }
    
    return 0;
}
/***************************************************************************/
