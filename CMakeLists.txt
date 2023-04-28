cmake_minimum_required(VERSION 3.5)

project(ObjectOrientedProgramming VERSION 1.0 LANGUAGES CXX)

#configure_file(${CMAKE_SOURCE_DIR}/data/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/data/data.json ${CMAKE_BINARY_DIR}/data.json COPYONLY)
#configure_file(${CMAKE_SOURCE_DIR}/img/elevator.svg ${CMAKE_BINARY_DIR}/img/elevator.svg COPYONLY)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")            # If you are using Windows
    set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/mingw_64")  # You need to change this path to your Qt MinGW path
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_SOURCES
        main.cpp
        ElevatorShaft.cpp
        MainWindow.cpp
        building.cpp
        elevator.cpp
        floor.cpp
        monitor.cpp
        passenger.cpp
        Chart.cpp
        statistics.cpp
        LineChart.cpp
        )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ObjectOrientedProgramming
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET ObjectOrientedProgramming APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ObjectOrientedProgramming SHARED
                ${PROJECT_SOURCES}
                )
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ObjectOrientedProgramming
                ${PROJECT_SOURCES}
                )
    endif()
endif()

target_link_libraries(ObjectOrientedProgramming PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

set_target_properties(ObjectOrientedProgramming PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        )

install(TARGETS ObjectOrientedProgramming
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ObjectOrientedProgramming)
endif()

# 检查当前操作系统
if(APPLE)
    set(CONFIG_PATH "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/MacOS")
elseif(WIN32)
    set(CONFIG_PATH "${CMAKE_BINARY_DIR}")
else()
    message(FATAL_ERROR "Unsupported operating system")
endif()

# 将配置文件复制到相应的目录
file(COPY ${CMAKE_SOURCE_DIR}/data/config.json DESTINATION ${CONFIG_PATH})
file(COPY ${CMAKE_SOURCE_DIR}/data/data.json DESTINATION ${CONFIG_PATH})
file(COPY ${CMAKE_SOURCE_DIR}/img/elevator.svg DESTINATION "${CONFIG_PATH}/img")
