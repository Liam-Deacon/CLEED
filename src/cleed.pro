# ---------------------------------------------
#         CLEED qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make all' to build 
# ---------------------------------------------

include(cleed.pri)

TEMPLATE = subdirs

SUBDIRS = c++ \
          ftsmooth \
          latt \
          mkiv \
          patt \
          rfac \
          search