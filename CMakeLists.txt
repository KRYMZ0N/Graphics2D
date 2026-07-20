cmake_minimum_required(VERSION 3.16)
project(PixelGraphicsLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ALlow other architectures to use
list(APPEND CMAKE_PREFIX_PATH
    "/usr/local/opt/sdl2"
    "/usr/local/opt/sdl2_image"
    "/opt/homebrew/opt/sdl2"
    "/opt/homebrew/opt/sdl2_image"
)

# Resolve SDL2 and SDL2_image as reusable dependencies.
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
)

file(GLOB_RECURSE LIB_HEADERS CONFIGURE_DEPENDS
    "src/*.h"
)

add_library(PixelGraphics SHARED ${LIB_SOURCES} ${LIB_HEADERS})

target_include_directories(PixelGraphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(PixelGraphics
    PUBLIC
        SDL2::SDL2
        SDL2_image::SDL2_image
)

set_target_properties(PixelGraphics PROPERTIES
    EXPORT_NAME PixelGraphics
    POSITION_INDEPENDENT_CODE ON
)