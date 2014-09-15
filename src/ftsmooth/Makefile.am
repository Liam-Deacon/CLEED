### automake file for ftsmooth ###
# Process this file with automake to produce Makefile.in

lib_LIBRARIES = libftsmooth.a

bin_PROGRAMS = ftsmooth

if WIN32
bin_LTLIBRARIES = libftsmooth.la
else
lib_LTLIBRARIES = libftsmooth.la
endif

ftsmooth_SOURCES = ftsmooth_main.c

ftsmooth_LTADD = libftsmooth_la

ftsmooth_la_SOURCES =                       \
    ftsmooth.c                              \
    ftsmooth_debug.c                        \
    ftsmooth_help.c                         \
    ftsmooth_parse_args.c                   \
    offset_data.c                           \
    print_data.c                            \
    read_data.c                             \
    remove_negative_data.c                  \
    trim_data.c
    
ftsmooth_a_SOURCES = $(ftsmooth_la_SOURCES)