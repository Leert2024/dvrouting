cmake_minimum_required(VERSION 3.10)
project(dvrouting C)

# 设置 C 标准为 gnu89，对应 gcc -std=gnu89
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)  # 启用 GNU 扩展，等效于 -std=gnu89

# 添加编译选项，禁用隐式声明警告
add_compile_options(-Wno-implicit-int)
add_compile_options(-Wno-implicit-function-declaration)

# 设置 include 目录
include_directories(${PROJECT_SOURCE_DIR}/inc)

# 源文件列表
set(SOURCES
    src/prog3.c
    src/common.c
    src/node0.c
    src/node1.c
    src/node2.c
    src/node3.c
)

# 创建可执行文件
add_executable(run ${SOURCES})