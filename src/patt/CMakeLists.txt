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
SET(PATT_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(PATT_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(PATT_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(PATT_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/include/patt.h.in"
   "${PROJECT_SOURCE_DIR}/src/include/patt.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (patt_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET (patt_SRCS 
    patt.c
    patt_decode_vectors.c
    patt_ps_add_vectors.c
    patt_ps_color.c
    patt_ps_draw_init.c
    patt_ps_draw_finalize.c
    patt_ps_draw_gun.c
    patt_ps_draw_vectors.c
    patt_ps_draw_screen.c
    patt_ps_draw_spot.c
    patt_ps_set_linewidth.c
    patt_ps_print_substitute.c
    patt_ps_write_title.c
    patt_fget_nocomm.c
    patt_help.c
    nice_frac.c
)


###############################################################################
# INSTALL TARGETS
###############################################################################
IF (WIN32)
    ENABLE_LANGUAGE(RC)
    # configure for windows icon
    CONFIGURE_FILE ("res/windows/patt.rc.in"
        "res/windows/patt.rc" IMMEDIATE @ONLY )
        
    SET (patt_SRCS ${patt_SRCS} res/windows/patt.rc)
    
    ADD_EXECUTABLE(patt ${patt_SRCS})
    
ELSE (WIN32)

    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(patt MACOSX_BUNDLE ${patt_SRCS})
            INSTALL(TARGETS patt BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(patt ${patt_SRCS})  
            
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)
    
TARGET_LINK_LIBRARIES (patt ${EXTRA_LIBS} m)

INSTALL (TARGETS patt 
    RUNTIME DESTINATION bin 
    COMPONENT runtime
)