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

###############################################################################
# Sources
###############################################################################
SET (debye_SRCS debye.c)


###############################################################################
# INSTALL TARGETS
###############################################################################
IF (WIN32)
    ENABLE_LANGUAGE(RC)
    # configure for windows icon
    # CONFIGURE_FILE ("res/windows/debye.rc.in"
        # "res/windows/debye.rc" IMMEDIATE @ONLY )
        
    # SET (debye_SRCS ${debye_SRCS} res/windows/debye.rc)
    
    ADD_EXECUTABLE(debye ${debye_SRCS})
    
ELSE (WIN32)

    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(debye MACOSX_BUNDLE ${debye_SRCS})
            INSTALL(TARGETS debye BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(debye ${debye_SRCS})  
            
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)
    
TARGET_LINK_LIBRARIES (debye m)

INSTALL (TARGETS debye
    BUNDLE DESTINATION ../Applications
    RUNTIME DESTINATION bin
    COMPONENT runtime
)
