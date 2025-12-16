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
SET(MKIV_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(MKIV_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(MKIV_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(MKIV_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/src/mkiv/mkiv_ver.h.in"
   "${PROJECT_SOURCE_DIR}/src/mkiv/mkiv_ver.h" IMMEDIATE @ONLY )

###############################################################################
# Sources
###############################################################################
SET (mkiv_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "" FORCE)

SET (mkivLib_SRCS
    bsmooth.c
    calca.c
    calcbase.c
    calcoi.c
    calcspotpos.c
    convtif2mat.c
    convmat2tif.c
    drawbound.c
    drawell.c
    drawgrid.c
    filename.c
    file_functs.c
    fimax4.c
    getint.c
    markref.c
    mem4spots.c
    mkmask.c
    outtif.c
    plotind.c
    quicksort.c
    readinp.c
    readvar.c
    refinp.c
    setcontrol.c
    sign.c
    readtif.c
    writetif.c
    
    signs.h
    variables.h
    
    mkiv.h
    mkiv_ver.h
    mkiv_help.c
)

SET (mkiv_SRCS mkiv.c)

# include directories
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/mkiv)

FIND_LIBRARY(TIFF_LIBRARY NAME tiff)
IF(WIN32)
    SET (TIFF_LIBRARY "${PROJECT_SOURCE_DIR}/lib/win32/libtiff3.dll" CACHE STRING "" FORCE)
    GET_FILENAME_COMPONENT(TIFF_LIBRARY_DIR "${TIFF_LIBRARY}" PATH)
ENDIF(WIN32)
                     
###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(mkivLib SHARED ${mkivLib_SRCS})
ADD_LIBRARY(mkivLibStatic STATIC ${mkivLib_SRCS})

SET_TARGET_PROPERTIES(mkivLib PROPERTIES OUTPUT_NAME mkiv)
SET_TARGET_PROPERTIES(mkivLibStatic PROPERTIES OUTPUT_NAME mkiv)

IF (WIN32)
    ENABLE_LANGUAGE(RC)
    
    # configure for windows icon
    CONFIGURE_FILE ("${mkiv_SOURCE_DIR}/res/windows/mkiv.rc.in"
        "${mkiv_SOURCE_DIR}/res/windows/mkiv.rc" IMMEDIATE @ONLY ) 
        
    SET (mkiv_SRCS 
        ${mkiv_SRCS} 
        "${mkiv_SOURCE_DIR}/res/windows/mkiv.rc"
    )
    
    ADD_EXECUTABLE(mkiv ${mkiv_SRCS})
    
    TARGET_LINK_LIBRARIES (mkiv -L"${TIFF_LIBRARY_DIR}" tiff3 m)
    TARGET_LINK_LIBRARIES (mkivLib -L"${TIFF_LIBRARY_DIR}" tiff3 m)
    TARGET_LINK_LIBRARIES (mkivLibStatic -L"${TIFF_LIBRARY_DIR}" tiff3 m)
    
    # These are the symbols for the linker
    INSTALL(FILES
        ${PROJECT_SOURCE_DIR}/lib/win32/libtiff3.dll
        ${PROJECT_SOURCE_DIR}/lib/win32/jpeg62.dll
        ${PROJECT_SOURCE_DIR}/lib/win32/zlib1.dll
        ${RUNTIME_LIBRARIES}
        DESTINATION bin
        COMPONENT runtime
    )
    
    # Header files are located in these directories
    INCLUDE_DIRECTORIES (
        "${PROJECT_SOURCE_DIR}/lib/win32/libtiff/include"
    )
    
ELSE (WIN32)
    
    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(mkiv MACOSX_BUNDLE ${mkiv_SRCS})
            INSTALL(TARGETS mkiv BUNDLE DESTINATION ../Applications)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(mkiv ${mkiv_SRCS})  
            
            # These are the symbols for the linker
            TARGET_LINK_LIBRARIES (mkiv tiff)
            TARGET_LINK_LIBRARIES (mkivLib tiff)
            TARGET_LINK_LIBRARIES (mkivLibStatic tiff)
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(mkivLib m)
TARGET_LINK_LIBRARIES(mkivLibStatic m)
TARGET_LINK_LIBRARIES(mkiv mkivLib m)

IF (WIN32)
    INSTALL (TARGETS mkiv mkivLib
        COMPONENT runtime
        BUNDLE DESTINATION ../Applications
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
ELSE (WIN32)
    INSTALL (TARGETS mkiv mkivLib
        COMPONENT runtime
        BUNDLE DESTINATION ../Applications
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
ENDIF (WIN32)

INSTALL(TARGETS mkivLibStatic ARCHIVE DESTINATION lib COMPONENT libraries)
