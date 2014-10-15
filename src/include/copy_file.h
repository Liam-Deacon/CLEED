/************************************************************************
 *                         COPY_FILE.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description: Header provides function prototype for copy_file()
 *
 * Changes:
 *   LD/2014.05.14 - Creation
 ************************************************************************/

#ifndef COPY_FILE_H
#define COPY_FILE_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif


int copy_file(char *old_filename, char  *new_filename);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* COPY_FILE_H */

