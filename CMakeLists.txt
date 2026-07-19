cmake_minimum_required(VERSION 3.12)
project(PixelGraphicsLibrary LANGUAGES CXX)

# Force C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find Dependencies (SDL2 and SDL2_image)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# 2. Automatically find your library source files inside the src folder
file(GLOB_RECURSE LIB_SOURCES 
    "src/*.cpp" 
    "src/*.h"
)

# 3. Define this project as a reusable STATIC LIBRARY (not an app)
add_library(PixelGraphics STATIC ${LIB_SOURCES})

# 4. Include Directories
target_include_directories(PixelGraphics PUBLIC 
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 5. Link Libraries
target_link_libraries(PixelGraphics PUBLIC 
    ${SDL2_LIBRARIES} 
    ${SDL2_IMAGE_LIBRARIES}
)