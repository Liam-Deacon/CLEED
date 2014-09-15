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

# Program information
SET(CRFAC_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(CRFAC_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(CRFAC_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(CRFAC_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/include/rfac_ver.h.in"
   "${PROJECT_SOURCE_DIR}/src/include/rfac_ver.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (crfac_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "" FORCE)

SET (rfaclib_SRCS
    bgets.c
    file2buffer.c
    rfhelp.c
    rflines.c
    rfctr2out.c
    rfversion.c
    crfinput.c
    crfintindl.c
    crflorentz.c
    crfmklide.c
    crfmklist.c
    crfrdargs.c
    crfrdcleed.c
    crfrdexpt.c
    crfrmin.c
    crfrb.c
    crfrp.c
    crfr1.c
    crfr2.c
    crfsort.c
    crfspline.c
    crfsplint.c 
)

SET (crfac_SRCS crfac.c)

###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(rfac SHARED ${rfaclib_SRCS})
ADD_LIBRARY(rfacStatic STATIC ${rfaclib_SRCS})

SET_TARGET_PROPERTIES(rfacStatic PROPERTIES OUTPUT_NAME rfac)

IF (WIN32)
    ENABLE_LANGUAGE(RC)
    
    # configure for windows icon
    CONFIGURE_FILE ("${crfac_SOURCE_DIR}/res/windows/crfac.rc.in"
        "${crfac_SOURCE_DIR}/res/windows/crfac.rc" IMMEDIATE @ONLY ) 
        
    SET (crfac_SRCS ${crfac_SRCS} res/windows/crfac.rc)
    
    ADD_EXECUTABLE(crfac ${crfac_SRCS})
    
ELSE (WIN32)
    
    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(crfac MACOSX_BUNDLE ${crfac_SRCS})
            INSTALL(TARGETS crfac BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(crfac ${crfac_SRCS})  
            
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(rfac m)
TARGET_LINK_LIBRARIES(rfacStatic m)
TARGET_LINK_LIBRARIES(crfac rfac m)

IF (WIN32)
    INSTALL (TARGETS crfac rfac
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
        COMPONENT runtime
    )
ELSE (WIN32)
    INSTALL (TARGETS crfac rfac
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        COMPONENT runtime
    )
ENDIF (WIN32)

INSTALL(TARGETS rfacStatic ARCHIVE DESTINATION lib COMPONENT libraries)
