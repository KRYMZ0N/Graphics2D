cmake_minimum_required(VERSION 3.16)
project(PixelGraphicsLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Source and header files
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE LIB_HEADERS CONFIGURE_DEPENDS "src/*.h")

# Example CMake configuration for your graphics library target
# Use the automatic file GLOB variable so any new .cpp file you add is picked up instantly!
add_library(PixelGraphics SHARED
    ${LIB_SOURCES}
    ${LIB_HEADERS}
)
target_compile_definitions(PixelGraphics PRIVATE PIXEL_GRAPHICS_EXPORTS)

# 2. Include directories
target_include_directories(PixelGraphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        "C:/Users/KRYMZ0N05/Documents/Undergrad/modernity/Engine2D/external/PixelGraphics/include"
)

target_link_libraries(PixelGraphics
    PRIVATE
        "C:/Users/KRYMZ0N05/Documents/Undergrad/modernity/Engine2D/external/PixelGraphics/lib/PixelGraphics.lib"
        "C:/Users/KRYMZ0N05/Documents/Undergrad/modernity/Engine2D/external/PixelGraphics/lib/SDL2.lib"
        "C:/Users/KRYMZ0N05/Documents/Undergrad/modernity/Engine2D/external/PixelGraphics/lib/SDL2_image.lib"
)
# 3. Manually link your library against your Engine2D external lib folder paths or local SDL paths
# (Update this path to point to your actual SDL2 include directory if your compiler can't find SDL.h)
# target_include_directories(PixelGraphics PRIVATE "C:/path/to/your/sdl2/include")

# Since we aren't using find_package, remove the SDL2:: targets 
# and let Visual Studio find them via standard library linking if needed, 
# or point target_link_libraries directly to your .lib files:

set_target_properties(PixelGraphics PROPERTIES
    EXPORT_NAME PixelGraphics
    POSITION_INDEPENDENT_CODE ON
)