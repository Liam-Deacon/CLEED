/************************************************************************
 *                           BASIS_VECTOR.H
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.07.17 - Creation
 ************************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 * \brief file contains \c struct #basis_vector
 */

#ifndef BASIS_VECTOR_H
#define BASIS_VECTOR_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*! \typedef basis_vector
 *  \brief basis vector struct.
 */
typedef struct basis_vector 
{
  double x;
  double y;
  double z;
} basis_vector;
extern basis_vector basis_vector_default; 

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* BASIS_VECTOR_H */
