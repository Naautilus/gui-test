cmake_minimum_required(VERSION 3.16)
project(new_engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -flto=12 -fomit-frame-pointer -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -Og -march=native -flto=12 -fomit-frame-pointer -fdiagnostics-color=always -g")

# Collect all .cpp files recursively
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(imgui
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PRIVATE
    external/imgui
    external/imgui/backends
)

# Create the executable
add_executable(main ${SRC_FILES})
target_link_libraries(main PRIVATE imgui glfw3 opengl32 m)

# Include ImGui headers
target_include_directories(main PRIVATE
    external/imgui
    external/imgui/backends
)