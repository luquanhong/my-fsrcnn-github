# 指定CMake最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(qt_opencv_camera_app)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 查找Qt5组件
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# 查找OpenCV库
#find_package(OpenCV 4.0 REQUIRED)
# 设置 OpenCV 库的路径 brew info opencv 找到路径
#set(OpenCV_INCLUDE_DIRS /usr/local/Cellar/opencv/4.9.0_8.reinstall/include/opencv4)
#set(OpenCV_LIBS_DIR  /usr/local/Cellar/opencv/4.9.0_8.reinstall/lib)
## 查找 OpenCV 库文件
#file(GLOB OpenCV_LIBS ${OpenCV_LIBS_DIR}/*.dylib)


# 尝试默认查找 OpenCV
find_package(OpenCV QUIET)

if(NOT OpenCV_FOUND)
    message(STATUS "未找到 OpenCV，尝试手动指定路径...")
    # 手动指定可能的 OpenCV 安装路径
    set(POSSIBLE_OPENCV_PATHS
            "/path/to/opencv"
            # 可以添加更多可能的路径
            " /usr/local/Cellar/opencv/4.9.0_8.reinstall"
    )

    foreach(PATH ${POSSIBLE_OPENCV_PATHS})
        set(OpenCV_DIR "${PATH}/lib/cmake/opencv4")
        find_package(OpenCV QUIET)
        if(OpenCV_FOUND)
            message(STATUS "在 ${PATH} 找到 OpenCV")
            break()
        endif()
    endforeach()
endif()

# 检查是否最终找到 OpenCV
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "无法找到 OpenCV，请检查安装路径或手动指定 OpenCV_DIR")
endif()


# 包含头文件目录
include_directories(include ${OpenCV_INCLUDE_DIRS})

# 自动生成moc文件
set(CMAKE_AUTOMOC ON)
# 自动生成ui文件对应的头文件
set(CMAKE_AUTOUIC ON)
# 自动生成资源文件对应的代码
set(CMAKE_AUTORCC ON)

# 源文件列表
set(SOURCES
        main.cpp
        mainwindow.cpp
)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接Qt5库
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

# 链接OpenCV库
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})