cmake_minimum_required(VERSION 3.16)

project(traces)


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE srcs "src/*.cpp")

add_executable(traces ${srcs})
set_property(TARGET traces PROPERTY CXX_STANDARD 17)
target_link_libraries(traces PRIVATE OpenGL::GL GLEW::glew glfw)

set(libSrcs ${srcs})
list(FILTER libSrcs EXCLUDE REGEX ".*src/main.cpp")
list(FILTER libSrcs EXCLUDE REGEX ".*src/Window.cpp")
list(APPEND libSrcs "src/traces_render.cpp")

add_library(traces_render STATIC ${libSrcs})
target_link_libraries(traces_render PRIVATE OpenGL::GL GLEW::glew)
set_property(TARGET traces_render PROPERTY CXX_STANDARD 17)

add_executable(test "src/testMain.c")
target_link_libraries(test PRIVATE OpenGL::GL GLEW::glew glfw traces_render)
