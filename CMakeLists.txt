cmake_minimum_required(VERSION 3.10)
project(BPT_Project)

# 设置 C++ 标准（根据需要调整）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置可执行文件输出目录为项目根目录下的 bin 文件夹
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# 添加头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/bpt/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ticket/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/map)

# 获取 bpt/src 下所有源文件（作为公共部分）
file(GLOB BPT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/bpt/src/*.cpp")
file(GLOB TICKET_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ticket/src/*.cpp")

# 获取 main 目录下所有源文件（每个文件包含一个 main 函数）
file(GLOB MAIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/main/*.cpp")

# 为每个 main 文件生成一个可执行文件
foreach(main_file ${MAIN_FILES})
    # 获取文件名（不含扩展名），作为可执行文件名
    get_filename_component(exe_name ${main_file} NAME_WE)
    # 创建可执行文件：公共源文件 + 当前 main 文件
    add_executable(${exe_name} ${BPT_SOURCES} ${TICKET_SOURCES} ${main_file})
    # 可选：打印构建信息
    # message(STATUS "Adding executable '${exe_name}' from ${main_file}")
endforeach()