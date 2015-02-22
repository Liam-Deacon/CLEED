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
SET(FTSMOOTH_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(FTSMOOTH_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(FTSMOOTH_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(FTSMOOTH_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/include/ftsmooth_ver.h.in"
   "${PROJECT_SOURCE_DIR}/src/include/ftsmooth_ver.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (ftsmooth_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "" FORCE)

SET (ftsmoothlib_SRCS
    ftsmooth.c
    ftsmooth_debug.c
    ftsmooth_help.c
    ftsmooth_parse_args.c
    fts_offset_data.c
    fts_print_data.c
    fts_read_data.c
    fts_rm_neg_data.c
    fts_trim_data.c
)

SET (ftsmooth_SRCS ftsmooth_main.c)

###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(ftsmoothLib SHARED ${ftsmoothlib_SRCS})
ADD_LIBRARY(ftsmoothLibStatic STATIC ${ftsmoothlib_SRCS})

SET_TARGET_PROPERTIES(ftsmoothLib PROPERTIES OUTPUT_NAME ftsmooth)
SET_TARGET_PROPERTIES(ftsmoothLibStatic PROPERTIES OUTPUT_NAME ftsmooth)

IF (WIN32)
    ENABLE_LANGUAGE(RC)
    
    # configure for windows icon
    CONFIGURE_FILE ("${ftsmooth_SOURCE_DIR}/res/windows/ftsmooth.rc.in"
        "${ftsmooth_SOURCE_DIR}/res/windows/ftsmooth.rc" IMMEDIATE @ONLY )  
        
    SET (ftsmooth_SRCS ${ftsmooth_SRCS} res/windows/ftsmooth.rc)
    
    ADD_EXECUTABLE(ftsmooth ${ftsmooth_SRCS})
    
ELSE (WIN32)
    
    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(ftsmooth MACOSX_BUNDLE ${ftsmooth_SRCS})
            INSTALL(TARGETS ftsmooth BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(ftsmooth ${ftsmooth_SRCS})  
            
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(ftsmoothLib m)
TARGET_LINK_LIBRARIES(ftsmoothLibStatic m)
TARGET_LINK_LIBRARIES(ftsmooth ftsmoothLib m)

IF (WIN32)
    INSTALL (TARGETS ftsmooth ftsmoothLib
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
ELSE (WIN32)
    INSTALL (TARGETS ftsmooth ftsmoothLib
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
ENDIF (WIN32)

INSTALL(TARGETS ftsmoothLibStatic ARCHIVE DESTINATION lib COMPONENT libraries)
