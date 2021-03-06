FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
SET( QT_USE_QT3SUPPORT TRUE )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTNETWORK TRUE )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_BINARY_DIR} 
    ${QT_INCLUDES}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTNETWORK_INCLUDE_DIR})

INCLUDE(${QT_USE_FILE})


# Program information
SET(LATT_GUI_VERSION_MAJOR "${CPACK_PACKAGE_VERSION_MAJOR}")
SET(LATT_GUI_VERSION_MINOR "${CPACK_PACKAGE_VERSION_MINOR}")
SET(LATT_GUI_VERSION_PATCH "${CPACK_PACKAGE_VERSION_PATCH}")
SET(LATT_GUI_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/latt_gui.hpp.in"
   "${CMAKE_CURRENT_SOURCE_DIR}/latt_gui.hpp" IMMEDIATE @ONLY )

SET (latt_gui_SRCS main.cpp lattmainwindow.cpp qcompile.cpp)


IF (Qt5_POSITION_INDEPENDENT_CODE OR Qt4_POSITION_INDEPENDENT_CODE)
  SET (CMAKE_POSITION_INDEPENDENT_CODE ON)
ENDIF ()   
   
QT4_WRAP_UI(UI_SRCS lattmainwindow.ui advancedoutput.ui)
Qt4_WRAP_CPP(MOC_SRCS latt_gui.hpp lattmainwindow.h qcompile.h)
QT4_ADD_RESOURCES(RESOURCES_RCC res/icons.qrc)

IF(WIN32) # Check if we are on Windows

    # configure for windows icon
    ENABLE_LANGUAGE(RC)
    CONFIGURE_FILE ("res/windows/latt-gui.rc.in" "res/windows/latt-gui.rc" IMMEDIATE @ONLY )
    LIST(APPEND latt_gui_SRCS res/windows/latt-gui.rc)

  ADD_EXECUTABLE(latt-gui WIN32 ${latt_gui_SRCS} 
    ${MOC_SRCS} ${UI_SRCS} ${RESOURCES_RCC})
  if(MSVC) # Check if we are using the Visual Studio compiler
    set_target_properties(latt-gui PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
  else()
    message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
  endif()
elseif(UNIX)
  # Nothing special required
  ADD_EXECUTABLE(latt-gui main.cpp lattmainwindow.cpp qcompile.cpp 
    ${MOC_SRCS} ${UI_SRCS} ${RESOURCES_RCC})
else()
  message(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
ENDIF()

INSTALL (TARGETS latt-gui RUNTIME DESTINATION bin COMPONENT gui)

TARGET_LINK_LIBRARIES(latt-gui ${QT_LIBRARIES} m)

# ensure all necessary dependencies are included in distribution
IF (WIN32)
    FILE (GLOB LATT_GUI_QT_DLLS 
        "${QT_BINARY_DIR}/QtCore*.dll"
        "${QT_BINARY_DIR}/QtGui*.dll" 
        "${QT_BINARY_DIR}/QtNetwork*.dll"
    )

    INSTALL (FILES ${LATT_GUI_QT_DLLS} COMPONENT gui DESTINATION bin)
    
ENDIF (WIN32)