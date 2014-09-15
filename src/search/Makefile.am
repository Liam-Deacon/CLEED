### automake file for csearch ###
# Process this file with automake to produce Makefile.in

bin_PROGRAMS = csearch

csearch_SOURCES = csearch.c
csearch_LTADD = libsearch_la

if WIN32bin_LTLIBRARY = libsearch.la
else
lib_LTLIBRARY = libsearch.la
endif
libsearch_la_SOURCES =      \    copy_file.c             \    nrrutil.c               \    nrrbrent.c              \    nrrlinmin.c             \    nrrmnbrak.c             \    nrrran1.c               \    sramoeba.c              \    sramebsa.c              \    srckgeo.c               \    srckrot.c               \    srevalrf.c              \    srhelp.c                \    srmkinp.c               \    srpo.c                  \    srpowell.c              \    srrdinp.c               \    srrdver.c               \    srsa.c                  \    srer.c                  \    srsx.c                  \    ../leed_nsym/linpdebtemp.c