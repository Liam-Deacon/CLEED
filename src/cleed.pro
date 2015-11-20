# ---------------------------------------------
#         CLEED qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make all' to build 
# ---------------------------------------------

include(cleed.pri)

TEMPLATE = subdirs

SUBDIRS = cleed++ \
          ftsmooth \
          latt \
          mkiv \
          patt \
          patt-gui \
          rfac \
          search
