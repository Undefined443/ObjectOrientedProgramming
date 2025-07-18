# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.20)

# 项目名称、版本、语言
project(
    ObjectOrientedProgramming
    VERSION 1.0
    LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows 指定 Qt MinGW 路径
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.5.0/mingw_64")
endif()

# 导入库
find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)
find_package(nlohmann_json REQUIRED)

# （Qt）自动生成 UI、MOC、RCC 文件
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 引入源文件和头文件
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "include/*.h")

# 创建可执行文件
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ObjectOrientedProgramming MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(ObjectOrientedProgramming ${PROJECT_SOURCES})
endif()

# 设置头文件目录
target_include_directories(ObjectOrientedProgramming PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 链接库
target_link_libraries(ObjectOrientedProgramming PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(ObjectOrientedProgramming PRIVATE nlohmann_json::nlohmann_json)

# 设置目标属性
set_target_properties(
    ObjectOrientedProgramming
    PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER app.undefined443.dev
               MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               MACOSX_BUNDLE ON
               WIN32_EXECUTABLE ON)

# 完成 Qt6 可执行文件的最终配置
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ObjectOrientedProgramming)
endif()

# 设置资源文件目录
if(APPLE)
    set(RESOURCE_PATH "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app/Contents/MacOS")
elseif(WIN32)
    set(RESOURCE_PATH "${CMAKE_BINARY_DIR}")
else()
    set(RESOURCE_PATH "${CMAKE_BINARY_DIR}")
    # message(FATAL_ERROR "Unsupported operating system")
endif()

# 拷贝资源文件
file(COPY ${CMAKE_SOURCE_DIR}/resources/data/ DESTINATION ${RESOURCE_PATH})
file(COPY ${CMAKE_SOURCE_DIR}/resources/images/ DESTINATION ${RESOURCE_PATH}/images)

# 定义平台特定的安装路径
if(APPLE)
    set(APP_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/Applications")
    set(RESOURCE_INSTALL_DIR "${APP_INSTALL_DIR}/${PROJECT_NAME}.app/Contents/Resources")
elseif(WIN32)
    set(APP_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
    set(RESOURCE_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
else()
    set(APP_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
    set(RESOURCE_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
endif()

# 安装可执行文件
install(
    TARGETS ObjectOrientedProgramming
    BUNDLE DESTINATION "${APP_INSTALL_DIR}"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# 安装资源文件
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/
    DESTINATION "${RESOURCE_INSTALL_DIR}")