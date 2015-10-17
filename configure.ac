##############################################################################
#                                               -*- Autoconf -*-             #
# Process this file with autoconf to produce a configure script.             #
#                                                                            #
# Author: Liam Deacon                                                        #
##############################################################################
AC_PREREQ([2.69])
AC_INIT([CLEED], [2014-04-26_Development], [liam.m.deacon@gmail.com])
AC_CONFIG_SRCDIR([INCLUDE/caoi_leed.h])
AC_CONFIG_AUX_DIR([config])

#=============================================================================
# Set Language to C, check system & check if build is sane
#=============================================================================
#AC_PROG_LIBTOOL
AC_LANG(C)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_SANITY_CHECK

#=============================================================================
# Init Automake subsystem
#=============================================================================
AM_INIT_AUTOMAKE()
AC_CONFIG_HEADERS([config.h])

#=============================================================================
# Check for missing autotools
#=============================================================================
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)

#=============================================================================
# Configuration checks
#=============================================================================

# Check for debug configuration
AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], 
    [build in debug configuration, default: no]),
    DEBUG=1,
    DEBUG=0
)

# Enable/disable LTO
AC_ARG_ENABLE([lto],
    AS_HELP_STRING([--enable-lto], [build with link-time optimizations (-flto)])
)
AS_IF([test "x$enable_lto" = "xyes"], [
    CFLAGS="$CFLAGS -flto" LDFLAGS="-flto $LDFLAGS"
])

# Enable/disable fast math
AC_ARG_ENABLE([fast-math],
    AS_HELP_STRING([--enable-fast-math], 
      [build with unsafe floating point math optimizations (-ffast-math)])
)
AS_IF([test "x$enable_fast_math" = "xyes"], [
    CFLAGS="$CFLAGS -ffast-math"
])

# Enable/disable OpenMP
AC_ARG_ENABLE([openmp],
    AS_HELP_STRING([--disable-openmp], 
      [disable parallel OpenMP code (-fopenmp)])
)
AS_IF([test "x$enable_openmp" != "xno"], [
    CFLAGS="$CFLAGS -fopenmp"
    LDFLAGS="-fopenmp $LDFLAGS"
])

#=============================================================================
# Checks for programs.
#=============================================================================
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_GREP
AC_PROG_RANLIB

#=============================================================================
# Check C Compiler definitions
#=============================================================================
AC_C_BIGENDIAN
AC_C_CONST
AC_C_VOLATILE
AC_C_INLINE
AC_C_CHAR_UNSIGNED
AC_C_LONG_DOUBLE
AC_C_STRINGIZE
AC_C_PROTOTYPES

AC_LANG_C

AC_INT_16_BITS
AC_LONG_64_BITS

AC_FUNC_STRFTIME
AC_HEADER_MAJOR
AC_HEADER_STDC
AC_MEMORY_H
AC_UNISTD_H

#=============================================================================
# Checks for libraries.
#=============================================================================
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lpsapi':
AC_CHECK_LIB([psapi], [main])
# FIXME: Replace `main' with a function in `-lws2_32':
AC_CHECK_LIB([ws2_32], [main])
AC_CHECK_LIB([z], [main])
AC_CHECK_LIB([tiff], [main])
AC_CHECK_LIB([jpeg], [main])
AC_CHECK_LIB([gomp], [main])
AC_CHECK_LIB([OpenCL], [main])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([
    gethostname 
    realpath 
    sqrt 
    strdup 
    strncasecmp 
    getrusage
])


#=============================================================================
# Checks for header files.
#=============================================================================
AC_CHECK_HEADERS([
    limits.h 
    malloc.h 
    stddef.h 
    stdlib.h 
    string.h 
    strings.h 
    sys/time.h 
    unistd.h 
    omp.h
])

#=============================================================================
# Check whether to build OpenCL support
#=============================================================================
AC_ARG_WITH([opencl],
AS_HELP_STRING([--with-opencl], [Build with OpenCL support]), [
  CPPFLAGS="$CPPFLAGS -D_OPENCL"
  AS_IF([test "$(uname -s)" != "Darwin"], 
   [AC_CHECK_HEADERS(CL/cl.h)], 
   [AC_CHECK_HEADERS(OpenCL/cl.h)]
  )
  OPENCL_DIR=opencl
  OPENCL_LT=opencl/lib_opencl.la]
  #PKG_CHECK_MODULES([OPENCL], [OpenCL >= 1.1])]
)
AC_SUBST([OPENCL_DIR])
AC_SUBST([OPENCL_LT])

#=============================================================================
# Check typedefs, structures, and compiler characteristics.
#=============================================================================
AC_TYPE_SIZE_T
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_OFF_T
AC_TYPE_UID_T

#=============================================================================
# Compiler hardening
#=============================================================================
AC_DEFUN([AX_CHECK_COMPILE_FLAG],
[AC_PREREQ(2.59)dnl for _AC_LANG_PREFIX
AS_VAR_PUSHDEF([CACHEVAR],[ax_cv_check_[]_AC_LANG_ABBREV[]flags_$4_$1])dnl
AC_CACHE_CHECK([whether _AC_LANG compiler accepts $1], CACHEVAR, [
  ax_check_save_flags=$[]_AC_LANG_PREFIX[]FLAGS
  _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $4 $1"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
    [AS_VAR_SET(CACHEVAR,[yes])],
    [AS_VAR_SET(CACHEVAR,[no])])
  _AC_LANG_PREFIX[]FLAGS=$ax_check_save_flags])
AS_IF([test x"AS_VAR_GET(CACHEVAR)" = xyes],
  [m4_default([$2], :)],
  [m4_default([$3], :)])
AS_VAR_POPDEF([CACHEVAR])dnl
])dnl AX_CHECK_COMPILE_FLAGS

# Check compiler flags
AX_CHECK_COMPILE_FLAG([-m64], [CFLAGS="$CFLAGS -m64"])
AX_CHECK_COMPILE_FLAG([-mfpmath=sse], [CFLAGS="$CFLAGS -mfpmath=sse"])
AX_CHECK_COMPILE_FLAG([-march=native], [CFLAGS="$CFLAGS -march=native"])
AX_CHECK_COMPILE_FLAG([-funroll-loops], [LDFLAGS="$LDFLAGS -funroll-loops"])
AX_CHECK_COMPILE_FLAG([-fstack-protector-all], 
                      [LDFLAGS="$LDFLAGS -fstack-protector-all"])
AX_CHECK_COMPILE_FLAG([-Wall], [CFLAGS="$CFLAGS -Wall"])
AX_CHECK_COMPILE_FLAG([-Wextra], [CFLAGS="$CFLAGS -Wextra"])

#=============================================================================
# Automake
#=============================================================================
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/aoi_leed/Makefile
                 src/aoi_rfac/Makefile
                 src/debye/Makefile
                 src/ftsmooth/Makefile
                 src/latt/Makefile
                 src/leed_nsym/Makefile
                 src/leed_sym/Makefile
                 src/mkiv/Makefile
                 src/patt/Makefile
                 src/rfac/Makefile
                 src/scripts/Makefile
                 src/search/Makefile
               ])
                 

AC_OUTPUT
