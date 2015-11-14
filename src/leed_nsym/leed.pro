# ---------------------------------------------
#         cleed qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make' to build 
# ---------------------------------------------

LEED_SRCS = *.c

# remove GSL sources if not required
!gsl {
  LEED_SRC -= *gsl*
}

win32:HEADERS += getrusage_win32.h

!win32 {
  LEED_SRCS -= getrusage_win32.c
}

SOURCES = $$LEED_SRCS 

TARGET = cleed