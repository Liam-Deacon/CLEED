/************************************************************************
 *                         ATOM.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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


#ifndef ATOM_H
#define ATOM_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*! \struct atom
 *  \brief basic atom struct.
 */
typedef struct atom {
  char *element;        /*!< Elemental symbol. */
  double x;             /*!< x Cartesian coordinate of atom. */
  double y;             /*!< y Cartesian coordinate of atom. */
  double z;             /*!< z Cartesian coordinate of atom. */
} atom;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* ATOM_H */
