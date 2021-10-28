cmake_minimum_required(VERSION 3.16)

project(traces)


find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE srcs "src/*.cpp")

add_executable(traces ${srcs})
target_link_libraries(traces PRIVATE OpenGL::GL GLEW::glew glfw)
set_property(TARGET traces PROPERTY CXX_STANDARD 17)

set(libSrcs ${srcs})
list(FILTER libSrcs EXCLUDE REGEX ".*src/main.cpp")
list(FILTER libSrcs EXCLUDE REGEX ".*src/Window.cpp")
list(APPEND libSrcs "src/traces_render.cpp")

add_library(traces_render STATIC ${libSrcs})
target_link_libraries(traces_render PRIVATE OpenGL::GL GLEW::glew)
set_target_properties(traces_render PROPERTIES PUBLIC_HEADER "src/traces_render.h")
set_property(TARGET traces_render PROPERTY CXX_STANDARD 17)
install(TARGETS traces_render ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)

add_executable(test "src/testMain.c")
target_link_libraries(test PRIVATE OpenGL::GL GLEW::glew glfw traces_render)

