cmake_minimum_required(VERSION 3.24)

project(squirrel LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
pkg_check_modules(SDL3_IMAGE REQUIRED IMPORTED_TARGET sdl3-image)

file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# We migrate the entrypoint to C++ (src/main.cpp). If src/main.c exists, don't build it.
list(FILTER C_SOURCES EXCLUDE REGEX ".*/src/main\\.c$")

add_executable(squirrel
  ${C_SOURCES}
  ${CPP_SOURCES}
)

target_compile_options(squirrel PRIVATE
  $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Wpedantic>
  $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Wpedantic>
)

target_link_libraries(squirrel PRIVATE
  PkgConfig::SDL3
  PkgConfig::SDL3_IMAGE
)
