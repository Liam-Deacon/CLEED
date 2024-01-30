/*
 * File:   debye.c
 * Author: Liam Deacon
 *
 * File contains: Method for calculating dr1 from Debye Temperature
 *
 * Created on 10 February 2012, 11:10
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* constants */
#define H_BAR 1.05457148e-34
#define K_BOLTKZMANN 1.3806503e-23
#define H_BAR_SQ 1.1121210064293904e-68
#define KILOGRAMS_PER_AMU 1.66053886e-27
#define N_AVOGADRO 6.0221415e23 /* 6^23 atoms per mol */
#define KILOGRAMS_PER_ATOMIC_MASS 1/(N_AVOGADRO/1000)
/* strictly "(atomic mass)/(Avogadros number)" gives g, so divide be 1000 to get kg */

void usage(FILE *output, char *prog)
{
    fprintf(output, "Usage: %s \n\nOPTIONS:-\n", prog);
    fprintf(output, "-T <expt_temperature>\n");
    fprintf(output, "\tspecify the absolute temperature of the experiment in Kelvin.\n");
    fprintf(output, "\tnote: default is 293K if no value given\n");
    fprintf(output, "-D <Debye Temperature (K)>\n");
    fprintf(output, "-M <mass> [optional: --kg]\n");
    fprintf(output, "\t<mass> is the mass in amu(Da) %s",
                    "or kilograms(kg) if --kg flag used\n");
    fprintf(output, "-o <output_file>\n");
    fprintf(output, "-h --help\n\tprint this help\n");
    fprintf(output, "-v --verbose\n");
    fprintf(output, "\tprint verbose output");

}

int main(int argc, char *argv[]) {
  /* Set variables */
  int i;
  int kilo_flag, verbose_flag;
  int atomic_weight;

  double temp, amu, mass, debye_temp;
  long double displacement, rms_displacement, dr1;

  char temp_str[1024] = "", debye_str[1024] = "", amu_str[1024] = "", out_str[1024] = "";

  /* initialise variables */
  temp = amu = mass = debye_temp = displacement = rms_displacement = dr1 = 0.0;
  kilo_flag = atomic_weight = verbose_flag = 0;

  for (i = 1; i <= argc; i++) {

    if (i + 1 != argc + 1) {

    if (strcmp(argv[i],"-T") == 0) {
      strcpy(temp_str, argv[i+1]);
        temp = atof(temp_str);
        i++;
    }

    if (strcmp(argv[i],"-D") == 0) {
      strcpy(debye_str,argv[i+1]);
        debye_temp = atof(debye_str);
        i++;
    }

    if (strcmp(argv[i],"-M") == 0) {
      strcpy(amu_str, argv[i+1]);
        amu = atof(amu_str);
        i++;
    }

    if (strcmp(argv[i],"--kg") == 0) {
        kilo_flag = 1;
        i++;
    }

    if (strcmp(argv[i],"-o") == 0) {
      strcpy(out_str, argv[i+1]);
        i++;
    }

    if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0) {
      usage(stdout, argv[0]);
        return 0;
    }

      if (strcmp(argv[i],"-v") == 0 || strcmp(argv[i],"--verbose") == 0) {
        verbose_flag = 1;
      }
    }
  }

  if (debye_temp > 0.0 && (mass > 0.0 || amu > 0.0))
    temp = 293.0;

  if (verbose_flag) {
    printf("Debye Temperature Calculator\n");
    printf("============================\n");
  }

  /* Do checks & get input if needed
     Incorrect input will return 0.0 from atof function */
  while (amu <= 0.0) {
    if ( !kilo_flag ) {
        printf("Input mass of atom in atomic mass units: ");
    }
    else if ( atomic_weight && !kilo_flag ) {
        printf("Input atomic weight of atom: ");
    }
    else {
        printf("Input mass of atom in kilograms: ");
    }
    fgets(amu_str, sizeof(amu_str), stdin);
    amu = atof(amu_str);
  }

  if (!kilo_flag) {
    mass = amu*KILOGRAMS_PER_AMU;
  }
  else {
    mass = amu;
  }

  /* get experimental temperature */
  while (temp <= 0.0) {
    printf("Input experimental temperature (K): ");
    fgets(temp_str, sizeof(temp_str), stdin);
    temp = atof(temp_str);
  }

  /* get Debye temperature */
  while (debye_temp <= 0.0) {
    printf("Input Debye Temperature (K): ");
    fgets(debye_str, sizeof(debye_str), stdin);
    debye_temp=atof(debye_str);
  }

  /* Calculate mean square displacement  of atomic vibrations */
  displacement = (H_BAR/K_BOLTKZMANN)*H_BAR;

  /* Note this is done in 2 parts to reduce rounding errors due to sig. figs. */
  displacement=((3*temp)/(mass*(debye_temp*debye_temp)))*displacement;

  /* Calculate final rms displacement */
  rms_displacement = sqrt(displacement);
  dr1 = rms_displacement*1e+10;

  /* write output */
  if (strcmp(out_str, "") != 0 ) {
    FILE *fo = fopen(out_str, "w");
    if (fo == NULL) {
      fprintf(stderr, "Cannot open output file: '%s'\n", out_str);
      exit(1);
    }

    /* write output to file */
    fprintf(fo, "Mass (amu) = %f\n", amu);
    fprintf(fo, "Expt._Temp. (K): %f\n", (double)temp);
    fprintf(fo, "Debye_Temp. (K): %f\n", (double)debye_temp);
    fprintf(fo, "<u>^2 (sq. m): %f\n", (double)displacement);
    fprintf(fo, "dr1 (1e-10 m): %f\n", (double)dr1);

    fclose(fo);
  }

  if (verbose_flag) {
    printf("Mass: %f kg\n", (double)mass);
    printf("Expt._Temp.: %f K\n", (double)temp);
    printf("Debye_Temp.: %f K\n", (double)debye_temp);
    printf("Mean square displacement: %f m^2\n", (double)displacement);
    printf("dr1: %f Angstroms\n", (double)dr1);
  }
  else {
    printf("%f\n", (double)dr1);
  }
  return 0;
}
