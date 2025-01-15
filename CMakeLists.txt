set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.10)
project(MazeGame)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# macOS specific - 使用 Homebrew 路径
if(APPLE)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

# SDL2 配置
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# 包含目录
include_directories(include)

# 源文件
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Maze.cpp
    src/Player.cpp
    src/Utils.cpp
    src/Effects.cpp  # 确保这行存在
    src/TextRenderer.cpp
)

# 可执行文件
add_executable(MazeGame ${SOURCES})

# 链接库
target_link_libraries(MazeGame SDL2 SDL2_ttf)

# 复制关卡文件到构建目录
file(COPY ${CMAKE_SOURCE_DIR}/levels DESTINATION ${CMAKE_BINARY_DIR})

# 创建 save 目录
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/save)