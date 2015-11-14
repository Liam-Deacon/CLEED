# ---------------------------------------------
#         libcleed++ qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make' to build 
# ---------------------------------------------

include(Core/core.pri)
include(LEED/leed.pri)
include(RFactor/rfactor.pri)
include(Search/search.pri)

TEMPLATE = lib

CONFIG = dll staticlib

QT = # empty


HEADERS += *.hpp *.hh *.h
SOURCES += *.cpp *.cc

