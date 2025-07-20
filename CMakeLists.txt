# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.20)

# 设置项目信息
project(
    ObjectOrientedProgramming
    VERSION 5.5.1
    LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 导入库
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)
find_package(nlohmann_json CONFIG REQUIRED)

# Qt 相关设置
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 引入源文件
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "include/*.h")

# 创建可执行文件
qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})

# 设置 include 目录
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Charts)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# 设置目标属性
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)

# 完成 Qt6 可执行文件的最终配置
qt_finalize_executable(${PROJECT_NAME})

# 拷贝资源文件
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR}/resources)

# ------------------------------------------------------------------------------
# 安装与打包

# 设置包信息
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Object Oriented Programming Project")
set(CPACK_PACKAGE_VENDOR "undefined443")

# 设置包格式
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

# 设置安装组件
install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/resources DESTINATION .)

# 使用 CPack 打包
include(CPack)
