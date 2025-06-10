cmake_minimum_required(VERSION 3.16)

project(3DLevED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Set the runtime output directory to 'bin'
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# Copy shader files to the appropriate location
if(APPLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME "3DLevED")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.3dleved")
    set(MACOSX_BUNDLE TRUE)

    set(SHADER_BUNDLE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/3DLevED.app/Contents/Resources")
    file(COPY shaders DESTINATION ${SHADER_BUNDLE_PATH})
else()
    file(COPY shaders DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

include(FetchContent)

# ----------- Dependencies ------------ #

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glm)

# ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.90.3
)
FetchContent_MakeAvailable(imgui)

# ----------- Manual GLAD Setup ------------ #

add_library(glad STATIC
  external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
  ${PROJECT_SOURCE_DIR}/external/glad/include
)

# ----------- ImGui Setup ------------ #

set(IMGUI_SOURCES
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# ----------- Source Files ------------ #

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS src/*.h)

add_executable(3DLevED MACOSX_BUNDLE
  ${SRC_FILES}
  ${HEADER_FILES}
  ${IMGUI_SOURCES}
)

target_include_directories(3DLevED
  PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    external/glad/include
)

# ----------- Link Libraries ------------ #

target_link_libraries(3DLevED
  PRIVATE
    glad
    glfw
)

if(WIN32)
    target_link_libraries(3DLevED PRIVATE opengl32)
elseif(APPLE)
    target_link_libraries(3DLevED PRIVATE "-framework OpenGL")
elseif(UNIX)
    target_link_libraries(3DLevED PRIVATE GL dl pthread)
endif()
