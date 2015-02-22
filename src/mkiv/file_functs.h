/*********************************************************************
 *                        FILE_FUNCTS.C
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/02.03.14 - creation
 *********************************************************************/

/* return timestamp in format YYYY-mm-dd_HHMM */
const char *timestamp();

/* test whether file exists */
int file_exists(const char *filename);

/* copy contents of src file into dst file */
int file_copy(const char *src_path, const char *dst_path);

/* convenience function for backing up files */
int file_backup(const char *filepath);

/* move contents of src file into dst file */
int file_move(const char *src_path, const char *dst_path);

/* cat replacement */
char *file_content(const char *filename);

/* remove extension */
const char *remove_ext(const char *str, char dot, char sep);
