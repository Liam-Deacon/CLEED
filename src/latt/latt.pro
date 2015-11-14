# ---------------------------------------------
#         latt qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make' to build 
# ---------------------------------------------

LATT_SRCS = *.c
LATT_SRCS -= c_latt.c

SOURCES = $$LATT_SRCS 

TARGET = lattice