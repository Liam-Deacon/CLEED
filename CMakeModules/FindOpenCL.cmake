#.rst:
# FindOpenCL
# ----------
#
# Try to find OpenCL
#
# Once done this will define::
#
# OpenCL_FOUND - True if OpenCL was found
# OpenCL_INCLUDE_DIRS - include directories for OpenCL
# OpenCL_LIBRARIES - link against this library to use OpenCL
# OpenCL_VERSION_STRING - Highest supported OpenCL version (eg. 1.2)
# OpenCL_VERSION_MAJOR - The major version of the OpenCL implementation
# OpenCL_VERSION_MINOR - The minor version of the OpenCL implementation
#
# The module will also define two cache variables::
#
# OpenCL_INCLUDE_DIR - the OpenCL include directory
# OpenCL_LIBRARY - the path to the OpenCL library
#

#=============================================================================
# Copyright 2014 Matthaeus G. Chajdas
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
# License text for the above reference.)

FIND_PATH(OPENCL_INCLUDE_DIR
        NAMES
                CL/cl.h OpenCL/cl.h
        PATHS
                $ENV{AMDAPPSDKROOT}/include
                $ENV{INTELOCLSDKROOT}/include
                $ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/inc
                # Legacy Stream SDK
                $ENV{ATISTREAMSDKROOT}/include)

IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(OPENCL_LIB_SEARCH_PATH
                ${OPENCL_LIB_SEARCH_PATH}
                $ENV{AMDAPPSDKROOT}/lib/x86
                $ENV{INTELOCLSDKROOT}/lib/x86
                $ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/lib/Win32
                # Legacy Stream SDK
                $ENV{ATISTREAMSDKROOT}/lib/x86)
ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(OPENCL_LIB_SEARCH_PATH
                ${OPENCL_LIB_SEARCH_PATH}
                $ENV{AMDAPPSDKROOT}/lib/x86_64
                $ENV{INTELOCLSDKROOT}/lib/x64
                $ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/lib/x64
                # Legacy stream SDK
                $ENV{ATISTREAMSDKROOT}/lib/x86_64)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)

FIND_LIBRARY(
    OPENCL_LIBRARY
    NAMES OpenCL
    PATHS ${OPENCL_LIB_SEARCH_PATH})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  OpenCL
  DEFAULT_MSG
  OPENCL_LIBRARY OPENCL_INCLUDE_DIR)

if(OPENCL_FOUND)
  set(OPENCL_LIBRARIES ${OPENCL_LIBRARY})
else(OPENCL_FOUND)
  set(OPENCL_LIBRARIES)
endif(OPENCL_FOUND)

mark_as_advanced(
  OPENCL_INCLUDE_DIR
  OPENCL_LIBRARY
  )