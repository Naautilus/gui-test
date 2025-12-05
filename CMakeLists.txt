cmake_minimum_required(VERSION 3.16)
project(new_engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -march=native -fomit-frame-pointer -fdiagnostics-color=always -lws2_32 -lmswsock -lwsock32")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -O0 -march=native -fomit-frame-pointer -fdiagnostics-color=always -g -lws2_32 -lmswsock -lwsock32")

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

add_library(implot
    external/implot/implot.cpp
    external/implot/implot_items.cpp
    external/implot/implot.h
    external/implot/implot_internal.h
)

target_include_directories(implot PRIVATE
    external/implot
    external/imgui
)

add_library(asio
    external/asio/include/asio.hpp
)

set_target_properties(asio PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(asio PRIVATE
    external/asio/include
)

target_link_libraries(asio PRIVATE ws2_32)

########

add_library(cppcodec
    external/cppcodec/cppcodec/base64_default_rfc4648.hpp
)

set_target_properties(cppcodec PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(cppcodec PRIVATE
    external/cppcodec/cppcodec
    external/cppcodec/cppcodec/detail
)

# Create the executable
add_executable(main ${SRC_FILES})
target_link_libraries(main PRIVATE imgui glfw3 opengl32 m implot asio cppcodec)

# Include ImGui headers
target_include_directories(main PRIVATE
    external/imgui
    external/imgui/backends
    external/implot
    external/asio/include
    external/cppcodec/cppcodec
)