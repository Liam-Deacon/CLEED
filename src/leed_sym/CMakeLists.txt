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
SET(CLEED_SYM_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(CLEED_SYM_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(CLEED_SYM_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(CLEED_SYM_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/include/leed_ver_sym.h.in"
   "${PROJECT_SOURCE_DIR}/src/include/leed_ver_sym.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (cleed_sym_SRCS cleed_sym.c)

###############################################################################
# INSTALL TARGETS
###############################################################################

IF (WIN32)
    ENABLE_LANGUAGE(RC)
    
    # configure for windows icon
    CONFIGURE_FILE ("${cleed_sym_SOURCE_DIR}/res/windows/cleed_sym.rc.in"
        "${cleed_sym_SOURCE_DIR}/res/windows/cleed_sym.rc" IMMEDIATE @ONLY )       
        
    SET (cleed_sym_SRCS ${cleed_sym_SRCS} res/windows/cleed_sym.rc)
    
    ADD_EXECUTABLE(cleed_sym ${cleed_sym_SRCS})

    TARGET_LINK_LIBRARIES(cleed_sym ws2_32 psapi leed m)
    
ELSE (WIN32)
    
    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(cleed_sym MACOSX_BUNDLE ${cleed_sym_SRCS})
            INSTALL(TARGETS cleed_sym BUNDLE DESTINATION ../Applications)
            
            TARGET_LINK_LIBRARIES(cleed_sym leed m)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(cleed_sym ${cleed_sym_SRCS})  
            
            TARGET_LINK_LIBRARIES(cleed_sym leed m)
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)

INSTALL (TARGETS cleed_sym
    COMPONENT runtime
    BUNDLE DESTINATION ../Applications
    RUNTIME DESTINATION bin
)
