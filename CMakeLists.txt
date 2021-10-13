cmake_minimum_required(VERSION 3.16)

project(traces)



find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE srcs "src/*.cpp")

add_executable(traces ${srcs})
set_property(TARGET traces PROPERTY CXX_STANDARD 17)
target_link_libraries(traces PRIVATE OpenGL::GL GLEW::glew glfw)