### automake file for mkiv ###
# Process this file with automake to produce Makefile.in

lib_LIBRARIES = libmkiv.a

if WIN32
bin_LTLIBRARIES = libmkiv.la
else
lib_LTLIBRARIES = libmkiv.la
endif

bin_PROGRAMS = mkiv

mkiv_SOURCES = mkiv.c

mkiv_LTADD = libmkiv_la

mkiv_LIBADD = tiff

libmkiv_la_SOURCES =                        \
    bsmooth.c                               \
    calca.c                                 \
    calcbase.c                              \
    calcoi.c                                \
    calcspotpos.c                           \
    convtif2xv.c                            \
    convxv2tif.c                            \
    drawbound.c                             \
    drawell.c                               \
    drawgrid.c                              \
    filename.c                              \
    file_functs.c                           \
    fimax4.c                                \
    getint.c                                \
    markref.c                               \
    mem4spots.c                             \
    mkmask.c                                \
    outtif.c                                \
    plotind.c                               \
    quicksort.c                             \
    readinp.c                               \
    readvar.c                               \
    refinp.c                                \
    setcontrol.c                            \
    sign.c                                  \
    readtif.c                               \
    writetif.c                              \
    signs.h                                 \
    tiffio.h                                \
    variables.h                             \
    v_iorw.h                                \
    vinclude.h                              \
    mkiv.h                                  \
    mkiv_ver.h                              \
    mkiv_help.c

libmkiv_a_SOURCES = $(libmkiv_la_SOURCES)

if WINDOWS
    bin_DATA +=                             \
        ../../lib/win32/libtiff3.dll        \
        ../../lib/win32/jpeg62.dll          \
        ../../lib/win32/zlib1.dll
        
    mkiv_SOURCES += res/windows/mkiv.res
endif