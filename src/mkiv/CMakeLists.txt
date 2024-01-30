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
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/src/mkiv/mkiv_ver.h.in"
    "${PROJECT_SOURCE_DIR}/src/mkiv/mkiv_ver.h"
    IMMEDIATE @ONLY
)

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
    img2tif.c
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
    mkiv_funcs.h
    mkiv_ver.h

    mkiv_args.c
    mkiv_help.c
)

SET (mkiv_SRCS mkiv.c)

FIND_LIBRARY(TIFF_LIBRARY NAME tiff tiff3 libtiff libtiff3)
IF(WIN32 AND NOT TIFF_LIBRARY_FOUND)
    SET (TIFF_LIBRARY "${PROJECT_SOURCE_DIR}/lib/win32/libtiff3.dll" CACHE STRING "" FORCE)
    GET_FILENAME_COMPONENT(TIFF_LIBRARY_DIR "${TIFF_LIBRARY}" PATH)
ENDIF()

# try to include ImageMagick
FIND_PACKAGE(ImageMagick COMPONENTS MagickWand MagickCore)
IF(WIN32 AND NOT ImageMagick_FOUND)
    FILE (GLOB ImageMagick_LIBRARIES ${PROJECT_SOURCE_DIR}/lib/win32/CORE_RL_*.dll)
    SET(ImageMagick_INCLUDE_DIRS
        "${PROJECT_SOURCE_DIR}/lib/win32/ImageMagick/include" CACHE STRING "" FORCE)
    SET(ImageMagick_FOUND TRUE)
ENDIF()
IF(ImageMagickFOUND OR (ImageMagick_MagickWand_FOUND AND ImageMagick_MagickCore_FOUND))
    # use the major version number as the value for USE_MAGICK and print a message
    ADD_DEFINITIONS(-DUSE_MAGICK=${ImageMagick_VERSION_MAJOR})
    MESSAGE(STATUS "Using ImageMagick version ${ImageMagick_VERSION_MAJOR}")
ENDIF()

# include directories
INCLUDE_DIRECTORIES (
    ${PROJECT_SOURCE_DIR}/src/mkiv
    ${ImageMagick_MagickWand_INCLUDE_DIRS}
    ${ImageMagick_MagickCore_INCLUDE_DIRS}
)


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

    TARGET_LINK_LIBRARIES (mkiv -L"${TIFF_LIBRARY_DIR}" ${ImageMagick_LIBRARIES} tiff3 m)
    TARGET_LINK_LIBRARIES (mkivLib -L"${TIFF_LIBRARY_DIR}" ${ImageMagick_LIBRARIES} tiff3 m)
    TARGET_LINK_LIBRARIES (mkivLibStatic -L"${TIFF_LIBRARY_DIR}" ${ImageMagick_LIBRARIES} tiff3 m)

    # These are the symbols for the linker
    INSTALL(FILES
        ${ImageMagick_LIBRARIES}
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
        ${ImageMagick_INCLUDE_DIRS}
    )

ELSE (WIN32)

    IF (UNIX)

        IF(APPLE)

            ADD_EXECUTABLE(mkiv MACOSX_BUNDLE ${mkiv_SRCS})
            INSTALL(TARGETS mkiv BUNDLE DESTINATION ../Applications)

        ELSE(APPLE)  # Linux

            ADD_EXECUTABLE(mkiv ${mkiv_SRCS})

            # These are the symbols for the linker
            TARGET_LINK_LIBRARIES (mkiv ${ImageMagick_LIBRARIES} tiff)
            TARGET_LINK_LIBRARIES (mkivLib ${ImageMagick_LIBRARIES} tiff)
            TARGET_LINK_LIBRARIES (mkivLibStatic ${ImageMagick_LIBRARIES} tiff)
        ENDIF(APPLE)

    ENDIF(UNIX)

ENDIF (WIN32)

TARGET_LINK_LIBRARIES(mkivLib m)
TARGET_LINK_LIBRARIES(mkivLibStatic m)
TARGET_LINK_LIBRARIES(mkiv mkivLib m)

IF (WIN32)
    INSTALL (TARGETS mkiv mkivLib
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
    )
ELSE (WIN32)
    INSTALL (TARGETS mkiv mkivLib
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
ENDIF (WIN32)

INSTALL(TARGETS mkivLibStatic ARCHIVE DESTINATION lib COMPONENT libraries)
