cmake_minimum_required(VERSION 3.8)

project(qt_download_manager)

set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (UNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()

find_package(Qt5 COMPONENTS Core Widgets Network Gui PrintSupport REQUIRED)

include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5OpenGL_INCLUDE_DIRS})
include_directories(${Qt5SerialPort_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

FILE(GLOB FORMS "*.ui")
qt5_wrap_ui(FORMS_HEADERS ${FORMS})

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(${PROJECT_NAME}
        ${GUI_TYPE}
        ${SOURCES} ${HEADERS}
        ${FORMS_HEADERS}
        )
qt5_use_modules(${PROJECT_NAME} LINK_PRIVATE Widgets Core Gui Network)
