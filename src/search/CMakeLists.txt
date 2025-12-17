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
SET(CSEARCH_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(CSEARCH_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(CSEARCH_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(CSEARCH_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/include/search_ver.h.in"
   "${PROJECT_SOURCE_DIR}/src/include/search_ver.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (csearch_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "" FORCE)

SET (searchlib_SRCS
    search_globals.c
    copy_file.c

    # Core optimisation algorithms (GPL-compatible, in-tree)
    sr_alloc.c
    sr_rng.c
    sr_amoeba.c
    sr_amebsa.c
    sr_powell.c

    # Search drivers + evaluation
    srckgeo.c
    srckrot.c
    srevalrf.c
    srhelp.c
    srmkinp.c
    srpo.c
    srrdinp.c
    srrdver.c
    srsa.c
    srer.c
    srsx.c

    ${cleed_nsym_SOURCE_DIR}/linpdebtemp.c
)
    
SET (csearch_SRCS csearch.c)

###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(search SHARED ${searchlib_SRCS})
ADD_LIBRARY(searchStatic STATIC ${searchlib_SRCS})

IF (NOT WIN32)
    SET_TARGET_PROPERTIES(searchStatic PROPERTIES OUTPUT_NAME search)
ENDIF()

IF (WIN32)
    ENABLE_LANGUAGE(RC)
    
    # configure for windows icon
    CONFIGURE_FILE ("${csearch_SOURCE_DIR}/res/windows/csearch.rc.in"
        "${csearch_SOURCE_DIR}/res/windows/csearch.rc" IMMEDIATE @ONLY ) 
        
    SET (csearch_SRCS ${csearch_SRCS} res/windows/csearch.rc)
    
    ADD_EXECUTABLE(csearch ${csearch_SRCS})
    
ELSE (WIN32)
    
    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(csearch MACOSX_BUNDLE ${csearch_SRCS})
            INSTALL(TARGETS csearch BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(csearch ${csearch_SRCS})  
            
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(search m)
TARGET_LINK_LIBRARIES(searchStatic m)
IF (WIN32)
    TARGET_LINK_LIBRARIES(csearch searchStatic m)
ELSE()
    TARGET_LINK_LIBRARIES(csearch search m)
ENDIF()

IF (GSL_LIBRARY)
    TARGET_LINK_LIBRARIES(search gsl)
    TARGET_LINK_LIBRARIES(searchStatic gsl)
    IF (WIN32)
        TARGET_LINK_LIBRARIES(csearch searchStatic gsl)
    ELSE()
        TARGET_LINK_LIBRARIES(csearch search gsl)
    ENDIF()
ENDIF()

IF (GSL_CBLAS_LIBRARY)
    TARGET_LINK_LIBRARIES(search gslcblas)
    TARGET_LINK_LIBRARIES(searchStatic gslcblas)
    IF (WIN32)
        TARGET_LINK_LIBRARIES(csearch searchStatic gslcblas)
    ELSE()
        TARGET_LINK_LIBRARIES(csearch search gslcblas)
    ENDIF()
ENDIF()

IF (WIN32)
    INSTALL (TARGETS csearch search
        COMPONENT runtime
        BUNDLE DESTINATION ../Applications
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
ELSE (WIN32)
    INSTALL (TARGETS csearch search
        COMPONENT runtime
        BUNDLE DESTINATION ../Applications
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
ENDIF (WIN32)

INSTALL(TARGETS searchStatic ARCHIVE DESTINATION lib COMPONENT libraries)
