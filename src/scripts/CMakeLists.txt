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

FIND_PACKAGE(Python3)
IF (PYTHONLIBS_FOUND)

    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIR})
    ADD_EXECUTABLE(set_env set_env.c)
    TARGET_LINK_LIBRARIES(set_env ${PYTHON_LIBRARIES} ${PYTHON_LIBRARY})

    IF (WIN32)
        FIND_LIBRARY(LIBPYTHON python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}.dll
            PATHS "C:/Windows/System32" "C:/Windows/SysWOW64")
        INSTALL(FILES ${LIBPYTHON} DESTINATION bin COMPONENT runtime)
    ENDIF(WIN32)

    INSTALL(TARGETS set_env RUNTIME DESTINATION bin COMPONENT runtime)

ENDIF (PYTHONLIBS_FOUND)

# install scripts
IF (WIN32)
    FILE (GLOB WINDOWS_SCRIPTS *.bat *.cmd *.ps1 *.py *.exe)

    INSTALL(FILES
        ${WINDOWS_SCRIPTS}
        DESTINATION bin
        COMPONENT applications
    )
ELSE (WIN32)
    IF (UNIX)
        IF (APPLE)
            # Currently same as Linux
            FILE (GLOB LINUX_SCRIPTS
                ${PROJECT_SOURCE_DIR}/src/scripts/*.sh
                ${PROJECT_SOURCE_DIR}/src/scripts/*.py
            )
            INSTALL(FILES
                ${PROJECT_SOURCE_DIR}/build/linux/install.sh
                ${LINUX_SCRIPTS}
                DESTINATION bin
                COMPONENT applications
            )
        ELSE (APPLE)
            FILE (GLOB LINUX_SCRIPTS
                ${PROJECT_SOURCE_DIR}/src/scripts/*.sh
                ${PROJECT_SOURCE_DIR}/src/scripts/*.py
            )
            INSTALL(FILES
                ${PROJECT_SOURCE_DIR}/build/linux/install.sh
                ${LINUX_SCRIPTS}
                DESTINATION bin
                COMPONENT applications
            )
        ENDIF (APPLE)
    ENDIF (UNIX)
ENDIF (WIN32)