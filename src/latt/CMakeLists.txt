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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
INCLUDE(InstallRequiredSystemLibraries)
# allow more human readable "if then else" constructs
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE )
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

# Program information
SET(LATT_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(LATT_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(LATT_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(LATT_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/../include/latt.h.in"
   "${CMAKE_CURRENT_SOURCE_DIR}/../include/latt.h" IMMEDIATE @ONLY )
   
###############################################################################
# Sources
###############################################################################
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET (latt_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET (latt_SRCS 
    basis.c
    coord.c
    lattice.c
    latt_help.c
    latt_parse_args.c
)


###############################################################################
# INSTALL TARGETS
###############################################################################
IF (WIN32)
    ENABLE_LANGUAGE(RC)
    # configure for windows icon
    CONFIGURE_FILE ("res/windows/latt.rc.in"
        "res/windows/latt.rc" IMMEDIATE @ONLY )
        
    SET (latt_SRCS ${latt_SRCS} res/windows/latt.rc)
    
    ADD_EXECUTABLE(latt latt.c ${latt_SRCS})
    
ELSE (WIN32)

    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(latt MACOSX_BUNDLE ${latt_SRCS})
            INSTALL(TARGETS latt BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(latt ${latt_SRCS})  
            
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)
    
TARGET_LINK_LIBRARIES (latt ${EXTRA_LIBS} m)

INSTALL (TARGETS latt RUNTIME DESTINATION bin COMPONENT runtime)