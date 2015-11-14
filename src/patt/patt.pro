# ---------------------------------------------
#         patt qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make' to build 
# ---------------------------------------------

PATT_SRCS = *.c
PATT_SRCS -= c_patt.c

cairo: DEFINES += USE_CAIRO

!cairo {
  CAIRO_SRCS ~= *cairo*.c
  PATT_SRCS -= $$CAIRO_SRCS
}

SOURCES = $$PATT_SRCS 

TARGET = pattern