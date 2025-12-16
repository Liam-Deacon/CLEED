# CLEED - Computational Low Energy Electron Diffraction
# Copyright (C) 1994-2014 Georg Held
# Copyright (C) 2013-2014 Liam Deacon
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

cmake_minimum_required(VERSION 3.16)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
INCLUDE(InstallRequiredSystemLibraries)
# allow more human readable "if then else" constructs
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set default cmake build type to RelWithDebInfo (None Debug Release RelWithDebInfo MinSizeRel)
IF(CMAKE_BUILD_TYPE STREQUAL "")
    SET(CMAKE_BUILD_TYPE "RelWithDebInfo")
ENDIF()

# set default install prefix to project root directory
IF(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
    SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
ENDIF()


##############################################################################
#                           Project section                                  #
##############################################################################

PROJECT(CLEED)
SET(CPACK_PACKAGE_NAME "CLEED")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Automated Surface Structure determination from LEED-IV curves")
SET(CPACK_PACKAGE_VENDOR "Liam Deacon")
SET(CPACK_PACKAGE_CONTACT "liam.deacon@diamond.ac.uk")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/Readme.md")
IF (EXISTS "${CMAKE_SOURCE_DIR}/RELEASE_NOTES.txt")
    SET(CPACK_RESOURCE_FILE_NOTES "${CMAKE_SOURCE_DIR}/RELEASE_NOTES.txt")
ENDIF()
SET(CPACK_RESOURCE_FILE_CHANGELOG "${CMAKE_SOURCE_DIR}/ChangeLog.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "2014")
SET(CPACK_PACKAGE_VERSION_MINOR "07")
SET(CPACK_PACKAGE_VERSION_PATCH "04")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CLEED")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY 
"CLEED ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_PACKAGE_URL "http://www.reading.ac.uk/chemistry/about/staff/g-held.aspx")
SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")

# Set the options file that needs to be included inside CMakeCPackOptions.cmake
#SET(CPACK_PROJECT_CONFIG_FILE "${CMAKE_BINARY_DIR}/CPackOptions.cmake") # File included at cpack time, once per generator after setting CPACK_GENERATOR to the actual generator being used; allows per-generator setting of CPACK_* variables at cpack time. ${PROJECT_BINARY_DIR}/CPackOptions.cmake

# Branch
SET(SOURCE_BRANCH "Development")
SET(LICENSE "")

# Project options
OPTION(WITH_PHASESHIFTS "Set to ON to download and integrate PhaseShifts Python package" ON)
OPTION(BUILD_SHARED_LIBS "Set to OFF to build static libraries" ON)
OPTION(USE_OPENMP "Set to OFF to have single threaded executables" ON)
OPTION(WITH_QT "Set to ON to build Qt app frontends" ON)
OPTION(WITH_OPENCL "Set to ON to enable OpenCL GPGPU support" ON)
OPTION(INSTALL_DOC "Set to OFF to skip build/install Documentation" ON)
OPTION(PACK_SOURCE "Set to OFF to pack without source code" ON)
OPTION(WITH_GSL "Set to OFF to build without GNU Scientific Library integration - NOTE: this means the code is no longer open source" ON)
OPTION(BUILD_PATT "Build patt (pattern visualisation) - experimental" OFF)

# set cmake module path
IF (NOT CMAKE_MODULE_PATH)
    SET (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
    #LIST (APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
ENDIF (NOT CMAKE_MODULE_PATH)

SET(EXTRA_COMPONENTS "")

IF (WITH_QT STREQUAL "ON")
    SET (QT_INCLUDE_MODULES "QtCore;QtGui;QtNetwork;QtSvg")
    FOREACH (QT_MODULE ${QT_INCLUDE_MODULES})
        STRING(TOUPPER ${QT_MODULE} _upper_qt_module)
        SET (QT_USE_${_upper_qt_module} ON)
    ENDFOREACH (QT_MODULE ${QT_INCLUDE_MODULES})
    IF (QT_VERSION STREQUAL "Qt5" OR QT_VERSION STREQUAL "qt5" )
        FIND_PACKAGE(Qt5)
    ELSE ()
        FIND_PACKAGE(Qt4)
        SET (QT_VERSION "Qt4")
        FOREACH (QT_MODULE ${QT_INCLUDE_MODULES})  
            # apply fix for Windows Qt Library path problems...
            IF (EXISTS QT_QMAKE_EXCUTABLE AND WIN32)
                STRING(TOUPPER ${QT_MODULE} _upper_qt_module)
                GET_FILENAME_COMPONENT(QT_DLL_DIR ${QT_QMAKE_EXECUTABLE} PATH)
                IF (QT_${_upper_qt_module}_LIBRARY_DEBUG MATCHES "*NOTFOUND")
                    SET (QT_${_upper_qt_module}_LIBRARY_DEBUG 
                        "${QT_DLL_DIR}/${_upper_qt_module}d4.dll"
                        CACHE STRING "" FORCE)
                ENDIF ()
                IF (QT_${_upper_qt_module}_LIBRARY_RELEASE MATCHES "*NOTFOUND")
                    SET (QT_${_upper_qt_module}_LIBRARY_RELEASE
                        "${QT_DLL_DIR}/${_upper_qt_module}4.dll"
                        CACHE STRING "" FORCE)
                ENDIF ()
            ENDIF (EXISTS QT_QMAKE_EXCUTABLE AND WIN32)
        ENDFOREACH (QT_MODULE ${QT_INCLUDE_MODULES})
    ENDIF ()
    
    # Instruct CMake to run moc automatically when needed.
    #SET(CMAKE_AUTOMOC ON)
    SET(CMAKE_INCLUDE_CURRENT_DIR ON)
      
    INCLUDE(${QT_USE_FILE})
    IF (NOT EXTRA_COMPONENTS)
        SET (EXTRA_COMPONENTS gui)
    ELSE ()
        LIST (APPEND EXTRA_COMPONENTS gui)
    ENDIF()
ENDIF ()

# This is only required for the script to work in the version control
IF (WITH_OPENCL STREQUAL "ON")
    SET (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")
    FIND_PACKAGE(OpenCL)
    INCLUDE_DIRECTORIES( ${OpenCL_INCLUDE_DIRS} ${OPENCL_INCLUDE_DIR} )
    IF (OpenCL_FOUND OR OPENCL_FOUND)
        SET (WITH_OPENCL ON)
        ADD_DEFINITIONS(-D_USE_OPENCL)
        GET_FILENAME_COMPONENT(OPENCL_LINK_DIR "${OPENCL_LIBRARY}" PATH)
        FOREACH(OPENCL_LIBRARY_DIR in ${OPENCL_LIB_SEARCH_PATH})
            LINK_DIRECTORIES("${OPENCL_LIBRARY_DIR}")
        ENDFOREACH(OPENCL_LIBRARY_DIR in ${OPENCL_LIBRARY_SEARCH_PATH})
        # SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OPENCL_LIBRARY}")
        # SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OPENCL_LIBRARIES}")
        # SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${OPENCL_LIBRARIES}")
        IF (WIN32)
            INSTALL (FILES "${OPENCL_LIBRARY}" DESTINATION bin COMPONENT runtime)
        ENDIF (WIN32)
    ELSE (OpenCL_FOUND OR OPENCL_FOUND)
        SET (WITH_OPENCL OFF)
    ENDIF (OpenCL_FOUND OR OPENCL_FOUND)    
ENDIF()

# Check whether to build open source version of CLEED
IF (WITH_GSL STREQUAL "ON")
    set( GSL_FOUND OFF )
    set( GSL_CBLAS_FOUND OFF )

    FIND_LIBRARY(GSL_LIBRARY NAMES gsl)
    IF (GSL_LIBRARY)
        SET (WITH_GSL ON)
        ADD_DEFINITIONS(-D_USE_GSL)
        GET_FILENAME_COMPONENT(GSL_LIBRARY_DIR "${GSL_LIBRARY}" PATH)        
        LINK_DIRECTORIES("${GSL_LIBRARY_DIR}")
        IF (WIN32)
            INSTALL (FILES "${GSL_LIBRARY}" DESTINATION bin COMPONENT runtime)
        ENDIF (WIN32)
        FIND_LIBRARY(GSL_CBLAS_LIBRARY NAMES gslcblas)
        IF (GSL_CBLAS_LIBRARY)
            GET_FILENAME_COMPONENT(GSL_CBLAS_LIBRARY_DIR "${GSL_CBLAS_LIBRARY}" PATH)        
            LINK_DIRECTORIES("${GSL_CBLAS_LIBRARY_DIR}")
            IF (WIN32)
                INSTALL (FILES "${GSL_CBLAS_LIBRARY}" DESTINATION bin COMPONENT runtime)
            ENDIF (WIN32)            
        ENDIF()
    ELSE (GSL_LIBRARY)
        SET (WITH_GSL OFF)
    ENDIF (GSL_LIBRARY)
ENDIF()

##############################################################################
#                          Compiler section                                  #
##############################################################################

INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCXXCompilerFlag)

#test for ANDROID and look for stuff not relevant to android.
# if(NOT ANDROID)
# #jni is available by default on android
# find_package(JNI)
# include_directories(${JNI_INCLUDE_DIRS})
# endif()

# find java
FIND_PACKAGE(Java)
IF (Java_FOUND)
    INCLUDE (UseJava)
ENDIF()

# find Python libraries & commands
FIND_PACKAGE(PythonInterp)
FIND_PACKAGE(PythonLibs)
FIND_PROGRAM(PYTHON_PIP_EXECUTABLE pip)
FIND_PROGRAM(PYTHON_EASY_INSTALL easy_install)
FIND_PROGRAM(CYTHON NAMES cython cython.py)
FIND_PROGRAM(F2PY_EXECUTABLE NAMES f2py f2py.py)

# enable resource compiler (Windows only)
IF (WIN32)
    ENABLE_LANGUAGE(RC)
ENDIF()

# Set Warnings
IF (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang")
    SET(WARNINGS "-pedantic -Wall -Wextra -Wfloat-equal -Wshadow -Wwrite-strings")
    IF (PARANOID)
        SET (WARNINGS "${WARNINGS} -W -Wconversion -Wcast-qual -Wunreachable-code")
        SET (WARNINGS "${WARNINGS} -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes")
    ENDIF (PARANOID)
    # set list of files to copy for running of program
    GET_FILENAME_COMPONENT(C_COMPILER_PARENT_DIR ${CMAKE_C_COMPILER} PATH)
    FILE (GLOB RUNTIME_LIBRARIES ${C_COMPILER_PARENT_DIR}/*.dll)
ELSEIF("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    SET(WARNINGS "/W4 /WX /EHsc")
    SET(RUNTIME_LIBRARIES
        "C:/Program Files/Microsoft Visual Studio 9.0/VC/redist/x86/Microsoft.VC90.CRT/msvcm90.dll"
        "C:/Program Files/Microsoft Visual Studio 9.0/VC/redist/x86/Microsoft.VC90.CRT/msvcp90.dll"
        "C:/Program Files/Microsoft Visual Studio 9.0/VC/redist/x86/Microsoft.VC90.CRT/msvcr90.dll"
        "C:/Program Files/Microsoft Visual Studio 9.0/VC/redist/x86/Microsoft.VC90.CRT/Microsoft.VC90.CRT.manifest"
    )
ENDIF()

# check compiler flags
SET(CMAKE_CXX_FLAGS ${WARNINGS}
    CACHE STRING "Flags used by the compiler during all build types." FORCE)
SET(CMAKE_C_FLAGS ${WARNINGS}
    CACHE STRING "Flags used by the compiler during all build types." FORCE)

IF (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang")
    
    # check compiler flags
    CHECK_C_COMPILER_FLAG(-fexceptions CC_HAS_EXCEPTIONS_FLAG)
    CHECK_C_COMPILER_FLAG(-fbounds-check CC_HAS_BOUNDS_CHECK_FLAG)
    CHECK_C_COMPILER_FLAG(-fstack-check CC_HAS_STACK_CHECK_FLAG)
    CHECK_C_COMPILER_FLAG(-g CC_HAS_G_FLAG)
    CHECK_C_COMPILER_FLAG(-g3 CC_HAS_G3_FLAG)
    CHECK_C_COMPILER_FLAG(-O0 CC_HAS_O0_FLAG)
    CHECK_C_COMPILER_FLAG(-O3 CC_HAS_O3_FLAG)
    CHECK_C_COMPILER_FLAG(-mfpmath=sse CC_HAS_FPMATH_SSE_FLAG)
    CHECK_C_COMPILER_FLAG(-ffast-math CC_HAS_FAST_MATH_FLAG)
    CHECK_C_COMPILER_FLAG(-funroll-loops CC_HAS_UNROLL_LOOPS_FLAG)
    CHECK_C_COMPILER_FLAG(-march=native CC_HAS_ARCH_NATIVE_FLAG)
    CHECK_C_COMPILER_FLAG(-mtune=native CC_HAS_TUNE_NATIVE_FLAG)
    CHECK_C_COMPILER_FLAG(-std=c99 CC_HAS_STD_C99_FLAG)
    CHECK_C_COMPILER_FLAG(-std=gnu99 CC_HAS_STD_GNU99_FLAG)
    CHECK_C_COMPILER_FLAG(-fdiagnostics-color=auto CC_HAS_DIAGNOSTICS_COLOR_FLAG)
        
    IF (CC_HAS_STD_GNU99_FLAG)
        SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
    ELSEIF (CC_HAS_C99_FLAG)
        SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
    ENDIF(CC_HAS_STD_GNU99_FLAG)
    
    IF (CC_HAS_DIAGNOSTICS_COLOR_FLAG)
        SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=auto")
    ENDIF (CC_HAS_DIAGNOSTICS_COLOR_FLAG)
    
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        IF (CC_HAS_O0_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
        ENDIF (CC_HAS_O0_FLAG)
    ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    
    # use '-mtune=native' on all but 'MinSizeRel' build
    IF (NOT CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        IF (CC_HAS_TUNE_NATIVE_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=native")
        ENDIF (CC_HAS_TUNE_NATIVE_FLAG)
    ENDIF (NOT CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    
    # add debugging options or else fast code optimisations for 'MinSizeRel' build
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
        CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        # optimise for debugging
        IF (CC_HAS_EXCEPTIONS_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
        ENDIF (CC_HAS_EXCEPTIONS_FLAG)
            
        IF (CC_HAS_BOUNDS_CHECK_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fbounds-check")
        ENDIF (CC_HAS_BOUNDS_CHECK_FLAG)

        IF (CC_HAS_STACK_CHECK_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-check")
        ENDIF (CC_HAS_STACK_CHECK_FLAG)
        
        IF (CC_HAS_G3_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
        ELSEIF (CC_HAS_G_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
        ENDIF()
    
    ELSEIF (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        # optimise for fast & lean code
        IF (CC_HAS_O3_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
        ENDIF (CC_HAS_O3_FLAG)
        
        IF (CC_HAS_FAST_MATH_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math")
        ENDIF (CC_HAS_FAST_MATH_FLAG)
        
        IF (CC_HAS_UNROLL_LOOPS_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funroll-loops")
        ENDIF (CC_HAS_UNROLL_LOOPS_FLAG)
        
        IF (CC_HAS_ARCH_NATIVE_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
        ENDIF (CC_HAS_ARCH_NATIVE_FLAG)
        
        IF (CC_HAS_FPMATH_SSE_FLAG)
            SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse")
        ENDIF (CC_HAS_FPMATH_SSE_FLAG)
    ENDIF()
    
ENDIF()

IF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    
    # check compiler flags
    CHECK_CXX_COMPILER_FLAG(-fexceptions CXX_HAS_EXCEPTIONS_FLAG)
    CHECK_CXX_COMPILER_FLAG(-fbounds-check CXX_HAS_BOUNDS_CHECK_FLAG)
    CHECK_CXX_COMPILER_FLAG(-fstack-check CXX_HAS_STACK_CHECK_FLAG)
    CHECK_CXX_COMPILER_FLAG(-g CXX_HAS_G_FLAG)
    CHECK_CXX_COMPILER_FLAG(-g3 CXX_HAS_G3_FLAG)
    CHECK_CXX_COMPILER_FLAG(-O0 CXX_HAS_O0_FLAG)
    CHECK_CXX_COMPILER_FLAG(-O3 CXX_HAS_O3_FLAG)
    CHECK_CXX_COMPILER_FLAG(-mfpmath=sse CXX_HAS_FPMATH_SSE_FLAG)
    CHECK_CXX_COMPILER_FLAG(-ffast-math CXX_HAS_FAST_MATH_FLAG)
    CHECK_CXX_COMPILER_FLAG(-funroll-loops CXX_HAS_UNROLL_LOOPS_FLAG)
    CHECK_CXX_COMPILER_FLAG(-march=native CXX_HAS_ARCH_NATIVE_FLAG)
    CHECK_CXX_COMPILER_FLAG(-mtune=native CXX_HAS_TUNE_NATIVE_FLAG)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 CXX_HAS_STD_GNUXX11_FLAG)
    CHECK_CXX_COMPILER_FLAG(-fdiagnostics-color=auto CXX_HAS_DIAGNOSTICS_COLOR_FLAG)
        
    # IF (CXX_HAS_STD_GNUXX11_FLAG)
        # SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    # ENDIF(CXX_HAS_STD_GNUXX11_FLAG)
    
    IF (CXX_HAS_DIAGNOSTICS_COLOR_FLAG)
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
    ENDIF (CXX_HAS_DIAGNOSTICS_COLOR_FLAG)
    
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        IF (CXX_HAS_O0_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
        ENDIF (CXX_HAS_O0_FLAG)
    ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    
    # use '-mtune=native' on all but 'MinSizeRel' build
    IF (NOT CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        IF (CXX_HAS_TUNE_NATIVE_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=native")
        ENDIF (CXX_HAS_TUNE_NATIVE_FLAG)
    ENDIF (NOT CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    
    # add debugging options or else fast code optimisations for 'MinSizeRel' build
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
        CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        # optimise for debugging
        IF (CXX_HAS_EXCEPTIONS_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
        ENDIF (CXX_HAS_EXCEPTIONS_FLAG)
            
        IF (CXX_HAS_BOUNDS_CHECK_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fbounds-check")
        ENDIF (CXX_HAS_BOUNDS_CHECK_FLAG)

        IF (CXX_HAS_STACK_CHECK_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-check")
        ENDIF (CXX_HAS_STACK_CHECK_FLAG)
        
        IF (CXX_HAS_G3_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
        ELSEIF (CXX_HAS_G_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        ENDIF()
    
    ELSEIF (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        # optimise for fast & lean code
        IF (CXX_HAS_O3_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
        ENDIF (CXX_HAS_O3_FLAG)
        
        IF (CXX_HAS_FAST_MATH_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
        ENDIF (CXX_HAS_FAST_MATH_FLAG)
        
        IF (CXX_HAS_UNROLL_LOOPS_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops")
        ENDIF (CXX_HAS_UNROLL_LOOPS_FLAG)
        
        IF (CXX_HAS_ARCH_NATIVE_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        ENDIF (CXX_HAS_ARCH_NATIVE_FLAG)
        
        IF (CXX_HAS_FPMATH_SSE_FLAG)
            SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse")
        ENDIF (CXX_HAS_FPMATH_SSE_FLAG)
    ENDIF()
    
ENDIF()

# FFLAGS depend on the compiler
if (Fortran_COMPILER_NAME MATCHES "gfortran*")  # gfortran
  set (CMAKE_Fortran_FLAGS_RELEASE "-funroll-all-loops -fno-f2c -O2")
  set (CMAKE_Fortran_FLAGS_DEBUG   "-fno-f2c -O0 -g")
elseif (Fortran_COMPILER_NAME MATCHES "ifort.*")  # ifort (untested)
  set (CMAKE_Fortran_FLAGS_RELEASE "-f77rtl -O3")
  set (CMAKE_Fortran_FLAGS_DEBUG   "-f77rtl -O0 -g")
elseif (Fortran_COMPILER_NAME MATCHES "g77")  # g77
  # set (CMAKE_Fortran_FLAGS_RELEASE "-funroll-all-loops -fno-f2c -O3 -m32")
  # set (CMAKE_Fortran_FLAGS_DEBUG   "-fno-f2c -O0 -g -m32")
else (Fortran_COMPILER_NAME MATCHES "gfortran*")
  message ("CMAKE_Fortran_COMPILER full path: " ${CMAKE_Fortran_COMPILER})
  message ("Fortran compiler: " ${Fortran_COMPILER_NAME})
  message ("No optimized Fortran compiler flags are known, we just try -O2...")
  set (CMAKE_Fortran_FLAGS_RELEASE "-O2")
  set (CMAKE_Fortran_FLAGS_DEBUG   "-O0 -g")
endif (Fortran_COMPILER_NAME MATCHES "gfortran*")

# check for OpenMP usability
IF (USE_OPENMP STREQUAL "ON")
    FIND_PACKAGE(OpenMP)
    IF (OPENMP_FOUND OR OpenMP_FOUND)
        SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        SET (CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${OpenMP_Fortran_FLAGS}")
        LIST (REMOVE_DUPLICATES CMAKE_C_FLAGS)
        LIST (REMOVE_DUPLICATES CMAKE_CXX_FLAGS)
        ADD_DEFINITIONS (-D_USE_OPENMP)
        SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
        SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
        SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
        SET (USE_OPENMP ON)
    ELSE (OPENMP_FOUND OR OpenMP_FOUND)
        SET (USE_OPENMP OFF)
    ENDIF (OPENMP_FOUND OR OpenMP_FOUND)
ENDIF(USE_OPENMP STREQUAL "ON")

# definitions
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" OR
    CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    ADD_DEFINITIONS(-DDEBUG)
ENDIF()
ADD_DEFINITIONS(-D_USE_MATH_DEFINES)

INCLUDE_DIRECTORIES ( ${PROJECT_SOURCE_DIR}/src/include)

# LD flags
IF(WIN32)
    SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
ELSE(WIN32)
    SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
ENDIF(WIN32)

SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lm")
SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lm")

# MinGW can also link to a MS .lib
if(MSYS OR MINGW)
  set(CMAKE_EXTRA_LINK_EXTENSIONS ".lib") 
endif()

# create uninstall configuration file
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake_uninstall.cmake.in"
   "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
   IMMEDIATE @ONLY 
)

# create uninstall target
ADD_CUSTOM_TARGET( uninstall "${CMAKE_COMMAND}" 
    -P "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake" 
)

# display status message for important variables
get_directory_property(DEFINES COMPILE_DEFINITIONS)
IF (WIN32)
    SET (USERNAME "$ENV{USERNAME}")
ELSE (WIN32)
    SET (USERNAME "$ENV{USER}")
ENDIF(WIN32)
SITE_NAME(HOSTNAME)
SET (PREAMBLE "CMake configuration log generated by ${USERNAME} on ${HOSTNAME}")
MESSAGE( STATUS )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "CMAKE_INCLUDE_PATH = ${CMAKE_INCLUDE_PATH}" )
MESSAGE( STATUS "CMAKE_LIBRARY_PATH = ${CMAKE_LIBRARY_PATH}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "CMAKE_SYSTEM = ${CMAKE_SYSTEM}" )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}" )
MESSAGE( STATUS "CMAKE_C_COMPILER = ${CMAKE_CXX_COMPILER}" )
MESSAGE( STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}" )
MESSAGE( STATUS "CMAKE_Fortran_COMPILER = ${CMAKE_Fortran_COMPILER}" )
MESSAGE( STATUS "Java_JAVAC_EXECUTABLE = ${Java_JAVAC_EXECUTABLE}" )
MESSAGE( STATUS "CMAKE_AR =  ${CMAKE_AR}" )
MESSAGE( STATUS "CMAKE_RANLIB =  ${CMAKE_RANLIB}" )
MESSAGE( STATUS "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}" )
MESSAGE( STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}" )
IF ( NOT CMAKE_BUILD_TYPE STREQUAL "None" AND NOT CMAKE_BUILD_TYPE STREQUAL "")
    STRING(TOUPPER ${CMAKE_BUILD_TYPE} _cmake_build_type_ )
    SET (CMAKE_C_FLAGS_${_cmake_build_type_} "${CMAKE_C_FLAGS}")
    SET (CMAKE_CXX_FLAGS_${_cmake_build_type_} "${CMAKE_CXX_FLAGS}")
    SET (CMAKE_SHARED_LINKER_FLAGS_${_cmake_build_type_} "${CMAKE_SHARED_LINKER_FLAGS}")
    SET (CMAKE_MODULE_LINKER_FLAGS_${_cmake_build_type_} "${CMAKE_MODULE_LINKER_FLAGS}")
    MESSAGE (STATUS "CMAKE_C_FLAGS_${_cmake_build_type_} = ${CMAKE_C_FLAGS_${_cmake_build_type_}}")
    MESSAGE (STATUS "CMAKE_CXX_FLAGS_${_cmake_build_type_} = ${CMAKE_CXX_FLAGS_${_cmake_build_type_}}")
ENDIF ()
MESSAGE( STATUS "CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")
MESSAGE( STATUS "CMAKE_MODULE_LINKER_FLAGS = ${CMAKE_MODULE_LINKER_FLAGS}")
MESSAGE( STATUS "CMAKE_SHARED_LINKER_FLAGS = ${CMAKE_SHARED_LINKER_FLAGS}")
MESSAGE( STATUS "CMAKE_C_COMPILER_VERSION = ${CMAKE_C_COMPILER_VERSION}" )
MESSAGE( STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}" )
MESSAGE( STATUS "COMPILE_DEFINITIONS = ${DEFINES}" )
MESSAGE( STATUS "INSTALL_DOC = ${INSTALL_DOC}" )
MESSAGE( STATUS "WITH_OPENCL = ${WITH_OPENCL}" )
MESSAGE( STATUS "WITH_PHASESHIFTS = ${WITH_PHASESHIFTS}" )
MESSAGE( STATUS "WITH_QT = ${WITH_QT}")
MESSAGE( STATUS "USE_OPENMP = ${USE_OPENMP}" )
IF (PYTHONLIBS_FOUND)
    MESSAGE( STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
    MESSAGE( STATUS "PYTHON_INCLUDE_PATH = ${PYTHON_INCLUDE_PATH}")
    MESSAGE( STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
    MESSAGE( STATUS "PYTHONLIBS_VERSION_STRING = ${PYTHONLIBS_VERSION_STRING}")
ENDIF (PYTHONLIBS_FOUND)
IF (CYTHON_EXECUTABLE)
    MESSAGE( STATUS "CYTHON_EXECUTABLE = ${CYTHON_EXECUTABLE}")
ENDIF (CYTHON_EXECUTABLE)
IF (WITH_PHASESHIFTS STREQUAL "ON")
    MESSAGE (STATUS "PYTHON_PIP_EXECUTABLE = ${PYTHON_PIP_EXECUTABLE}" )
ENDIF (WITH_PHASESHIFTS STREQUAL "ON")
IF (F2PY_EXECUTABLE)
    MESSAGE (STATUS "F2PY_EXECUTABLE = ${F2PY_EXECUTABLE}")
ENDIF (F2PY_EXECUTABLE)

IF (WITH_QT STREQUAL "ON")
    MESSAGE (STATUS "QT_QMAKE_EXECUTABLE = ${QT_QMAKE_EXECUTABLE}")
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        MESSAGE (STATUS "QT_QTCORE_LIBRARY_DEBUG = ${QT_QTCORE_LIBRARY_DEBUG}" )
        MESSAGE (STATUS "QT_QTGUI_LIBRARY_DEBUG = ${QT_QTGUI_LIBRARY_DEBUG}" )
        MESSAGE (STATUS "QT_QTNETWORK_LIBRARY_DEBUG = ${QT_QTNETWORK_LIBRARY_DEBUG}" )
    ELSE()
        MESSAGE (STATUS "QT_QTCORE_LIBRARY_RELEASE = ${QT_QTCORE_LIBRARY_RELEASE}" )
        MESSAGE (STATUS "QT_QTGUI_LIBRARY_RELEASE = ${QT_QTGUI_LIBRARY_RELEASE}" )
        MESSAGE (STATUS "QT_QTNETWORK_LIBRARY_RELEASE = ${QT_QTNETWORK_LIBRARY_RELEASE}" )
    ENDIF()
    #MESSAGE (STATUS "QT_INCLUDES = ${QT_INCLUDES}")
    #MESSAGE (STATUS "QT_LIBRARIES = ${QT_LIBRARIES}")
ENDIF (WITH_QT STREQUAL "ON")
IF (USE_OPENMP STREQUAL ON)
    MESSAGE (STATUS "OPENMP_LINKER_FLAGS = ${OpenMP_CXX_FLAGS}" )
ENDIF (USE_OPENMP STREQUAL ON)
IF (WITH_OPENCL STREQUAL ON)
    MESSAGE (STATUS "OPENCL_LIB_SEARCH_PATH = ${OPENCL_LIB_SEARCH_PATH}" )
    MESSAGE (STATUS "OPENCL_INCLUDE_DIR = ${OPENCL_INCLUDE_DIR}" )
    MESSAGE (STATUS "OPENCL_LIBRARY = ${OPENCL_LIBRARY}" )
ENDIF (WITH_OPENCL STREQUAL ON)
IF (WITH_GSL STREQUAL ON)
    MESSAGE(STATUS "GSL_LIBRARY = ${GSL_LIBRARY}")
    IF (GSL_CBLAS_LIBRARY)
        MESSAGE(STATUS "GSL_CBLAS_LIBRARY = ${GSL_CBLAS_LIBRARY}")
    ENDIF (GSL_CBLAS_LIBRARY)
ENDIF (WITH_GSL STREQUAL ON)

MESSAGE( STATUS )
MESSAGE( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE( STATUS "-------------------------------------------------------------------------------" )
MESSAGE( STATUS )

#CHECK_INCLUDE_FILES(CheckIncludeFiles)
#CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)

##############################################################################
#                         Packaging section                                  #
##############################################################################
ADD_SUBDIRECTORY(src)

IF (USE_PHASESHIFTS STREQUAL "ON")
    IF (NOT PYTHON_PIP_EXECUTABLE STREQUAL "PYTHON_PIP-NOTFOUND")
        ADD_CUSTOM_COMMAND (TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND pip install phaseshifts
            COMMENT "Install Python phaseshift package using pip"
        )
    ENDIF (NOT PYTHON_PIP STREQUAL "PYTHON_PIP-NOTFOUND")
ENDIF (USE_PHASESHIFTS STREQUAL "ON")

# install documents
IF (INSTALL_DOC STREQUAL "ON")
    LIST (APPEND EXTRA_COMPONENTS "documentation")
    FILE(GLOB_RECURSE DOCUMENTS  RELATIVE ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/doc/*.pdf)
    INSTALL(FILES ${DOCUMENTS} DESTINATION doc COMPONENT documentation)
    
    FILE(GLOB_RECURSE MAN_PAGES RELATIVE ${PROJECT_SOURCE_DIR} 
        ${PROJECT_SOURCE_DIR}/man/*.*)
    INSTALL(FILES ${MAN_PAGES} DESTINATION man COMPONENT documentation)

    SET (CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")
ENDIF (INSTALL_DOC STREQUAL "ON")

IF (WIN32)
    # ============ BUILD SETTINGS FOR WINDOWS ============
    # ============ USING VISUAL STUDIO 2008 ============
    # /Od -> disable optimization
    # /W3 -> warning level
    # /ZI -> program database for edit & continue
    # /Gm -> enable minimal rebuild
    # /RTC1 -> runtime checks: stack frames + uninitialized variables
    # /MTd -> Multi-threaded Debug
    
    # Given this bug in the NMake generator related to 'manifest files' in CMake 2.6
    # http://www.cmake.org/Bug/view.php?id=7222
    # it is recommended to use CMake 2.4.7 in Windows, so that both the
    # application and the installer can be built from the command line.

    # SET(CMAKE_CXX_FLAGS_DEBUG "/RTC1 /Gm /MTd /ZI /W3 /Od")
    #SET(CMAKE_CXX_FLAGS_RELEASE "/RTC1 /Gm /MT /Zi")
    #SET(CMAKE_EXE_LINKER_FLAGS "/MACHINE:X86 /DYNAMICBASE:NO /MANIFEST:NO /LTCG")
    
    # This sets up the exe icon for windows under mingw.
    IF (MINGW)
        SET(CMAKE_RC_COMPILER_INIT windres)
        ENABLE_LANGUAGE(RC)
        SET(CMAKE_RC_COMPILE_OBJECT
            "<CMAKE_RC_COMPILER> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
    ENDIF (MINGW)

    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}\\\\src\\\\res\\\\CLEED.ico")
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\cleed_nsym.exe")
    SET(CPACK_NSIS_HELP_LINK "http://www.reading.ac.uk/web/FILES/chemistry/Georg_Held_LEED_Description.pdf")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.reading.ac.uk/web/FILES/chemistry/Georg_Held_LEED_Description.pdf")
    SET(CPACK_NSIS_CONTACT "g.held@reading.ac.uk or liam.deacon@diamond.ac.uk")
    SET(CPACK_NSIS_MODIFY_PATH ON)
    # SET(CPACK_PACKAGE_EXECUTABLES "cleed-gui;cleed-gui")
    SET(CPACK_GENERATOR "NSIS;ZIP")
    SET(CPACK_NSIS_DISPLAY_NAME "CLEED")
    SET(CPACK_NSIS_INSTALLER_ICON_CODE "")
    SET(CPACK_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
    
ELSE (WIN32)

    IF (UNIX)
        IF (APPLE)
            # This is bundle metadata to create "bundles" and "Universal Binaries" on Mac OS X
            # http://www.cmake.org/Wiki/CMake_FAQ#How_do_I_build_universal_binaries_on_Mac_OS_X.3F
            # By the way, there is a bug in the cross-compilation SDKs for Tiger in Leopard,
            # so you need to follow these instructions:
            # http://groups.google.com/group/vim_mac/browse_thread/thread/064da55ec412e768
            # which means typing the following command in Terminal.app:
            # $ sudo ln -s /Developer/SDKs/MacOSX10.4u.sdk/usr/lib/crt1.o /Developer/SDKs/MacOSX10.4u.sdk/usr/lib/crt1.10.5.o
            # Do not force SDK/architectures here; let the active toolchain decide.
            SET(MACOSX_BUNDLE_ICON_FILE icon.icns)
            SET(MACOSX_BUNDLE_BUNDLE_NAME cleed)
            SET(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
            SET(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
            SET(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${VERSION}")

            # ============ BUILD SETTINGS FOR MAC ============
            # ============ USING MAKEFILE ============
            # This information is needed to compile the GUI application as a "bundle".
            # Info on how to use OSX frameworks taken from
            # http://www.vtk.org/Wiki/CMake:HowToUseExistingOSXFrameworks
        
            # Header files are located in these directories
            INCLUDE_DIRECTORIES (
                /usr/local/include/cleed/../ # Somehow the tests won't build if not specified like this!
            )
        
            # Libraries are located in these directories
            LINK_DIRECTORIES (/usr/local/lib/)

            ENABLE_TESTING()
            #ADD_TEST(cleed ${CMAKE_CURRENT_BINARY_DIR}/cleedtest)
            
            # The support for Mac OS X bundle installation is available for version 2.6 of CMake:
            # http://www.cmake.org/pipermail/cmake/2007-October/016893.html

            
            SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cleed ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
            SET(CPACK_PACKAGE_EXECUTABLES "cleed;cleed")
            SET(CPACK_GENERATOR "PackageMaker")
            
            # `install_files()` was removed from CMake; keep runtime library installs
            # in the relevant per-platform targets instead.
        
        ELSE (APPLE)
            # ============ BUILD SETTINGS FOR LINUX ============
            # ============ USING MAKEFILE ============
            SET(EXTRA_LIBS
                pthread
            )
            # Header files are located in these directories
            INCLUDE_DIRECTORIES (
                "/usr/local/include/"
                "/usr/include"
            )
        
            # Libraries are located in these directories
            LINK_DIRECTORIES (
                "/usr/local/lib/"
                "/usr/X11R6/lib/"
            )
            
            ADD_DEFINITIONS(
                -DLINUX
            )
        
            SET (CPACK_GENERATOR "TGZ;STGZ")  
        
            IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
                SET (DDM_DESKTOP_DIR "/usr/share/applications/")
                SET (DDM_PIXMAPS_DIR "/usr/share/icons/")
                
                # copy the icon/pixmap files
                INSTALL (FILES 
                    share/*.desktop 
                    share/*.xml 
                    DESTINATION ${DDM_DESKTOP_DIR}
                )
                INSTALL (FILES share/*.png DESTINATION ${DDM_PIXMAPS_DIR})
                
                # try to set up the menu system
                find_program(XDG-MIME_EXECUTABLE xdg-mime)
                find_program(XDG-DESKTOP-MENU_EXECUTABLE xdg-desktop-menu)
                
                INSTALL(CODE "
                    execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor ${DDM_DESKTOP_DIR}/ddm.xml)
                    execute_process(COMMAND ${XDG-DESKTOP-MENU_EXECUTABLE} install --novendor ${DDM_DESKTOP_DIR}/ddm.desktop)
                    execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default ${DDM_DESKTOP_DIR}/ddm.desktop application/x-ddm-item)
                    "
                )
                
                # Debian packaging (optional; module may not exist in-tree yet)
                IF (EXISTS "${CMAKE_MODULE_PATH}/DpkgBuild.cmake")
                    INCLUDE ("${CMAKE_MODULE_PATH}/DpkgBuild.cmake")
                ELSE()
                    SET(DPKG_FOUND OFF)
                ENDIF()
                
                IF(DPKG_FOUND AND NOT WIN32)
                    SET(CPACK_GENERATOR "${CPACK_GENERATOR};DEB")
                    SET(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
                    SET(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
                    SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
                    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
                    SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
                    SET(CPACK_DEBIAN_PACKAGE_DEBUG ON)
                    SET(CPACK_DEBIAN_PACKAGE_DEPENDS ${PACKAGE_REQUIRES})
                    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libtiff ( >= 3.0 ), libpng, libz") # Specify dependencies here
                    SET(CPACK_DEBIAN_PACKAGE_SUGGESTS "ImageMagick, Python")
                    SET(CPACK_SET_DESTDIR TRUE)
                ENDIF(DPKG_FOUND AND NOT WIN32)
                
                # RPM packaging (optional; module may not exist in-tree yet)
                IF (EXISTS "${CMAKE_MODULE_PATH}/RpmBuild.cmake")
                    INCLUDE ("${CMAKE_MODULE_PATH}/RpmBuild.cmake")
                ELSE()
                    SET(RPM_FOUND OFF)
                ENDIF()
                
                IF (RPM_FOUND AND NOT WIN32)
                    SET(CPACK_GENERATOR "${CPACK_GENERATOR};RPM")
                    SET(CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
                    SET(CPACK_RPM_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
                    SET(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
                    SET(CPACK_RPM_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
                    SET(CPACK_RPM_PACKAGE_PRIORITY "optional")
                    SET(CPACK_RPM_PACKAGE_LICENSE "${LICENSE}")
                    SET(CPACK_RPM_PACKAGE_DEBUG ON)
                    SET(CPACK_RPM_PACKAGE_REQUIRES ${PACKAGE_REQUIRES})
                    SET(CPACK_RPM_PACKAGE_REQUIRES "libtiff ( >= 3.0 ), libpng, libz") # Specify dependencies here
                    SET(CPACK_SET_DESTDIR TRUE)
                    SET(CPACK_RPM_PACKAGE_SUGGESTS "ImageMagick, Python")
                    SET(CPACK_RPM_PACKAGE_URL "${CPACK_PACKAGE_URL}")
                ENDIF (RPM_FOUND AND NOT WIN32)
                
            ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
        
            ENABLE_TESTING()
            ADD_TEST(cleedtest ${CMAKE_CURRENT_BINARY_DIR}/cleedtest)

        ENDIF (APPLE)
    ENDIF (UNIX)
ENDIF (WIN32)

# install include files for external library use
INSTALL ( DIRECTORY "${PROJECT_SOURCE_DIR}/src/include" DESTINATION . 
    COMPONENT headers
    PATTERN "*~" EXCLUDE PATTERN "*CVS*" EXCLUDE 
    PATTERN "*.bak" EXCLUDE PATTERN "*.in" EXCLUDE)

# setup file associations and desktop icons
IF (UNIX AND NOT APPLE)
    INSTALL ( DIRECTORY "${PROJECT_SOURCE_DIR}/share" DESTINATION . COMPONENT associations
        PATTERN "*~" EXCLUDE PATTERN "*CVS*" EXCLUDE PATTERN "*.bak" EXCLUDE)
    SET (CPACK_COMPONENT_ASSOCIATIONS_DISPLAY_NAME "File Associations")
    LIST (APPEND EXTRA_COMPONENTS "associations")
ELSEIF(WIN32)
    FILE(GLOB EXTRA_DIST "${PROJECT_SOURCE_DIR}/data/win32/dist/*.*")
    INSTALL (FILES 
        ${EXTRA_DIST}
        DESTINATION bin 
        COMPONENT runtime
    )

    INSTALL (DIRECTORY "${PROJECT_SOURCE_DIR}/data/win32/icons" DESTINATION . COMPONENT icons)
    SET (CPACK_COMPONENT_ICONS_DISPLAY_NAME "Icons")
    SET (CPACK_COMPONENT_ICONS_GROUP_NAME "extra")
    LIST (APPEND EXTRA_COMPONENTS "icons")
    
    INSTALL (DIRECTORY "${PROJECT_SOURCE_DIR}/data/templates" DESTINATION . COMPONENT explorer)
    SET (CPACK_COMPONENT_EXPLORER_DISPLAY_NAME "Input file templates")
    SET (CPACK_COMPONENT_EXPLORER_GROUP_NAME "extra")
    LIST (APPEND EXTRA_COMPONENTS "explorer")
    
    SET (CPACK_COMPONENT_GROUP_EXTRA_DISPLAY_NAME "Extra Components") 
    SET (CPACK_COMPONENT_GROUP_EXTRA_EXPANDED ON)
ENDIF ()

INSTALL (DIRECTORY "${PROJECT_SOURCE_DIR}/examples" DESTINATION . COMPONENT examples)
INSTALL (DIRECTORY "${PROJECT_SOURCE_DIR}/data/phase" DESTINATION . COMPONENT phaseshifts)

# include source code in package if desired
IF (PACK_SOURCE STREQUAL ON)
    INSTALL (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src" DESTINATION . COMPONENT source)
    SET (CPACK_COMPONENT_SOURCE_DISPLAY_NAME "Source Code")
    LIST (APPEND EXTRA_COMPONENTS "source")
ENDIF (PACK_SOURCE STREQUAL ON)

# specify CPack components
SET (CPACK_COMPONENTS_ALL 
    runtime examples phaseshifts libraries headers)
SET (CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "CLEED Runtime")
SET (CPACK_COMPONENT_RUNTIME_REQUIRED ON)
SET (CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C/C++ Headers")
SET (CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Static Libraries")
SET (CPACK_COMPONENT_EXAMPLES_DISPLAY_NAME "Examples")
SET (CPACK_COMPONENT_PHASESHIFTS_DISPLAY_NAME "Phase Shifts Data")
SET (CPACK_COMPONENT_GUI_DISPLAY_NAME "GUI (experimental)")

# add extra components
FOREACH(COMPONENT ${EXTRA_COMPONENTS})
    LIST (APPEND CPACK_COMPONENTS_ALL ${COMPONENT})
ENDFOREACH()
IF (UNIX AND NOT APPLE)
    LIST (APPEND CPACK_COMPONENTS_ALL associations)
ENDIF (UNIX AND NOT APPLE)

INCLUDE(InstallRequiredSystemLibraries)

# Hack to force 'CMAKE_*' variables at CMake configuration time
SET (DOLLAR "$")
SET (CPACK "@CPACK")
SET (AT "@")
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/CMakeModules/NSIS.template.cmake.in"
    "${CMAKE_BINARY_DIR}/NSIS.template.in" @ONLY)
# configure CPack - doesn't appear to work...
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/CPackOptions.cmake.in"
    "${CMAKE_BINARY_DIR}/CPackOptions.cmake" @ONLY)
INCLUDE(CPack)

#INCLUDE(CreateConfigLog)
