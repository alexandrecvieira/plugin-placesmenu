cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)
option(PULL_TRANSLATIONS "Pull translations" ON)
option(WITH_SCREENSAVER_FALLBACK "Include support for converting the deprecated 'screensaver' plugin to 'quicklaunch'. This requires the lxqt-leave (lxqt-session) to be installed in runtime." ON)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. C++11 support is required")
endif()

macro(setByDefault VAR_NAME VAR_VALUE)
  if(NOT DEFINED ${VAR_NAME})
    set (${VAR_NAME} ${VAR_VALUE})
  endif(NOT DEFINED ${VAR_NAME})
endmacro()

# use gcc visibility feature to decrease unnecessary exported symbols
if (CMAKE_COMPILER_IS_GNUCXX)
  # set visibility to hidden to hide symbols, unlesss they're exporeted manually in the code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -fno-exceptions")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-no-undefined")
endif()

#########################################################################

add_definitions (-Wall)
include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(KF5WindowSystem REQUIRED)

find_package(lxqt REQUIRED)
find_package(lxqt-globalkeys REQUIRED)
find_package(lxqt-globalkeys-ui REQUIRED)


include(LXQtTranslate)


set(PLUGIN "placesmenu")

set(HEADERS
    placesmenu.h
    placesmenuconfiguration.h
)

set(SOURCES
    placesmenu.cpp
    placesmenuconfiguration.cpp
)

set(UIS
    placesmenuconfiguration.ui
)

set(LIBRARIES
    ${LIBRARIES}
    Qt5Xdg
)

include ("cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
