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

IF (WITH_GSL STREQUAL "OFF")
    # Closed source version
    SET (searchlib_SRCS
        copy_file.c
        nrrutil.c
        nrrbrent.c
        nrrlinmin.c
        nrrmnbrak.c
        nrrran1.c 
        sramoeba.c
        sramebsa.c
        srckgeo.c
        srckrot.c
        srevalrf.c
        srhelp.c
        srmkinp.c
        srpo.c
        srpowell.c
        srrdinp.c
        srrdver.c
        srsa.c
        srer.c
        srsx.c
        ${cleed_nsym_SOURCE_DIR}/linpdebtemp.c
    )
ELSE()
    # Open Source version using GNU Scientific Library
    SET (searchlib_SRCS
        copy_file.c
        srckrot.c
        srhelp.c
        srrdinp.c
        
        #brent_gsl.c
        #linmin_gsl.c
        #ran1.c 
        
        #sramebsa_gsl.c
        #srpowell_gsl.c
        sramoeba_gsl.c
        srckgeo_gsl.c
        srevalrf_gsl.c
        srmkinp_gsl.c
        #srpo_gsl.c
        srrdver_gsl.c
        #srsa_gsl.c
        srer_gsl.c
        srsx_gsl.c
        
        ${cleed_nsym_SOURCE_DIR}/linpdebtemp.c
    )
    ADD_DEFINITIONS(-DUSE_GSL)
    ADD_DEFINITIONS(-D_USE_GSL)
ENDIF()
    
SET (csearch_SRCS csearch.c)

###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(search SHARED ${searchlib_SRCS})
ADD_LIBRARY(searchStatic STATIC ${searchlib_SRCS})

SET_TARGET_PROPERTIES(searchStatic PROPERTIES OUTPUT_NAME search)

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
TARGET_LINK_LIBRARIES(csearch search m)

IF (GSL_LIBRARY)
    TARGET_LINK_LIBRARIES(search gsl)
    TARGET_LINK_LIBRARIES(searchStatic gsl)
    TARGET_LINK_LIBRARIES(csearch search gsl)
ENDIF()

IF (GSL_CBLAS_LIBRARY)
    TARGET_LINK_LIBRARIES(search gslcblas)
    TARGET_LINK_LIBRARIES(searchStatic gslcblas)
    TARGET_LINK_LIBRARIES(csearch search gslcblas)
ENDIF()

IF (WIN32)
    INSTALL (TARGETS csearch search
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
ELSE (WIN32)
    INSTALL (TARGETS csearch search
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
ENDIF (WIN32)

INSTALL(TARGETS searchStatic ARCHIVE DESTINATION lib COMPONENT libraries)
