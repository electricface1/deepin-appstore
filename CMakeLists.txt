cmake_minimum_required(VERSION 3.2)
project(Shell)
set(SHELL_VERSION 4.0.8)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Werror -Wno-unused-variable -Wfatal-errors")

#add_definitions(-D DEBUG_LASTORE=1)
#add_definitions(-D DEBUG_LOCAL_REQUEST=1)

# deepin-wm and mutter support the _NET_WM_OPAQUE_REGION
# However this optimization causes uncomposited area to be black during window resizing
# keep it disabled for now
add_definitions(-D USE_OPAQUE_REGION_OPTIMIZATION=0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebChannel REQUIRED)

set(CEF_ROOT "/usr/lib/cef-1650")
set(CEF_INCLUDE_DIRS "${CEF_ROOT}")
set(CEF_LIBRARIES "-L${CEF_ROOT}")

set(CMAKE_INSTALL_RPATH "${CEF_ROOT}")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

add_definitions(-DCEF_ROOT="${CEF_ROOT}")

include_directories(AFTER ${CEF_INCLUDE_DIRS})
include_directories(AFTER "CEFView")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_KEYWORDS")

pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(GTK REQUIRED gtk+-2.0)
pkg_search_module(X11 REQUIRED x11)
pkg_search_module(X11Ext REQUIRED xext)
pkg_search_module(GIO REQUIRED gio-unix-2.0)

include_directories(AFTER ${GLIB_INCLUDE_DIRS})
include_directories(AFTER ${X11_INCLUDE_DIRS})
include_directories(AFTER ${X11Ext_INCLUDE_DIRS})
include_directories(AFTER ${GIO_INCLUDE_DIRS})
include_directories(AFTER ${GTK_INCLUDE_DIRS})


set(SOURCE_FILES main.cpp
                 StupidWindow.cpp StupidWindow.h
                 MainWindow.cpp MainWindow.h
                 Shell.cpp Shell.h
                 Bridge.cpp Bridge.h
                 FilterMouseMove.cpp FilterMouseMove.h
                 TextBrowser.cpp TextBrowser.h
                 AboutWindow.cpp AboutWindow.h
                 DBusInterface.cpp DBusInterface.h)

set(TOOLTIP_FILES ToolTip.cpp)
set(DBUSMENU_FILES dbusmenu.cpp dbusmenu.h dbusmenumanager.cpp dbusmenumanager.h)
set(DBUS_BACKEND_FILES LAStoreBridge.cpp LAStoreBridge.h)

set(LINK_LIBS
        Qt5::Widgets
        Qt5::X11Extras
        Qt5::DBus
        Qt5::Gui
        Qt5::Network
        Qt5::WebChannel
        ${GLIB_LIBRARIES}
        ${X11_LIBRARIES}
        ${X11Ext_LIBRARIES}
        ${GIO_LIBRARIES}
        ${CEF_LIBRARIES}
        ${GTK_LIBRARIES}
        "${CEF_LIBRARIES}/obj.target"
)

add_definitions(-DSHELL_VERSION="${SHELL_VERSION}")

set(WEB_WIDGET_FILES
  CEFView/cef_init.cc
  CEFView/cef_init.h

  CEFView/browser_handler.cc
  CEFView/browser_handler.h

  CEFView/renderer_handler.cc
  CEFView/renderer_handler.h

  CEFView/webview.cpp
  CEFView/webview.h

  CEFView/webchannel_transport.cc
  CEFView/webchannel_transport.h
  )

add_definitions(-DBUILD_WITH_CEF=1)
add_executable(deepin-appstore ${SOURCE_FILES} ${TOOLTIP_FILES} ${DBUSMENU_FILES} ${DBUS_BACKEND_FILES} ${WEB_WIDGET_FILES}
  res.qrc
  "/usr/include/lastore-daemon.h")

target_link_libraries(deepin-appstore -lcef ${LINK_LIBS} ${CEF_STANDARD_LIBS} -lcef -lcef_dll_wrapper)


install(TARGETS deepin-appstore
        DESTINATION "/usr/bin")
install(FILES support/deepin-appstore.desktop
        DESTINATION "/usr/share/applications")
install(DIRECTORY support/icons/
        DESTINATION "/usr/share/icons/")
install(DIRECTORY manual/
        DESTINATION "/usr/share/dman/deepin-appstore")
