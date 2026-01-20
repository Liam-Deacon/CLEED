### automake file for cleed_io ###
# Process this file with automake to produce Makefile.in

lib_LIBRARIES = libcleedio.a

if WIN32
bin_LTLIBRARIES = libcleedio.la
else
lib_LTLIBRARIES = libcleedio.la
endif

libcleedio_la_SOURCES = cleed_io.c

libcleedio_a_SOURCES = $(libcleedio_la_SOURCES)
