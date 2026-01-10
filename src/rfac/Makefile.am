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
    rfac.c                                  \
    rfac_ivcur.c                            \
    rfgetsa.c                               \
    rfinput.c                               \
    rfintindl.c                             \
    rflorentz.c                             \
    rfmklide.c                              \
    rfmklist.c                              \
    rfr1.c                                  \
    rfr2.c                                  \
    rfrb.c                                  \
    rfrp.c                                  \
    rfrdargs.c                              \
    rfrdcleed.c                             \
    rfrdexpt.c                              \
    rfrmin.c                                \
    rfsort.c                                \
    rfspline.c                              \
    rfsplint.c                              \
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
