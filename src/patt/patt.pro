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
  PATT_SRCS -= *cairo*.c
}

SOURCES = $$PATT_SRCS 

TARGET = pattern
