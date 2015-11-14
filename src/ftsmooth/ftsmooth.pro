# ---------------------------------------------
#         ftsmooth qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make' to build 
# ---------------------------------------------

FTSMOOTH_SRCS = *.c
FTSMOOTH_SRCS -= test*.c

SOURCES = $$FTSMOOTH_SRCS 