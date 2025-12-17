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

FIND_PACKAGE(TIFF QUIET)

# vcpkg on Windows can provide headers/version but not populate TIFF_LIBRARY via FindTIFF.
# Fall back to a direct library search with additional common names.
IF (NOT TIFF_FOUND)
    SET(TIFF_INCLUDE_DIR "")
    SET(TIFF_LIBRARIES "")
ENDIF()

IF (NOT TIFF_INCLUDE_DIR OR NOT TIFF_LIBRARIES)
    FIND_PATH(TIFF_INCLUDE_DIR NAMES tiffio.h)
    FIND_LIBRARY(
        TIFF_LIBRARY
        NAMES
            tiff
            tiffxx
            tiff3
            tiff4
            libtiff
            libtiffxx
            libtiff3
            libtiff4
    )

    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(TIFF REQUIRED_VARS TIFF_LIBRARY TIFF_INCLUDE_DIR)

    SET(TIFF_LIBRARIES "${TIFF_LIBRARY}")
ENDIF()

IF (NOT TARGET TIFF::TIFF)
    SET(_mkiv_tiff_imported_location "")
    IF (TIFF_LIBRARY)
        SET(_mkiv_tiff_imported_location "${TIFF_LIBRARY}")
    ELSEIF (TIFF_LIBRARIES)
        LIST(GET TIFF_LIBRARIES 0 _mkiv_tiff_imported_location)
    ENDIF()

    IF (NOT _mkiv_tiff_imported_location OR NOT TIFF_INCLUDE_DIR)
        MESSAGE(FATAL_ERROR "mkiv requires libtiff, but TIFF was not found. Install libtiff (and headers) and reconfigure.")
    ENDIF()

    # Prefer shared libtiff if a static archive is discovered (avoids explicitly
    # linking optional codec dependencies).
    IF (_mkiv_tiff_imported_location MATCHES "\\.a$")
        IF (APPLE)
            STRING(REPLACE ".a" ".dylib" _mkiv_tiff_shared_candidate "${_mkiv_tiff_imported_location}")
        ELSE()
            STRING(REPLACE ".a" ".so" _mkiv_tiff_shared_candidate "${_mkiv_tiff_imported_location}")
        ENDIF()
        IF (EXISTS "${_mkiv_tiff_shared_candidate}")
            SET(_mkiv_tiff_imported_location "${_mkiv_tiff_shared_candidate}")
        ENDIF()
    ENDIF()

    ADD_LIBRARY(TIFF::TIFF UNKNOWN IMPORTED)
    SET_TARGET_PROPERTIES(TIFF::TIFF PROPERTIES
        IMPORTED_LOCATION "${_mkiv_tiff_imported_location}"
        INTERFACE_INCLUDE_DIRECTORIES "${TIFF_INCLUDE_DIR}"
    )
ENDIF()

SET(_mkiv_tiff_target TIFF::TIFF)
INCLUDE_DIRECTORIES (${TIFF_INCLUDE_DIR})

SET(_mkiv_tiff_is_static OFF)
IF (_mkiv_tiff_imported_location MATCHES "\\.a$")
    SET(_mkiv_tiff_is_static ON)
ENDIF()

# libtiff may be discovered as a static library (e.g. Homebrew), which requires
# explicitly linking its optional codec dependencies.
SET(_mkiv_tiff_extra_targets "")
SET(_mkiv_tiff_extra_libs "")

IF (_mkiv_tiff_is_static AND NOT WIN32)
    FIND_PACKAGE(JPEG QUIET)
    IF (JPEG_FOUND)
        IF (TARGET JPEG::JPEG)
            LIST(APPEND _mkiv_tiff_extra_targets JPEG::JPEG)
        ELSE()
            LIST(APPEND _mkiv_tiff_extra_libs ${JPEG_LIBRARIES})
            INCLUDE_DIRECTORIES (${JPEG_INCLUDE_DIRS})
        ENDIF()
    ENDIF()

    FIND_PACKAGE(LibLZMA QUIET)
    IF (LIBLZMA_FOUND)
        IF (TARGET LibLZMA::LibLZMA)
            LIST(APPEND _mkiv_tiff_extra_targets LibLZMA::LibLZMA)
        ELSE()
            LIST(APPEND _mkiv_tiff_extra_libs ${LIBLZMA_LIBRARIES})
            INCLUDE_DIRECTORIES (${LIBLZMA_INCLUDE_DIRS})
        ENDIF()
    ENDIF()

    FIND_PACKAGE(ZLIB QUIET)
    IF (ZLIB_FOUND)
        IF (TARGET ZLIB::ZLIB)
            LIST(APPEND _mkiv_tiff_extra_targets ZLIB::ZLIB)
        ELSE()
            LIST(APPEND _mkiv_tiff_extra_libs ${ZLIB_LIBRARIES})
            INCLUDE_DIRECTORIES (${ZLIB_INCLUDE_DIRS})
        ENDIF()
    ENDIF()

    FIND_LIBRARY(_mkiv_zstd_library NAMES zstd)
    IF (_mkiv_zstd_library)
        LIST(APPEND _mkiv_tiff_extra_libs ${_mkiv_zstd_library})
    ENDIF()

    SET(_mkiv_zlib_library "")
    IF (APPLE AND CMAKE_OSX_SYSROOT)
        FIND_LIBRARY(_mkiv_zlib_library NAMES z PATHS "${CMAKE_OSX_SYSROOT}/usr/lib" NO_DEFAULT_PATH)
    ENDIF()
    IF (NOT _mkiv_zlib_library)
        FIND_LIBRARY(_mkiv_zlib_library NAMES z)
    ENDIF()
    IF (_mkiv_zlib_library)
        LIST(APPEND _mkiv_tiff_extra_libs ${_mkiv_zlib_library})
    ELSEIF (UNIX AND NOT WIN32)
        LIST(APPEND _mkiv_tiff_extra_libs z)
    ENDIF()
ENDIF()
                     
###############################################################################
# INSTALL TARGETS
###############################################################################
ADD_LIBRARY(mkivLib SHARED ${mkivLib_SRCS})
ADD_LIBRARY(mkivLibStatic STATIC ${mkivLib_SRCS})

SET_TARGET_PROPERTIES(mkivLib PROPERTIES OUTPUT_NAME mkiv)
IF (NOT WIN32)
    SET_TARGET_PROPERTIES(mkivLibStatic PROPERTIES OUTPUT_NAME mkiv)
ENDIF()

TARGET_LINK_LIBRARIES(mkivLib ${_mkiv_tiff_target} ${_mkiv_tiff_extra_targets} ${_mkiv_tiff_extra_libs} m)
TARGET_LINK_LIBRARIES(mkivLibStatic ${_mkiv_tiff_target} ${_mkiv_tiff_extra_targets} ${_mkiv_tiff_extra_libs} m)

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
    TARGET_LINK_LIBRARIES (mkiv mkivLib m)
    
ELSE (WIN32)
    
    IF (UNIX)
    
        IF(APPLE)
        
            ADD_EXECUTABLE(mkiv MACOSX_BUNDLE ${mkiv_SRCS})
            INSTALL(TARGETS mkiv BUNDLE DESTINATION ../Applications)
            TARGET_LINK_LIBRARIES (mkiv mkivLib m)
            
        ELSE(APPLE)
        
            ADD_EXECUTABLE(mkiv ${mkiv_SRCS})  
            TARGET_LINK_LIBRARIES (mkiv mkivLib m)
        ENDIF(APPLE)
        
    ENDIF(UNIX)
    
ENDIF (WIN32)

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
