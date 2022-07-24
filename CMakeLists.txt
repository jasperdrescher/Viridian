cmake_minimum_required(VERSION 3.11)

project(Viridian)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DUNICODE)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/MP>")
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Binaries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Libraries)
set(SUBMODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Submodules")
set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies")

add_executable(Game "Source/Viridian.cpp")

include_directories(Game PRIVATE "${SUBMODULES_DIR}/Tileson")

add_library(GLFW STATIC IMPORTED)
set_target_properties(GLFW PROPERTIES
  IMPORTED_LOCATION "${DEPENDENCIES_DIR}/GLFW/src/Debug/glfw3.lib"
  INTERFACE_INCLUDE_DIRECTORIES "${SUBMODULES_DIR}/GLFW/include"
)
target_link_libraries(Game GLFW)

add_library(GLAD STATIC IMPORTED)
set_target_properties(GLAD PROPERTIES
  IMPORTED_LOCATION "${DEPENDENCIES_DIR}/GLAD/Debug/glad.lib"
  INTERFACE_INCLUDE_DIRECTORIES "${DEPENDENCIES_DIR}/GLAD/include"
)
target_link_libraries(Game GLAD)

add_custom_command(
  TARGET Game
  POST_BUILD COMMAND ${CMAKE_COMMAND}
  -E copy_directory
  "${CMAKE_SOURCE_DIR}/Data"
  "$<TARGET_FILE_DIR:Game>/Data")