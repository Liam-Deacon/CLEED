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
SET(PATT_GUI_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(PATT_GUI_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(PATT_GUI_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(PATT_GUI_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/patt_gui.hpp.in"
   "${CMAKE_CURRENT_SOURCE_DIR}/patt_gui.hpp" IMMEDIATE @ONLY )

 
###############################################################################
# Sources
###############################################################################
 
SET (patt_gui_HEADERS
    patt_gui.hpp
    calculations.hpp
    config.hpp
    #drawing.hpp
    drawprefs.hpp
    lattice.hpp
    latticestructure.hpp
    mainwindow.hpp
    matrix.hpp
    model.hpp
    paintdialog.hpp
    paintrenderer.hpp
    #pattern.hpp
    preferences.hpp
    qcompile.hpp
    qtarrowitem.hpp
    #shapes.hpp
    superstructuredialog.hpp
)

SET (patt_gui_SRCS
    calculations.cpp
    #drawing.cpp
    drawprefs.cpp
    lattice.cpp
    latticestructure.cpp
    mainwindow.cpp
    matrix.cpp
    model.cpp
    paintdialog.cpp
    paintrenderer.cpp
    #pattern.cpp
    preferences.cpp
    qcompile.cpp
    qtarrowitem.cpp
    #shapes.cpp
    superstructuredialog.cpp
)

SET (patt_gui_ui_SRCS
    alignment.ui
    drawprefs.ui
    mainwindow.ui 
    model.ui
    preferences.ui
    superstructuredialog.ui
)


SET (patt_gui_RESOURCES_RCC res/icons.qrc)
 
IF (WIN32) # Check if we are on Windows
 
 # configure for windows icon
 ENABLE_LANGUAGE(RC)
 CONFIGURE_FILE ("res/windows/patt-gui.rc.in" "res/windows/patt-gui.rc" IMMEDIATE @ONLY )
 LIST(APPEND patt_gui_SRCS res/windows/patt-gui.rc)
 
 SET(PATT_GUI_WIN32_FLAG WIN32)
 
ELSEIF (UNIX)
  # Nothing special required
  SET(PATT_GUI_WIN32_FLAG "")
ELSE ()
  # MESSAGE(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
ENDIF()

IF (TARGET Qt6::Core OR TARGET Qt5::Core)
    SET(CMAKE_AUTOMOC ON)
    SET(CMAKE_AUTOUIC ON)
    SET(CMAKE_AUTORCC ON)

    ADD_EXECUTABLE(patt-gui ${PATT_GUI_WIN32_FLAG}
        main.cpp
        ${patt_gui_SRCS}
        ${patt_gui_ui_SRCS}
        ${patt_gui_RESOURCES_RCC}
    )
    TARGET_INCLUDE_DIRECTORIES(patt-gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    TARGET_LINK_LIBRARIES(patt-gui PRIVATE ${QT_LIBRARIES})
ELSE()
    FIND_PACKAGE(Qt4 REQUIRED)
    INCLUDE(${QT_USE_FILE})
    ADD_DEFINITIONS(${QT_DEFINITIONS})

    QT4_WRAP_UI (patt_gui_UI_SRCS ${patt_gui_ui_SRCS})
    QT4_WRAP_CPP (patt_gui_MOC_SRCS ${patt_gui_HEADERS})
    QT4_ADD_RESOURCES(patt_gui_RESOURCES_RCC res/icons.qrc)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

    ADD_EXECUTABLE(patt-gui ${PATT_GUI_WIN32_FLAG}
        main.cpp
        ${patt_gui_SRCS}
        ${patt_gui_MOC_SRCS}
        ${patt_gui_UI_SRCS}
        ${patt_gui_RESOURCES_RCC}
    )
    TARGET_LINK_LIBRARIES(patt-gui PRIVATE ${QT_LIBRARIES})
ENDIF()

IF(WIN32 AND TARGET patt-gui AND MSVC)
    SET_TARGET_PROPERTIES(patt-gui PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
ENDIF()

INSTALL (TARGETS patt-gui RUNTIME DESTINATION bin COMPONENT gui)

# ensure all necessary dependencies are included in distribution
IF (WIN32 AND DEFINED QT_BINARY_DIR AND EXISTS "${QT_BINARY_DIR}")
    FILE (GLOB PATT_GUI_QT_DLLS 
        "${QT_BINARY_DIR}/QtCore*.dll"
        "${QT_BINARY_DIR}/QtGui*.dll" 
        "${QT_BINARY_DIR}/QtNetwork*.dll"
        "${QT_BINARY_DIR}/QtSvg*.dll"
    )

    INSTALL (FILES ${PATT_GUI_QT_DLLS} COMPONENT gui DESTINATION bin)
    
ENDIF ()
