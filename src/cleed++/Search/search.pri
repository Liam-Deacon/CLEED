# ---------------------------------------------
# libcleed++ search qmake project include file
# ---------------------------------------------
INCLUDEPATH *= ../include
HEADERS += *.hpp *.hh *.h
SOURCES += *.cc
CPP_SOURCES = *.cpp
if (!isEmpty(CPP_SOURCES)) {
  SOURCES += $$CPP_SOURCES
}
