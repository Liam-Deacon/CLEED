# ---------------------------------------------
#  libcleed++ LEED qmake project include file
# ---------------------------------------------

DIR = LEED

HEADERS += $$DIR/*.hpp $$DIR/*.hh $$DIR/*.h
SOURCES += $$DIR/*.cc
CPP_SOURCES = $$DIR/*.cpp
if (!isEmpty(CPP_SOURCES)) {
  SOURCES += $$CPP_SOURCES
}
