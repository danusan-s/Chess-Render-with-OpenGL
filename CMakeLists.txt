cmake_minimum_required(VERSION 4.0)
project(ChessRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Include dirs
include_directories(
  ${CMAKE_SOURCE_DIR}/include
  /usr/local/include/freetype2  # Explicit FreeType header path
)

# Add source files
file(GLOB SRC_CPP ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Fetch dependencies
include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://github.com/freetype/freetype.git
  GIT_TAG VER-2-13-3 
)
FetchContent_MakeAvailable(freetype)

# GLM - Header-only math library
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Add executable
add_executable(render ${SRC_CPP})


# Link system OpenGL and libraries
if (APPLE)
    target_link_libraries(render PRIVATE "-framework OpenGL" glfw freetype)
target_include_directories(render PRIVATE ${glm_SOURCE_DIR})
elseif (UNIX)
    find_package(OpenGL REQUIRED)
    target_link_libraries(render PRIVATE OpenGL::GL glfw freetype dl pthread)
target_include_directories(render PRIVATE ${glm_SOURCE_DIR})
endif()

