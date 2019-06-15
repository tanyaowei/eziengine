cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 14)

project (eziengine VERSION 1.0.0)

add_subdirectory(dependencies/SDL)
add_subdirectory(dependencies/glm)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/dependencies/SDL/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SDL/src/video/khronos)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SDL/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/glm)

include_directories(include)

file(GLOB SOURCES 
"source/graphics/*.cpp"
"source/reflection/*.cpp" 
"source/serialization/*.cpp")

add_library(eziengine STATIC ${SOURCES})

target_link_libraries(eziengine glm_static SDL2-static)
