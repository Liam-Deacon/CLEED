# ---------------------------------------------
#         libcleed++ qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make' to build 
# ---------------------------------------------

include(../rfac/rfac.pri)
#include(Core/core.pri)
#include(LEED/leed.pri)
#include(RFactor/rfactor.pri)
#include(Search/search.pri)

TEMPLATE = lib

CONFIG = dll #staticlib

QT = # empty

INCLUDEPATH *= ../include

HEADERS += IVCurve.hh \
           IVCurvePair.hh

SOURCES += IVCurve.cc \
           IVCurvePair.cc

SOURCES *= $$RFAC_SRCS
