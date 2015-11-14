# ---------------------------------------------
#         mkiv qmake project file
# 
# author: Liam Deacon <liam.m.deacon@gmail.com>
#
# run 'qmake -project && make' to build 
# ---------------------------------------------

MKIV_SRCS = *.c

!win32:LIBS += -ltiff

win32 {
  LIBS += -L"../../lib/win32/" -ltiff3
  INCLUDEPATH += -I"../../lib/win32/libtiff/include/"
}

SOURCES = $$MKIV_SRCS 
