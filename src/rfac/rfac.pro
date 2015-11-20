# ---------------------------------------------
#         rfac qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make rfac' to build 
# ---------------------------------------------

include(rfac.pri)

RFAC_SRCS -= crfac.c c_rfac.c

SOURCES = $$RFAC_SRCS
SOURCES += crfac.c

TARGET = crfac

DISTFILES += \
    rfac.pri
