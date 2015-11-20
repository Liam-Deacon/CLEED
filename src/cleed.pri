# ---------------------------------------------
# CLEED qmake project include file
# ---------------------------------------------
# author: Liam Deacon <liam.m.deacon@gmail.com>

INCLUDEPATH += include 

LIBS += -lm

win32 {
  DEST = "Program Files/CLEED/"
  LIBS *= -lpthread
}

unix {
  DEST = "/usr/local"
}
