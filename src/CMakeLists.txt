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

# Recursive CMake Source Subdirectories
ADD_SUBDIRECTORY(aoi_leed)
ADD_SUBDIRECTORY(aoi_rfac)
ADD_SUBDIRECTORY(debye)
ADD_SUBDIRECTORY(ftsmooth)
ADD_SUBDIRECTORY(latt)
ADD_SUBDIRECTORY(leed_nsym)
#ADD_SUBDIRECTORY(leed_sym)
ADD_SUBDIRECTORY(mkiv)
ADD_SUBDIRECTORY(patt)
ADD_SUBDIRECTORY(rfac)
ADD_SUBDIRECTORY(search)
ADD_SUBDIRECTORY(scripts)

# c++ interface library
ADD_SUBDIRECTORY(c++)

# optional gui elements
ADD_SUBDIRECTORY(latt-gui)
ADD_SUBDIRECTORY(patt-gui)

# additional python components
SET(PHASESHIFTS_VERSION_MAJOR 0)
SET(PHASESHIFTS_VERSION_MINOR 1)
SET(PHASESHIFTS_VERSION_PATCH 3)
SET (PHSH_VERSION 
    "phaseshifts-${PHASESHIFTS_VERSION_MAJOR}.${PHASESHIFTS_VERSION_MINOR}.${PHASESHIFTS_VERSION_PATCH}-dev")
SET (PHSH_URL https://pypi.python.org/packages/source/p/phaseshifts/${PHSH_VERSION}.zip)
IF (F2PY_EXECUTABLE AND CYTHON AND PYTHONLIBS_FOUND AND PYTHONLIBS_FOUND)
    IF (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../${PHSH_VERSION}.zip")
        MESSAGE (STATUS "Downloading: ${PHSH_URL}")
        FILE(DOWNLOAD ${PHSH_URL} "${CMAKE_CURRENT_BINARY_DIR}/../${PHSH_VERSION}.zip" 
            TIMEOUT 10 SHOW_PROGRESS STATUS _phsh_status)
    ENDIF(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../${PHSH_VERSION}.zip")
   
   EXECUTE_PROCESS( 
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_BINARY_DIR}/../${PHSH_VERSION}.zip"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        #COMMENT "Extracting ${PHSH_VERSION}..."
    )
    
    SET (phsh_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PHSH_VERSION}/phaseshifts")
    
    ADD_CUSTOM_COMMAND(OUTPUT cython_build
        COMMAND "${PYTHONINTERP}" "Cython-Build.py" build_ext --inplace
        WORKING_DIRECTORY "${phsh_SOURCE_DIR}/cython"
        COMMENT "Cythoning phaseshift modules..."
    )

    ADD_CUSTOM_COMMAND(OUTPUT cython_source
        COMMAND "${PYTHONINTERP}" "${CYTHON}" --cplus --embed phsh.pyx
        WORKING_DIRECTORY "${phsh_SOURCE_DIR}/cython"
        COMMENT "Embedding Python Intepreter into 'phsh.cpp'..."
    )
    
    ENABLE_LANGUAGE(RC)
    CONFIGURE_FILE("${phsh_SOURCE_DIR}/res/windows/phaseshifts.rc.in"
        "${phsh_SOURCE_DIR}/res/windows/phaseshifts.rc") 
    SET (phsh_SRCS 
        "${phsh_SOURCE_DIR}/cython/phsh.cpp"
        "${phsh_SOURCE_DIR}/res/windows/phaseshifts.rc")
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIR})
    ADD_EXECUTABLE(phsh ${phsh_SRCS} cython_source cython_build)
    TARGET_LINK_LIBRARIES(phsh ${PYTHON_LIBRARIES})
    
    # create libphsh library
    SET(libphsh_SRCS "${phsh_SOURCE_DIR}/lib/libphsh.f")
    
    ADD_LIBRARY(libphsh SHARED ${libphsh_SRCS})
    SET_TARGET_PROPERTIES(libphsh PROPERTIES OUTPUT_NAME phsh)
    
    FILE(GLOB PHASESHIFT_DOCUMENTS "${phsh_SOURCE_DIR}/doc/*.pdf")
    INSTALL(FILES ${PHASESHIFT_DOCUMENTS} DESTINATION doc COMPONENT documentation)
    
    IF (WIN32)
        FILE(GLOB PHASESHIFT_LIBRARIES "${phsh_SOURCE_DIR}/cython/*.pyd")
        INSTALL(FILES ${PHASESHIFT_LIBRARIES} DESTINATION bin COMPONENT runtime)
        INSTALL(TARGETS phsh libphsh COMPONENT runtime RUNTIME DESTINATION bin LIBRARY DESTINATION bin)
        FIND_LIBRARY(LIBPYTHON python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}.dll 
            PATHS "C:/Windows/System32" "C:/Windows/SysWOW64")
        INSTALL(FILES ${LIBPYTHON} DESTINATION bin COMPONENT runtime) 
    ELSE(WIN32)
        FILE(GLOB PHASESHIFT_LIBRARIES "${phsh_SOURCE_DIR}/cython/*.so")
        INSTALL(FILES ${PHASESHIFT_LIBRARIES} DESTINATION bin COMPONENT runtime)
        INSTALL(TARGETS phsh libphsh COMPONENT runtime RUNTIME DESTINATION bin LIBRARY DESTINATION lib)
    ENDIF(WIN32)

ENDIF (F2PY_EXECUTABLE AND CYTHON AND PYTHONLIBS_FOUND AND PYTHONLIBS_FOUND)