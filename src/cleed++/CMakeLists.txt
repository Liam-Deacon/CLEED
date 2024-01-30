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

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
INCLUDE(InstallRequiredSystemLibraries)
# allow more human readable "if then else" constructs
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

###############################################################################
# Sources
###############################################################################
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB_RECURSE cleedlib_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(cleedplusplus SHARED ${cleedlib_SRCS})
ADD_LIBRARY(cleedplusplusStatic STATIC ${cleedlib_SRCS})

SET_TARGET_PROPERTIES(cleedplusplus PROPERTIES OUTPUT_NAME cleed++)
SET_TARGET_PROPERTIES(cleedplusplusStatic PROPERTIES OUTPUT_NAME cleed++)

TARGET_LINK_LIBRARIES(cleedplusplus m)
TARGET_LINK_LIBRARIES(cleedplusplusStatic m)
TARGET_LINK_LIBRARIES(cleedplusplus m)

IF (WIN32)
    INSTALL (TARGETS cleedplusplus
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
        COMPONENT runtime
    )
ELSE (WIN32)
    INSTALL (TARGETS cleedplusplus
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        COMPONENT runtime
    )
ENDIF (WIN32)

INSTALL(TARGETS cleedplusplusStatic ARCHIVE DESTINATION lib COMPONENT libraries)