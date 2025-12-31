### automake file for crfac ###
# Process this file with automake to produce Makefile.in

bin_PROGRAMS = crfac

lib_LIBRARIES = librfac.a

if WIN32
bin_LTLIBRARIES = librfac.la
else
lib_LTLIBRARIES = librfac.la
endif

crfac_SOURCES = crfac.c

crfac_LTADD = librfac_la

librfac_la_SOURCES =                        \
    bgets.c                                 \
    file2buffer.c                           \
    rfhelp.c                                \
    rflines.c                               \
    rfctr2out.c                             \
    rfversion.c                             \
    crfinput.c                              \
    crfintindl.c                            \
    crflorentz.c                            \
    crfmklide.c                             \
    crfmklist.c                             \
    crfrdargs.c                             \
    crfrdcleed.c                            \
    crfrdexpt.c                             \
    crfrmin.c                               \
    crfrb.c                                 \
    crfrp.c                                 \
    crfrs.c                                 \
    crfr1.c                                 \
    crfr2.c                                 \
    crfsort.c                               \
    crfspline.c                             \
    crfsplint.c 
    
librfac_a_SOURCES = $(librfac_la_SOURCES)
