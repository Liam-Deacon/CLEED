# Write configuration file
FILE(WRITE ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "${PREAMBLE}\n")
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "\n")
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "-------------------------------------------------------------------------------\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_INCLUDE_PATH = ${CMAKE_INCLUDE_PATH}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_LIBRARY_PATH = ${CMAKE_LIBRARY_PATH}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_SYSTEM = ${CMAKE_SYSTEM}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_C_COMPILER = ${CMAKE_CXX_COMPILER}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_Fortran_COMPILER = ${CMAKE_Fortran_COMPILER}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "Java_JAVAC_EXECUTABLE = ${Java_JAVAC_EXECUTABLE}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_AR =  ${CMAKE_AR}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_RANLIB =  ${CMAKE_RANLIB}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_C_COMPILER_VERSION = ${CMAKE_C_COMPILER_VERSION}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "COMPILE_DEFINITIONS = ${DEFINES}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "INSTALL_DOC = ${INSTALL_DOC}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "WITH_OPENCL = ${WITH_OPENCL}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "WITH_PHASESHIFTS = ${WITH_PHASESHIFTS}\n" )
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "WITH_QT = ${WITH_QT}")
FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "USE_OPENMP = ${USE_OPENMP}\n" )
IF (WITH_PHASESHIFTS STREQUAL "ON")
    FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "PYTHON_PIP_EXECUTABLE = ${PYTHON_PIP_EXECUTABLE}\n" )
ENDIF (WITH_PHASESHIFTS STREQUAL "ON")
IF (WITH_QT STREQUAL "ON")
    FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_QMAKE_EXECUTABLE = ${QT_QMAKE_EXECUTABLE}")
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_QTCORE_LIBRARY_DEBUG = ${QT_QTCORE_LIBRARY_DEBUG}\n" )
        FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_QTGUI_LIBRARY_DEBUG = ${QT_QTGUI_LIBRARY_DEBUG}\n" )
        FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_QTNETWORK_LIBRARY_DEBUG = ${QT_QTNETWORK_LIBRARY_DEBUG}\n" )
    ELSE()
        FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_QTCORE_LIBRARY_RELEASE = ${QT_QTCORE_LIBRARY_RELEASE}\n" )
        FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_QTGUI_LIBRARY_RELEASE = ${QT_QTGUI_LIBRARY_RELEASE}\n" )
        FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_QTNETWORK_LIBRARY_RELEASE = ${QT_QTNETWORK_LIBRARY_RELEASE}\n" )
    ENDIF()
    #FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_INCLUDES = ${QT_INCLUDES}")
    #FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "QT_LIBRARIES = ${QT_LIBRARIES}")
ENDIF (WITH_QT STREQUAL "ON")
IF (WITH_OPENCL STREQUAL ON)
    FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "OPENCL_INCLUDE_DIR = ${OPENCL_INCLUDE_DIR}\n" )
    FILE(APPEND ${CMAKE_BUILD_DIR}/ConfigurationLog.txt "OPENCL_LIBRARY = ${OPENCL_LIBRARY}\n" )
ENDIF (WITH_OPENCL STREQUAL ON)