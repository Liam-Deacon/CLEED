# ---------------------------------------------
# libcleed++ rfactor qmake project include file
# ---------------------------------------------

HEADERS += *.hpp *.hh *.h
SOURCES += *.cc
CPP_SOURCES = *.cpp
if (!isEmpty(CPP_SOURCES)) {
  SOURCES += $$CPP_SOURCES
}

