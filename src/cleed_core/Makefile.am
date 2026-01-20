### automake file for cleed_core ###
# Process this file with automake to produce Makefile.in

lib_LIBRARIES = libcleedcore.a

if WIN32
bin_LTLIBRARIES = libcleedcore.la
else
lib_LTLIBRARIES = libcleedcore.la
endif

libcleedcore_la_SOURCES = cleed_core.c \
    cleed_curve.c

libcleedcore_a_SOURCES = $(libcleedcore_la_SOURCES)
