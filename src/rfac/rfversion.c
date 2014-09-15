/*********************************************************************
GH/18.02.93

Current Program version and log.

 Version 1.0:
GH/30.10.92 - works only for equidistant energies.

GH/02.11.92 - Find R factor minima.
	    - Read theoretical file in standard format
		(additional to VHT format)
GH/12.11.92 - rf_version as external variable.

 Version 1.1:
GH/17.02.93 - new option -e:
	     Calculate R factor either for I(E) or I(kz) curves. For
	     this purpose an E0 is needed for each set of spots which
	     can be entered in the control file (e0). Alternatively the
	     first energy with nonzero intensities for this set is
	     taken from the theoretical input file.

GH/18.02.93 - introduce function rf_version (program version).
GH/19.02.93 - new normalisation for Rb factors (according to C = 0.75).
GH/01.03.93 - Skaling of theoretical IV curve with respect to
             experimental is changed for R2 (2-Norm instead of 1-Norm).

GH/04.03.93 - Rg has been changed almost completely:
	     Summation is now over (ac^2 + as^2) where ac and as are
	     the cos and sin Fourier Coefficients normalised with
	     respect to a0.
	     There is also a new option -m, which can modify the
	     integration range of Rg.

GH/05.03.93 - Include help utility (rf_help/rfhelpfile)
GH/11.03.93 - Include output of average R-factor when option "-o single"
	     option is chosen. (Does not work yet)
GH/30.06.93 - Program made compatible with cc - compiler (instead of
             gcc)

 Version 1.2:
GH/08.10.93 - Function file2buffer is also capable to read compressed
             files.
GH/08.02.94 - Normalization in Rb is changed according to PhD thesis.
GH/24.06.94 - Error messages for allocation errors are included.

 Version 1.3:
LD/03.04.14 - Added OpenMP support for multi-threaded calculations
            - 

*********************************************************************/
#define RF_VERSION     "RFAC Version 1.3 - LD/03.04.14 (Development)"

#include <string.h>
#include <strings.h>
/********************************************************************/

char *rf_version(char *version)
/*
 Copy the current version ID into the string version.
 The function returns its argument.
*/
{
 strcpy(version, RF_VERSION);
 return (version);
}
/********************************************************************/
