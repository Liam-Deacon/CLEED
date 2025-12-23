# Program information
SET(LATT_GUI_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(LATT_GUI_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(LATT_GUI_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(LATT_GUI_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/latt_gui.hpp.in"
   "${CMAKE_CURRENT_SOURCE_DIR}/latt_gui.hpp" IMMEDIATE @ONLY )

SET (latt_gui_SRCS
    main.cpp
    lattmainwindow.cpp
    qcompile.cpp
    lattmainwindow.ui
    advancedoutput.ui
    res/icons.qrc
)

IF(WIN32) # Check if we are on Windows

    # configure for windows icon
    ENABLE_LANGUAGE(RC)
    CONFIGURE_FILE ("res/windows/latt-gui.rc.in" "res/windows/latt-gui.rc" IMMEDIATE @ONLY )
    LIST(APPEND latt_gui_SRCS res/windows/latt-gui.rc)

    SET(LATT_GUI_WIN32_FLAG WIN32)
ELSEIF(UNIX)
    # Nothing special required
    SET(LATT_GUI_WIN32_FLAG "")
ELSE()
    MESSAGE(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
ENDIF()

IF (TARGET Qt6::Core OR TARGET Qt5::Core)
    SET(CMAKE_AUTOMOC ON)
    SET(CMAKE_AUTOUIC ON)
    SET(CMAKE_AUTORCC ON)

    ADD_EXECUTABLE(latt-gui ${LATT_GUI_WIN32_FLAG} ${latt_gui_SRCS})
    TARGET_INCLUDE_DIRECTORIES(latt-gui PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    TARGET_LINK_LIBRARIES(latt-gui PRIVATE ${QT_LIBRARIES} m)
ELSE()
    FIND_PACKAGE(Qt4 REQUIRED)
    INCLUDE(${QT_USE_FILE})
    ADD_DEFINITIONS(${QT_DEFINITIONS})

    INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_INCLUDES}
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTGUI_INCLUDE_DIR}
        ${QT_QTNETWORK_INCLUDE_DIR}
    )

    QT4_WRAP_UI(UI_SRCS lattmainwindow.ui advancedoutput.ui)
    QT4_WRAP_CPP(MOC_SRCS latt_gui.hpp lattmainwindow.h qcompile.h)
    QT4_ADD_RESOURCES(RESOURCES_RCC res/icons.qrc)

    ADD_EXECUTABLE(latt-gui ${LATT_GUI_WIN32_FLAG}
        main.cpp lattmainwindow.cpp qcompile.cpp
        ${MOC_SRCS} ${UI_SRCS} ${RESOURCES_RCC}
    )
    TARGET_LINK_LIBRARIES(latt-gui PRIVATE ${QT_LIBRARIES} m)
ENDIF()

IF(WIN32 AND TARGET latt-gui AND MSVC)
    SET_TARGET_PROPERTIES(latt-gui PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
ENDIF()

INSTALL (TARGETS latt-gui RUNTIME DESTINATION bin COMPONENT gui)

# ensure all necessary dependencies are included in distribution
IF (WIN32 AND DEFINED QT_BINARY_DIR AND EXISTS "${QT_BINARY_DIR}")
    FILE (GLOB LATT_GUI_QT_DLLS 
        "${QT_BINARY_DIR}/QtCore*.dll"
        "${QT_BINARY_DIR}/QtGui*.dll" 
        "${QT_BINARY_DIR}/QtNetwork*.dll"
    )

    INSTALL (FILES ${LATT_GUI_QT_DLLS} COMPONENT gui DESTINATION bin)
    
ENDIF ()
