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
SET(AOI_LEED_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(AOI_LEED_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(AOI_LEED_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(AOI_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/include/caoi_leed.h.in"
    "${PROJECT_SOURCE_DIR}/src/include/caoi_leed.h" IMMEDIATE @ONLY )

# sources
SET (caoi_leed_SRCS
    caoi_leed_bsr.c
    caoi_leed_help.c
    caoi_leed_main.c
)

IF (WIN32)
    # ADD_CUSTOM_TARGET(caoi_leed.res
        # COMMAND ${CMAKE_RC_COMPILER} 
            # ${caoi_leed_SOURCE_DIR}/res/windows/caoi_leed.rc
            # ${caoi_leed_SOURCE_DIR}/res/windows/caoi_leed.res
        # DEPENDS ${caoi_leed_SOURCE_DIR}/res/windows/caoi_leed.ico
        # SOURCES ${caoi_leed_SOURCE_DIR}/res/windows/caoi_leed.rc
    # )

    # configure for windows icon
    CONFIGURE_FILE ("res/windows/caoi_leed.rc.in"
        "res/windows/caoi_leed.rc" IMMEDIATE @ONLY )
        
    SET (caoi_leed_SRCS ${caoi_leed_SRCS} res/windows/caoi_leed.rc)
    
    ADD_EXECUTABLE(caoi_leed ${caoi_leed_SRCS})

    TARGET_LINK_LIBRARIES(caoi_leed ws2_32 psapi)
    
ELSE (WIN32)

    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(caoi_leed MACOSX_BUNDLE ${caoi_leed_SRCS})
            INSTALL(TARGETS caoi_leed BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(caoi_leed ${caoi_leed_SRCS})   
            
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)
    
TARGET_LINK_LIBRARIES (caoi_leed m)

INSTALL (TARGETS caoi_leed 
    BUNDLE DESTINATION ../Applications
    RUNTIME DESTINATION bin
    COMPONENT runtime
)
