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
SET(AOI_RFAC_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(AOI_RFAC_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(AOI_RFAC_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(AOI_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/include/caoi_rfac.h.in"
    "${PROJECT_SOURCE_DIR}/src/include/caoi_rfac.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (caoi_rfac_SRCS
    caoi_rfac_ctr.c
    caoi_rfac_help.c
    caoi_rfac_main.c
)

###############################################################################
# INSTALL TARGETS
###############################################################################
IF (WIN32)
    ENABLE_LANGUAGE(RC)
    
    # configure for windows icon
    CONFIGURE_FILE ("res/windows/caoi_rfac.rc.in"
        "res/windows/caoi_rfac.rc" IMMEDIATE @ONLY )
        
    SET (caoi_rfac_SRCS ${caoi_rfac_SRCS} res/windows/caoi_rfac.rc)
    
    ADD_EXECUTABLE(caoi_rfac ${caoi_rfac_SRCS})

    TARGET_LINK_LIBRARIES(caoi_rfac ws2_32 psapi)
    
ELSE (WIN32)

    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(caoi_rfac MACOSX_BUNDLE ${caoi_rfac_SRCS})
            INSTALL(TARGETS caoi_rfac BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(caoi_rfac ${caoi_rfac_SRCS})  
            
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)
    
TARGET_LINK_LIBRARIES (caoi_rfac m)

INSTALL (TARGETS caoi_rfac  RUNTIME DESTINATION bin COMPONENT runtime)