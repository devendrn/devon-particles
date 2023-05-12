cmake_minimum_required(VERSION 3.0)
project(devon-particles)

set(CMAKE_CXX_STANDARD 11)

# Not sure if this is where libGL is loaded from
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_subdirectory("${CMAKE_SOURCE_DIR}/lib/external/glad")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/external/glm")

find_package(glfw3 3.3 REQUIRED)

file(GLOB_RECURSE SRCS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE SHADERS "${CMAKE_SOURCE_DIR}/src/shaders/*")

# copy shader files into build
file(COPY ${SHADERS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

add_executable(devon-particles ${SRCS} ${HEADERS})
target_link_libraries(devon-particles glfw glad glm GL)
