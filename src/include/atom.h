/************************************************************************
 *                         ATOM.H
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:   file contains struct atom - an atom structure
 *                containing element name and x,y,z site coordinates
 *
 * Changes:
 *   LD/2014.08.26 - Creation
 ************************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 * \brief Header for \c struct #atom - an atom structure containing the
 * element name and x,y,z site coordinates.
 *
 */

#ifndef ATOM_H
#define ATOM_H

#if !__GNUC__
#define __attribute__(x) /* empty */
#define auto_atom
#else
#define auto_atom __attribute__((cleanup(atom_free)))
#endif

#include <stdlib.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*! \typedef atom
 *  \brief basic atom struct.
 */
typedef struct atom {
  char *element;        /*!< Elemental symbol. */
  double x;             /*!< x Cartesian coordinate of atom. */
  double y;             /*!< y Cartesian coordinate of atom. */
  double z;             /*!< z Cartesian coordinate of atom. */
  double rmin;          /*!< minimum radius of atom (muffin-tin radius) */
} atom;

static inline void atom_free(atom *a) { if (a != NULL) free(a); a = NULL; }

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* ATOM_H */
