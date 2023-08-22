cmake_minimum_required(VERSION 3.0)

project(devon-particles
  VERSION 0.1.10
  LANGUAGES CXX
  DESCRIPTION "3D Particle simulator"
)

set(CMAKE_CXX_STANDARD 11)
set(OpenGL_GL_PREFERENCE GLVND)

add_subdirectory("lib/external/glad")
add_subdirectory("lib/external/glm")

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(IMGUI_SOURCES
  imgui/imgui_impl_glfw.cpp
  imgui/imgui_impl_opengl3.cpp
  imgui/imgui.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui_tables.cpp
)

set(PROJ_SOURCES
  src/devon.cpp
  src/particle.cpp
  src/shader.cpp
)

set(PROJ_HEADERS
  src/devon.hpp
  src/particle.hpp
  src/shader.hpp
  src/scene.hpp
)

set(PROJ_SHADERS
  src/shaders/particle.vert
  src/shaders/particle.frag
  src/shaders/box.vert
  src/shaders/box.frag
)

file(COPY ${PROJ_SHADERS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

add_executable(devon-particles
  ${PROJ_SOURCES}
  ${PROJ_HEADERS}
  ${IMGUI_SOURCES}
)

target_include_directories(devon-particles PRIVATE imgui src)

target_link_libraries(devon-particles glfw glad glm GL)

