/****************************************************************************
                        PATTERN (Georg Held 22.03.93)
NAME
	pattern -- LEED pattern visualisation program  

DESCRIPTION
	Pattern simulates Low Energy Electron Diffraction (LEED) patterns
to help in analysis of complex surface structures. Diffraction peaks 
(spots), indices, reciprocal lattice vectors & different domains can 
all be visualised using vector graphics (PostScript). 

******************************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef _PATT_H
#define _PATT_H

#include <stdio.h>

#define VERSION "PATTERN - version 1.4 (2014.07.04 - )"
#define NOTES "Pattern is part of the CLEED package by Georg Held\n\t(contact g.held@reading.ac.uk)"

#ifdef USE_MAGICK
#include <MagickCore>
#include <MagickWand>
#endif

#include <patt_ver.h>
#include <patt_def.h>
#include <patt_func.h>

#endif /* _PATT_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
/* add C++ features here */


#endif
