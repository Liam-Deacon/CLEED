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
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/include/ftsmooth.h.in"
   "${PROJECT_SOURCE_DIR}/src/include/ftsmooth.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (ftsmooth_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "" FORCE)

SET (ftsmoothlib_SRCS
    ftsmooth.c
    ftsmooth_debug.c
    ftsmooth_help.c
    ftsmooth_parse_args.c
    offset_data.c
    print_data.c
    read_data.c
    remove_negative_data.c
    trim_data.c
)

SET (ftsmooth_SRCS ftsmooth_main.c)

###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(ftsmoothLib SHARED ${ftsmoothlib_SRCS})
ADD_LIBRARY(ftsmoothLibStatic STATIC ${ftsmoothlib_SRCS})

SET_TARGET_PROPERTIES(ftsmoothLib PROPERTIES OUTPUT_NAME ftsmooth)
IF (NOT WIN32)
    SET_TARGET_PROPERTIES(ftsmoothLibStatic PROPERTIES OUTPUT_NAME ftsmooth)
ENDIF()

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
IF (WIN32)
    TARGET_LINK_LIBRARIES(ftsmooth ftsmoothLibStatic m)
ELSE()
    TARGET_LINK_LIBRARIES(ftsmooth ftsmoothLib m)
ENDIF()

IF (WIN32)
    INSTALL (TARGETS ftsmooth ftsmoothLib
        COMPONENT runtime
        BUNDLE DESTINATION ../Applications
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
ELSE (WIN32)
    INSTALL (TARGETS ftsmooth ftsmoothLib
        COMPONENT runtime
        BUNDLE DESTINATION ../Applications
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
ENDIF (WIN32)

INSTALL(TARGETS ftsmoothLibStatic ARCHIVE DESTINATION lib COMPONENT libraries)
