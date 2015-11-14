# ---------------------------------------------
#         rfac qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make rfac' to build 
# ---------------------------------------------

RFAC_SRCS = *.c
RFAC_SRCS -= rfac_main.c c_rfac.c

SOURCES = $$RFAC_SRCS 
SOURCES += rfac_main.c

TARGET = crfac