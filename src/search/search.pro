# ---------------------------------------------
#         search qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make search' to build 
# ---------------------------------------------

SEARCH_SRCS = *.c
SEARCH_SRCS -= csearch.c c_search.c cysearch.c

# default linear algebra package is in-built one
# based on numerical recipes in C
DEFINES += USE_NATIVE

# use GNU Scientific Library for speedier searches
GSL {
  LIBS += -lgsl -lgslcblas
  DEFINES += USE_GSL
  DEFINES -= USE_NATIVE
} else {
  SEARCH_SRCS -= *gsl*.c
}

# or use BLAS/LAPACK for even faster searches
BLAS|LAPACK {
  win32:HEADERS += "../../lib/win32/lapacke.h"
  win32:LIBS += -L"../../lib/win32/"
  LIBS += -lblas -llapacke
  DEFINES += USE_BLAS USE_LAPACK
  DEFINES -= USE_GSL USE_NATIVE
}

SOURCES = $$SEARCH_SRCS 
SOURCES += csearch.c
