cmake_minimum_required(VERSION 3.5)

project (eziengine VERSION 1.0.0)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/SDL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dependencies/glm)

file(GLOB SOURCES 
"${CMAKE_CURRENT_LIST_DIR}/source/graphics/*.cpp"
"${CMAKE_CURRENT_LIST_DIR}/source/reflection/*.cpp" 
"${CMAKE_CURRENT_LIST_DIR}/source/serialization/*.cpp")

add_library(eziengine STATIC ${SOURCES})

target_link_libraries(eziengine glm_static SDL2-static)

target_include_directories(eziengine PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dependencies/glm)

target_include_directories(eziengine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

install(TARGETS eziengine
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  FILES_MATCHING PATTERN "*.h"
)




