# 指定CMake最低版本要求
cmake_minimum_required(VERSION 3.10)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置项目名称
project(qt_opencv_camera_app)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt5组件

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64" AND APPLE)
# 执行 shell 命令获取 Qt 安装路径, 因为要区分86和arm64架构
execute_process(
        COMMAND /opt/homebrew/bin/brew --prefix qt@5
        OUTPUT_VARIABLE QT_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# 设置 CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH ${QT_DIR})
endif() # 结束 CMAKE_SYSTEM_PROCESSOR 语句

find_package(Qt5 COMPONENTS Widgets REQUIRED)


# 添加 OpenCV 子模块作为子项目
# 关闭通用非必要功能
set(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_python2 OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_python3 OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_js OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_ts OFF CACHE BOOL "" FORCE)
# 关闭 OpenEXR 相关功能
set(WITH_OPENEXR OFF CACHE BOOL "" FORCE)
#set(BUILD_opencv_external OFF CACHE BOOL "" FORCE)

# 关闭第三方库支持
set(WITH_FFMPEG OFF CACHE BOOL "" FORCE)
set(WITH_GSTREAMER OFF CACHE BOOL "" FORCE)
set(WITH_VTK OFF CACHE BOOL "" FORCE)

# 根据平台关闭特定功能
if(WIN32)
    set(WITH_CUDA OFF CACHE BOOL "" FORCE)
    set(BUILD_opencv_cudaarithm OFF CACHE BOOL "" FORCE)
    set(BUILD_opencv_cudabgsegm OFF CACHE BOOL "" FORCE)
    # 可添加更多 Windows 特定的关闭选项
elseif(APPLE)
    set(WITH_QUICKTIME OFF CACHE BOOL "" FORCE)
    # 可添加更多 macOS 特定的关闭选项
elseif(UNIX)
    set(WITH_LIBV4L OFF CACHE BOOL "" FORCE)
    set(WITH_GTK OFF CACHE BOOL "" FORCE)
    # 可添加更多 Linux 特定的关闭选项
endif()

add_subdirectory(external/opencv)

# 添加 ncnn 子模块作为子项目
set(NCNN_VULKAN OFF CACHE BOOL "Disable Vulkan support" FORCE)  # 若不需要 Vulkan 支持可关闭
add_subdirectory(external/ncnn)

# 源文件列表
set(SOURCES
        main.cpp
        mainwindow.cpp
        CameraWidget.cpp
        CameraWidget.h
        FrameCaptureThread.cpp
        FrameCaptureThread.h
        FrameRenderThread.cpp
        FrameRenderThread.h
        FrameFilterThread.cpp
)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 包含 OpenCV 头文件 因为正常编译opencv有一个 make release来分布include和lib的过程， addsub没有这个过程，所以需要手动添加
target_include_directories(${PROJECT_NAME} PRIVATE external/opencv/include)
# find opencv module headers and include them
macro(find_opencv_headers return_list)
    file(GLOB new_list ${PROJECT_SOURCE_DIR}/external/opencv/modules/**/*)
    get_filename_component(CMAKE_CURRENT_LIST_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
    set(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path}/include)
    ENDFOREACH()
    list(REMOVE_DUPLICATES dir_list)
    set(${return_list} ${dir_list})
endmacro()
find_opencv_headers(open_cv_headers)
set(OpenCV_INCLUDE_DIRS ${open_cv_headers})
include_directories(${OpenCV_INCLUDE_DIRS})

# include generated opencv headers
include_directories(${PROJECT_SOURCE_DIR}/opencv/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/opencv)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
#end opencv

# 包含 ncnn 头文件
target_include_directories(${PROJECT_NAME} PRIVATE external/ncnn/src)

# 链接Qt5库
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

# 链接OpenCV库
target_link_libraries(${PROJECT_NAME} opencv_core opencv_imgproc opencv_highgui)

# 链接 ncnn 库
target_link_libraries(${PROJECT_NAME} ncnn)

