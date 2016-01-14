/************************************************************************
 *                        CAOI_LEED_BSR.C
 *
 *  Copyright 2008-2015 Zhasmina Zheleeva
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 *
 * Implementation file for function bsrinp() which makes \c sa
 * bsr files for each angle of incidence.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "leed.h"
#include "leed_aoi.h"

#if _MSC_VER
#define strncasecmp(x,y,z) _strnicmp((x),(y),(z)) 
#else                
#include <strings.h>
#endif

/*!
 * Returns the number of angle inputs as given by 'sa:' command string in
 * the CLEED input file.
 *
 * \param filebsr pointer to filename string to be read.
 *
 * \return number of search angles given by \c 'sa:' input file of \p filebsr
 *
 * \retval 0 if no \c 'sa:' input found or input was unable to be parsed
 * successfully.
 *
 * \warning if \p filebsr is too long or the file cannot be opened then the
 * program will exit with the corresponding standard error code.
 */
size_t leed_get_number_of_angles(const char *filebsr)
{
  size_t length = 0;
  size_t sa = 0;
  char *ext = NULL;
  char linebuffer[STRSZ] = "";
  char fname_string[FILENAME_MAX] = "";
  char project_name[FILENAME_MAX] = "";
  FILE *fp = NULL;

  /* Read sa value from the .bul file */
  if (strlen(filebsr) > 3)
  {
    /* assume project name is control filename minus file extension */
    ext = strrchr(filebsr, '.');
    if (ext)
      length = ext - filebsr;
    else
      length = strlen(filebsr); /* use full path name */
  }

  strncpy(project_name, filebsr,
          (length < sizeof(project_name)-1) ? length : sizeof(project_name)-1);
  project_name[sizeof(project_name)-1] = '\0';

  strncpy(fname_string, filebsr,
          (length < sizeof(fname_string)-1) ? length : sizeof(fname_string)-1);
  fname_string[FILENAME_MAX - 1] = '\0';

  if (strlen(fname_string) + 4 >= sizeof(fname_string)-1 ||
      strlen(fname_string) + 4 > FILENAME_MAX ||
      length > sizeof(fname_string)-1)
  {
    ERROR_MSG("string length too long\n");
    exit(ENAMETOOLONG);
  }
  sprintf(fname_string+length, ".bsr");

  if ((fp = fopen(fname_string, "r")) == NULL)
  {
    ERROR_MSG("");
    perror(fname_string);
    exit(EIO);
  }

  while (fgets(linebuffer, STRSZ, fp) != NULL)
  {
    if (!strncasecmp(linebuffer, "sa:", 3))
    {
      if (sscanf(linebuffer+3, " %u", &sa) < 1)
      {
        WARNING_MSG("could not read value from '%s'(sa=%u)\n", linebuffer, sa);
      }
    }
  }  /*while*/

  fclose(fp);

  return sa;

}

/*!
 * Creates an aoi version of the input filename for a given angle.
 *
 * \param aoi_filename New filename for given angle of incidence.
 * Memory will be allocated if \c NULL is passed into the function.
 * \param[in] src_filename Original filename
 * \param[in] i_ang Angles of incidence to append to \p src_filename
 *
 * \warning \p src_filename must not be \c NULL otherwise no
 * changes will be made to \p aoi_filename
 */
void leed_create_aoi_filename(char *aoi_filename,
    const char *src_filename, size_t i_ang)
{
  if (src_filename == NULL) return;
  char *ext = src_filename ? strrchr(src_filename, '.') : NULL;
  size_t length = ext ? (ext - src_filename) : strlen(src_filename);
  if (aoi_filename == NULL) {
    if ((aoi_filename = (char*)calloc(length + 1, sizeof(char))) == NULL)
    {
      ERROR_MSG("could not allocate memory for new filename\n");
      exit(ENOMEM);
    }
  }
  strncpy(aoi_filename, src_filename, length);
  aoi_filename[length] = '\0';
  strcat(aoi_filename, leed_aoi_extension);
  if (ext)
    strcat(aoi_filename, ext);
}

/*!
 * Merges multiple angle of incidence result files
 */
int leed_merge_result_files(const char *par_file, size_t sa) {
  size_t i_ang;
  size_t length = (par_file != NULL) ? strlen(par_file) : 0;
  FILE *write_stream = NULL;
  FILE *read_stream = NULL;
  char res_filename[FILENAME_MAX] = "";
  char tmp_filename[FILENAME_MAX] = "";
  char *ext = (par_file != NULL) ? strrchr(par_file, '.') : NULL;
  char linebuffer[STRSZ] = "";

  if (par_file == NULL)
  {
    ERROR_MSG("parameter input file cannot be NULL\n");
    exit(EINVAL);
  }

  if (ext)
    length = ext - par_file;

  if (length >= FILENAME_MAX)
  {
    WARNING_MSG("parameter filename '%s' is too long - "
                "truncated to %i characters", par_file, FILENAME_MAX);
    length = FILENAME_MAX - strlen(".res") - 1;
  }

  /* Make proj_name.res = proj_nameia_1.res + proj_nameia_2.res + ... */
  strncpy(res_filename, par_file, length);
  res_filename[length] = '\0';
  sprintf(res_filename + length, ".res");

  if ((write_stream = fopen(res_filename, "w")) == NULL)
  {
    ERROR_MSG("");
    perror(res_filename);
    exit(EIO);
  }

  for (i_ang = 0; i_ang < sa; i_ang ++)
  {
    strncpy(tmp_filename, par_file, length);
    tmp_filename[length] = '\0';
    sprintf(tmp_filename + length, "ia_%u.res", i_ang + 1);
    if ((read_stream = fopen(tmp_filename, "r")) == NULL)
    {
      ERROR_MSG("could not open output file \"%s\"\n", tmp_filename);
      exit(EIO);
    }

    fprintf(write_stream, "Copy of %s\n", tmp_filename);

    while (fgets(linebuffer, STRSZ, read_stream))
    {
      fprintf(write_stream, "%s", linebuffer);
    }

    fclose(read_stream);

  } /* for i_ang */

  fclose(write_stream);
  fflush(STDERR);

  return 0;
}

leed_args *leed_args_init_aoi(leed_args *src_args, size_t i_ang) {
  leed_args *aoi_args = leed_args_init();

  if (aoi_args) {
    leed_create_aoi_filename(aoi_args->bul_file, src_args->bul_file, i_ang);
    leed_create_aoi_filename(aoi_args->par_file, src_args->par_file, i_ang);
    leed_create_aoi_filename(aoi_args->pro_name, src_args->pro_name, i_ang);
    leed_create_aoi_filename(aoi_args->res_file, src_args->res_file, i_ang);
    aoi_args->ctr_flag = src_args->ctr_flag;
    aoi_args->sa = src_args->sa;
  }
  return aoi_args;
}

/*!
 * Makes bsr files for each angle of incidence.
 *
 * \param filebsr Root filename to which \p i_ang & .bsr are appended.
 *
 * \warning exits on file read/open failure.
 *
 * \return 0 on success, non-zero on minor failure.
 *
 */
int leed_bsrinp(const char *filebsr, size_t number)
{
  size_t i_ang;
  size_t num;
  size_t length = (filebsr == NULL) ? 0 : strlen(filebsr);
  int err = 0;

  float ip, it;

  char linebuffer[STRSZ] = "";
  char bsr_filename[FILENAME_MAX] = "";
  char *ext = NULL;

  FILE *readstream = NULL;
  FILE *writestream = NULL;

  CONTROL_MSG(CONTROL, "sa: %u\n", number);
  fprintf(STDERR, "%s\n", filebsr);

  /* Open .bsr file for reading */
  if ((readstream = fopen (filebsr, "r")) == NULL)
  {
    ERROR_MSG("");
    perror(filebsr);
    exit(EIO);
  }

  /* Make sa ia_i.bsr files for each angle of incidence */
  if (strlen(filebsr) && (ext = strrchr(filebsr, '.')) != NULL)
  {
    length = ext - filebsr;
  }
  fprintf(STDERR, "length = %d\n", length);

  for (i_ang = 0; i_ang < number; i_ang ++)
  {
    rewind(readstream);
    strncpy(bsr_filename, filebsr, length);
    bsr_filename[FILENAME_MAX - 1] = '\0';
    fprintf(STDERR, "i_ang: %u\n%s\n", i_ang, bsr_filename);
    sprintf(linebuffer + length, "%s%u.bsr", leed_aoi_extension, i_ang + 1);
    fprintf(STDERR, "%s\n", bsr_filename);

    if ((writestream = fopen(bsr_filename, "w")) == NULL)
    {
      ERROR_MSG("");
      perror(bsr_filename);
      exit(EIO);
    }

    while (fgets(linebuffer, STRSZ, readstream))
    {
      if (!strncasecmp(linebuffer, "ipt:", 4))
      {
        if (sscanf(linebuffer + 4, " %u", &num) < 1)
          ERROR_MSG("no value given for 'ipt:'\n");
 
        if (num == (i_ang + 1))
        {
          if (err = sscanf(linebuffer + 4, " %u %f %f", &num, &ip, &it) < 3) 
          {
            ERROR_MSG("could not read all values from line '%s' (%i read)\n", 
              linebuffer, err);
          }
          fprintf(writestream, "ip: %.2f\nit: %.2f\n", ip, it);
        }
      }
      else
      {
        fprintf(writestream, "%s", linebuffer);
      }

    } /*while helpstring*/

    fclose(writestream);

  } /*for i_ang*/

 fclose(readstream);

 return 0;
} /*bsrinp*/
